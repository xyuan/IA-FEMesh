/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkAnatomicalBSplineFilter.cxx,v $
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
#include "itkAnatomicalBSplineFilter.h"
#include <itkIOCommon.h>
#include <itkCastImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkExtractImageFilter.h>
#include "itkMetaDataObject.h"
#include "itkProgressAccumulator.h"

#include <iostream>


namespace itk
{

    AnatomicalBSplineFilter::AnatomicalBSplineFilter()
        {

        m_SpatialSampleScale = 100;
        m_MaximumNumberOfIterations = 500;
        m_MaximumNumberOfEvaluations = 500;
        m_MaximumNumberOfCorrections = 12;
        m_BSplineHistogramBins = 50;
        m_BaseImage = 0;
        m_GridXSize = 12;
        m_GridYSize = 12;
        m_GridZSize = 12;
        m_GridBorderXSize = 3;
        m_GridBorderYSize = 3;
        m_GridBorderZSize = 3;
        m_CostFunctionConvergenceFactor = 1e+7;
        m_ProjectedGradientTolerance = 1e-4;
        m_BoundTypeX = 0;
        m_BoundTypeY = 0;
        m_BoundTypeZ = 0;
        m_LowerBoundX = 0.0;
        m_LowerBoundY = 0.0;
        m_LowerBoundZ = 0.0;
        m_UpperBoundX = 0.0;
        m_UpperBoundY = 0.0;
        m_UpperBoundZ = 0.0;

        m_BulkTransform = NULL;
        m_Output = TransformType::New();

        }



    void AnatomicalBSplineFilter::Update ( )
        {

        std::cout << "AnatomicalBSplineFilter()...." << std::endl;


        MetricTypePointer         metric        = MetricType::New();
        OptimizerTypePointer      optimizer     = OptimizerType::New();
        InterpolatorTypePointer   interpolator  = InterpolatorType::New();
        RegistrationTypePointer   registration  = RegistrationType::New();

        ExtractFilterTypePointer extractImageFilter     = ExtractFilterType::New();

        MovingImageRegionType  movingRegion  = m_MovingImage->GetLargestPossibleRegion();
        MovingImageSizeType    movingSize    = movingRegion.GetSize();
        MovingImageSpacingType movingSpacing = m_MovingImage->GetSpacing();
        MovingImagePointType   movingOrigin  = m_MovingImage->GetOrigin();


        std::cout << "Region: " << movingRegion << std::endl;
        std::cout << "Spacing: " << movingSpacing << std::endl;
        std::cout << "Origin: " << movingOrigin << std::endl;

        movingSize[3] = 0;
        movingRegion.SetSize(movingSize);

        MovingImageIndexType movingIndex = movingRegion.GetIndex();
        movingIndex[0] = 0;
        movingIndex[1] = 0;
        movingIndex[2] = 0;
        movingIndex[3] = 0;
        movingRegion.SetIndex( movingIndex );
        std::cout << "Region: " << movingRegion << std::endl;

        extractImageFilter->SetExtractionRegion( movingRegion );
        extractImageFilter->SetInput( m_MovingImage );
        extractImageFilter->Update();


        /*** Set up the Registration ***/
        registration->SetMetric(        metric        );
        registration->SetOptimizer(     optimizer     );
        registration->SetInterpolator(  interpolator  );
        registration->SetTransform( m_Output );

        /*** Setup the Registration ***/
        registration->SetFixedImage(  m_FixedImage   );
        registration->SetMovingImage(   extractImageFilter->GetOutput()   );


        FixedImageRegionType fixedImageRegion = m_FixedImage->GetBufferedRegion();

        registration->SetFixedImageRegion( fixedImageRegion );

        /*** Setup the B-SPline Parameters ***/
        TransformRegionType    bsplineRegion;
        TransformSizeType      gridSizeOnImage;
        TransformSizeType      gridBorderSize;
        TransformSizeType      totalGridSize;

        gridSizeOnImage[0] = m_GridXSize;
        gridSizeOnImage[1] = m_GridYSize;
        gridSizeOnImage[2] = m_GridZSize;
        gridBorderSize[0]  = m_GridBorderXSize;    // Border for spline order = 3 ( 1 lower, 2 upper )
        gridBorderSize[1]  = m_GridBorderYSize; 
        gridBorderSize[2]  = m_GridBorderZSize; 
        
        totalGridSize = gridSizeOnImage + gridBorderSize;
        bsplineRegion.SetSize( totalGridSize );

        TransformSpacingType spacing = m_FixedImage->GetSpacing();
        TransformOriginType origin = m_FixedImage->GetOrigin();

        FixedImageSizeType fixedImageSize = fixedImageRegion.GetSize();

        for(unsigned int r=0; r<3; r++)
            {
            spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  /
                static_cast<double>(gridSizeOnImage[r] - 1) );
            origin[r]  -=  spacing[r];
            }

        m_Output->SetGridSpacing( spacing );
        m_Output->SetGridOrigin( origin );
        m_Output->SetGridRegion( bsplineRegion );

        if ( m_BulkTransform.IsNotNull() )
            {
            std::cout << "Using Bulk Transform" << std::endl;
            m_Output->SetBulkTransform(   m_BulkTransform   );
            }


        const unsigned int numberOfParameters =
            m_Output->GetNumberOfParameters();

        TransformParametersType parameters( numberOfParameters );

        parameters.Fill( 0.0 );

        m_Output->SetParameters( parameters );

        registration->SetInitialTransformParameters( m_Output->GetParameters() );


        OptimizerBoundSelectionType boundSelect( m_Output->GetNumberOfParameters() );
        OptimizerBoundValueType     upperBound( m_Output->GetNumberOfParameters() );
        OptimizerBoundValueType     lowerBound( m_Output->GetNumberOfParameters() );

        
        /* Old Method - Unbounded Deformations in X,Y,Z */ 
        // boundSelect.Fill( 0 );
        // upperBound.Fill( 0.0 );
        // lowerBound.Fill( 0.0 );
        
        /* New Method - User Specifies the Deformation Bounds in X,Y,Z */
        /*		Default is the same as the Old method          */
        for ( int i=0; i<boundSelect.size(); i+=3)
           {
           boundSelect[i+0] = m_BoundTypeX;
           boundSelect[i+1] = m_BoundTypeY;
           boundSelect[i+2] = m_BoundTypeZ;
           lowerBound[i+0]  = m_LowerBoundX;
           lowerBound[i+1]  = m_LowerBoundY;
           lowerBound[i+2]  = m_LowerBoundZ;
           upperBound[i+0]  = m_UpperBoundX;
           upperBound[i+1]  = m_UpperBoundY;
           upperBound[i+2]  = m_UpperBoundZ;
           }


        optimizer->SetBoundSelection( boundSelect );
        optimizer->SetUpperBound( upperBound );
        optimizer->SetLowerBound( lowerBound );

        optimizer->SetCostFunctionConvergenceFactor( m_CostFunctionConvergenceFactor );
        optimizer->SetProjectedGradientTolerance( m_ProjectedGradientTolerance );
        optimizer->SetMaximumNumberOfIterations( m_MaximumNumberOfIterations );
        optimizer->SetMaximumNumberOfEvaluations( m_MaximumNumberOfEvaluations );
        optimizer->SetMaximumNumberOfCorrections( m_MaximumNumberOfCorrections );

        metric->SetNumberOfHistogramBins( m_BSplineHistogramBins );

        /*** Make this a Parameter ***/
        const unsigned int numberOfSamples = fixedImageRegion.GetNumberOfPixels() / m_SpatialSampleScale;

        metric->SetNumberOfSpatialSamples( numberOfSamples );
        metric->ReinitializeSeed( 76926294 );


        // Add a time probe
        itk::TimeProbesCollectorBase collector;

        std::cout << std::endl << "Starting Registration" << std::endl;

        try
            {
            collector.Start( "Registration" );
            registration->StartRegistration();
            collector.Stop( "Registration" );
            }
        catch( itk::ExceptionObject & err )
            {
            std::cerr << "ExceptionObject caught !" << std::endl;
            std::cerr << err << std::endl;
            return;
            }

        OptimizerType::ParametersType finalParameters =
            registration->GetLastTransformParameters();

        collector.Report();

        /* This call is required to copy the parameters */
        m_Output->SetParametersByValue( finalParameters );

        std::cout << m_Output << std::endl;

        }



}// end namespace itk

