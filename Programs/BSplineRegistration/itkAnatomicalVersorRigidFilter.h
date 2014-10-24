/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkAnatomicalVersorRigidFilter.h,v $
Language:  C++
Date:      $Date: 2007/06/17 23:44:27 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnatomicalVersorRigidFilter_h
#define __itkAnatomicalVersorRigidFilter_h

#include "itkObject.h"
#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkIOCommon.h"
#include <itkExtractImageFilter.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include <itkVersorRigid3DTransformOptimizer.h>
#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkVersorRigid3DTransform.h>
#include <itkCenteredTransformInitializer.h>
#include <itkTimeProbesCollectorBase.h>
#include <itkTransformFactory.h>

#include <map>
#include <string>

namespace itk {

    /** \class Orient4dDiffusionImageFilter
     * \brief Permute axes and then flip images as needed to obtain
     *  agreement in coordinateOrientation codes.
     *
     * This class satisfies performs the following steps:
     *    For i in 4th Dimension
     *      ExtractVolume with Extract Image Filter
     *      Orient 3D extracted volume
     *    End
     *
     * It is build upon the ExtractImageFilter and the OrientImageFilter
     */

    class AnatomicalVersorRigidFilter : public itk::Object
    {
public:
    /** Standard class typedefs. */
    typedef AnatomicalVersorRigidFilter Self;
    typedef itk::Object        Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Fixed Image typedefs. */
    typedef itk::Image<signed short,3>      FixedImageType;
    typedef FixedImageType::Pointer    FixedImagePointer;
    typedef FixedImageType::ConstPointer    FixedImageConstPointer;
    typedef FixedImageType::RegionType    FixedImageRegionType;
    typedef FixedImageType::SizeType    FixedImageSizeType;
    typedef FixedImageType::SpacingType    FixedImageSpacingType;
    typedef FixedImageType::PointType    FixedImagePointType;
    typedef FixedImageType::PixelType    FixedImagePixelType;
    typedef FixedImageType::DirectionType   FixedImageDirectionType;
    typedef FixedImageType::IndexType    FixedImageIndexType;

    /** Moving Image typedefs. */
    typedef itk::Image<signed short,4>       MovingImageType;
    typedef MovingImageType::Pointer     MovingImagePointer;
    typedef MovingImageType::ConstPointer    MovingImageConstPointer;
    typedef MovingImageType::RegionType     MovingImageRegionType;
    typedef MovingImageType::SizeType     MovingImageSizeType;
    typedef MovingImageType::SpacingType     MovingImageSpacingType;
    typedef MovingImageType::PointType     MovingImagePointType;
    typedef MovingImageType::PixelType     MovingImagePixelType;
    typedef MovingImageType::DirectionType   MovingImageDirectionType;
    typedef MovingImageType::IndexType     MovingImageIndexType;

    /*** Internal typedefs ***/
    typedef itk::Image<MovingImagePixelType, 3>        ExtractImageType;
    typedef ExtractImageType::Pointer        ExtractImagePointer;
    typedef itk::ExtractImageFilter< MovingImageType, ExtractImageType >
        ExtractFilterType;
    typedef ExtractFilterType::Pointer ExtractFilterTypePointer;
    typedef ExtractImageType::IndexType          ExtractImageIndexType;


    /** Output Transform typedefs. */
    typedef itk::VersorRigid3DTransform< double >     TransformType;
    typedef itk::VersorRigid3DTransformOptimizer      OptimizerType;
    typedef itk::MattesMutualInformationImageToImageMetric<
        FixedImageType,
        ExtractImageType >        MetricType;

    typedef itk:: LinearInterpolateImageFunction<
        ExtractImageType,
        double          >         InterpolatorType;

    typedef itk::ImageRegistrationMethod<
        FixedImageType,
        ExtractImageType >        RegistrationType;

    typedef itk::CenteredTransformInitializer< TransformType,
            FixedImageType,
            ExtractImageType
                >  TransformInitializerType;
    typedef TransformType::Pointer  TransformTypePointer;
    typedef TransformType::VersorType  VersorType;
    typedef VersorType::VectorType     VectorType;
    typedef MetricType::Pointer        MetricTypePointer;
    typedef OptimizerType::Pointer      OptimizerTypePointer;
    typedef OptimizerType::ParametersType OptimizerParameterType;
    typedef OptimizerType::ScalesType  OptimizerScalesType;
    typedef InterpolatorType::Pointer   InterpolatorTypePointer;
    typedef RegistrationType::Pointer   RegistrationTypePointer;
    typedef TransformInitializerType::Pointer TransformInitializerTypePointer;

#if 0
    / ** ImageDimension constants * /
      itkStaticConstMacro(InputImageDimension, unsigned int,
      TInputImage::ImageDimension);
      itkStaticConstMacro(OutputImageDimension, unsigned int,
      TOutputImage::ImageDimension);

    / ** The dimensions of the input image must equal those of the
    output image. * /
    itkConceptMacro(SameDimension,
    (Concept::SameDimension<itkGetStaticConstMacro(InputImageDimension),itkGetStaticConstMacro(OutputImageDimension)>));

    / ** The dimension of the input image must be 4. * /
    itkConceptMacro(DimensionShouldBe4,
    (Concept::SameDimension<itkGetStaticConstMacro(InputImageDimension),4>));
     */
#endif
    /** Standard New method. */
    itkNewMacro(Self);

    /** Runtime information support. */
    itkTypeMacro(AnatomicalVersorRigidFilter, itk::Object);

    /* SetInput and GetOutput Macros */
    itkSetObjectMacro (FixedImage,  FixedImageType);
    itkSetObjectMacro (MovingImage, MovingImageType);
    itkGetObjectMacro (Output,      TransformType);

    itkSetMacro (NumberOfSpatialSamples, int);
    itkSetMacro (NumberOfIterations,     int);
    itkSetMacro (TranslationScale,       float);
    itkSetMacro (MaximumStepLength,      float);
    itkSetMacro (MinimumStepLength,      float);
    itkSetMacro (RelaxationFactor,       float);
    itkSetMacro (BaseImage,              int);


    void Update();

protected:
    AnatomicalVersorRigidFilter();
    ~AnatomicalVersorRigidFilter() {};


private:
    AnatomicalVersorRigidFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    // Input and Output Image
    FixedImagePointer            m_FixedImage;
    MovingImagePointer            m_MovingImage;
    TransformTypePointer           m_Output;


    // Optional Flip - Used to fix problems with the Direction Cosines
    float m_TranslationScale;
    float m_MaximumStepLength;
    float m_MinimumStepLength;
    float m_RelaxationFactor;
    int   m_NumberOfSpatialSamples;
    int   m_NumberOfIterations;
    int   m_BaseImage;


    } ; // end of class

} // end namespace itk


#endif


