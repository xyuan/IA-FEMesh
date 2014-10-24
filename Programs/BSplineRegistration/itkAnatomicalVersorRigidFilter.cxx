/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkAnatomicalVersorRigidFilter.cxx,v $
Language:  C++
Date:      $Date: 2007/06/17 23:44:27 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkOrientImageFilter.h"
#include "itkAnatomicalVersorRigidFilter.h"
#include <itkIOCommon.h>
#include <itkCastImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkExtractImageFilter.h>
#include "itkMetaDataObject.h"
#include "itkProgressAccumulator.h"

#include <iostream>


namespace itk
{

    AnatomicalVersorRigidFilter::AnatomicalVersorRigidFilter()
        {

        m_NumberOfSpatialSamples = 100000;
        m_NumberOfIterations = 1000;
        m_TranslationScale = 1000.0;
        m_MaximumStepLength = 0.2;
        m_MinimumStepLength = 0.0001;
        m_RelaxationFactor = 0.5;
        m_BaseImage = 0;

        }



    void AnatomicalVersorRigidFilter::Update ( )
        {

        std::cout << "AnatomicalVersorRigidFilter()...." << std::endl;


        MetricTypePointer         metric        = MetricType::New();
        OptimizerTypePointer      optimizer     = OptimizerType::New();
        InterpolatorTypePointer   interpolator  = InterpolatorType::New();
        RegistrationTypePointer   registration  = RegistrationType::New();

        ExtractFilterTypePointer extractImageFilter     = ExtractFilterType::New();

        MovingImageRegionType    movingRegion  = m_MovingImage->GetLargestPossibleRegion();
        MovingImageSizeType      movingSize    = movingRegion.GetSize();
        MovingImageSpacingType  movingSpacing = m_MovingImage->GetSpacing();
        MovingImagePointType    movingOrigin  = m_MovingImage->GetOrigin();


        std::cout << "Region: " << movingRegion << std::endl;
        std::cout << "Spacing: " << movingSpacing << std::endl;
        std::cout << "Origin: " << movingOrigin << std::endl;

        movingSize[3] = 0;
        movingRegion.SetSize( movingSize );

        MovingImageIndexType movingIndex = movingRegion.GetIndex();
        movingIndex[0] = 0;
        movingIndex[1] = 0;
        movingIndex[2] = 0;
        movingIndex[3] = m_BaseImage;
        movingRegion.SetIndex( movingIndex );
        std::cout << "Region: " << movingRegion << std::endl;

        extractImageFilter->SetExtractionRegion( movingRegion );
        extractImageFilter->SetInput( m_MovingImage );
        extractImageFilter->Update();



        /*** Set up the Registration ***/
        metric->SetNumberOfSpatialSamples( m_NumberOfSpatialSamples );
        registration->SetMetric(        metric        );
        registration->SetOptimizer(     optimizer     );
        registration->SetInterpolator(  interpolator  );

        TransformType::Pointer  transform = TransformType::New();
        registration->SetTransform( transform );



        itk::Point<double, 3> zeroOrigin;
        zeroOrigin.GetVnlVector().fill(0.0);
        extractImageFilter->GetOutput()->SetOrigin(zeroOrigin);

        registration->SetFixedImage(   m_FixedImage   );
        registration->SetMovingImage(   extractImageFilter->GetOutput()   );
        registration->SetFixedImageRegion( m_FixedImage->GetBufferedRegion() );


        TransformInitializerTypePointer initializer = TransformInitializerType::New();
        initializer->SetTransform(   transform );
        initializer->SetFixedImage(  m_FixedImage );
        initializer->SetMovingImage( extractImageFilter->GetOutput() );
        initializer->MomentsOn();
        initializer->InitializeTransform();

        //std::cout << "Fixed Image: " << m_FixedImage << std::endl;
        std::cout << "Initializer, center: " << transform->GetCenter()
            << ", offset: " << transform->GetOffset()
            << "." << std::endl;

        VersorType     rotation;
        VectorType     axis;

        axis[0] = 0.0;
        axis[1] = 0.0;
        axis[2] = 1.0;

        const double angle = 0;

        rotation.Set(  axis, angle  );
        transform->SetRotation( rotation );
        registration->SetInitialTransformParameters( transform->GetParameters() );

        const double translationScale = 1.0 / m_TranslationScale;

        OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );

        optimizerScales[0] = 1.0;
        optimizerScales[1] = 1.0;
        optimizerScales[2] = 1.0;
        optimizerScales[3] = translationScale;
        optimizerScales[4] = translationScale;
        optimizerScales[5] = translationScale;
        optimizer->SetScales( optimizerScales );

        optimizer->SetMaximumStepLength( m_MaximumStepLength );
        optimizer->SetMinimumStepLength( m_MinimumStepLength );

        optimizer->SetRelaxationFactor( m_RelaxationFactor );

        optimizer->SetNumberOfIterations( m_NumberOfIterations );

        std::cout << "Before Rigid Registration, center: " << transform->GetCenter()
            << ", offset: " << transform->GetOffset()
            << "." << std::endl;


        try
            {
            registration->StartRegistration();
            }
        catch( itk::ExceptionObject & err )
            {
            std::cerr << "ExceptionObject caught !" << std::endl;
            std::cerr << err << std::endl;
            }

        OptimizerParameterType finalParameters = registration->GetLastTransformParameters();


        const double versorX              = finalParameters[0];
        const double versorY              = finalParameters[1];
        const double versorZ              = finalParameters[2];
        const double finalTranslationX    = finalParameters[3];
        const double finalTranslationY    = finalParameters[4];
        const double finalTranslationZ    = finalParameters[5];
        const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
        const double bestValue = optimizer->GetValue();

        // Print out results
        //
        std::cout << std::endl << std::endl;
        std::cout << "Result = " << std::endl;
        std::cout << " versor X      = " << versorX  << std::endl;
        std::cout << " versor Y      = " << versorY  << std::endl;
        std::cout << " versor Z      = " << versorZ  << std::endl;
        std::cout << " Translation X = " << finalTranslationX  << std::endl;
        std::cout << " Translation Y = " << finalTranslationY  << std::endl;
        std::cout << " Translation Z = " << finalTranslationZ  << std::endl;
        std::cout << " Iterations    = " << numberOfIterations << std::endl;
        std::cout << " Metric value  = " << bestValue          << std::endl;


        transform->SetParameters( finalParameters );

        std::cout << "After Rigid Registration, center: " << transform->GetCenter()
            << ", offset: " << transform->GetOffset()
            << "." << std::endl;


        TransformType::MatrixType matrix = transform->GetRotationMatrix();
        TransformType::OffsetType offset = transform->GetOffset();

        std::cout << "Matrix = " << std::endl << matrix << std::endl;
        std::cout << "Offset = " << std::endl << offset << std::endl;

        m_Output = TransformType::New();
        m_Output->SetCenter( transform->GetCenter() );
        m_Output->SetParameters( transform->GetParameters() );

        }



}// end namespace itk

