/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkAnatomicalBSplineFilter.h,v $
Language:  C++
Date:      $Date: 2007/06/17 23:44:27 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAnatomicalBSplineFilter_h
#define __itkAnatomicalBSplineFilter_h

#include "itkObject.h"
#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkIOCommon.h"
#include <itkExtractImageFilter.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkBSplineDeformableTransform.h>
#include <itkLBFGSBOptimizer.h>
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

    class AnatomicalBSplineFilter : public itk::Object
    {
public:
    /** Standard class typedefs. */
    typedef AnatomicalBSplineFilter   Self;
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
    typedef itk::Image<signed short,4>      MovingImageType;
    typedef MovingImageType::Pointer     MovingImagePointer;
    typedef MovingImageType::ConstPointer    MovingImageConstPointer;
    typedef MovingImageType::RegionType     MovingImageRegionType;
    typedef MovingImageType::SizeType     MovingImageSizeType;
    typedef MovingImageType::SpacingType     MovingImageSpacingType;
    typedef MovingImageType::PointType     MovingImagePointType;
    typedef MovingImageType::PixelType     MovingImagePixelType;
    typedef MovingImageType::DirectionType   MovingImageDirectionType;
    typedef MovingImageType::IndexType     MovingImageIndexType;


    /** Internal typedefs. */
    typedef itk::Image<MovingImagePixelType, 3>    ExtractImageType;
    typedef ExtractImageType::Pointer    ExtractImagePointer;
    typedef itk::ExtractImageFilter< MovingImageType, ExtractImageType >
        ExtractFilterType;
    typedef ExtractFilterType::Pointer ExtractFilterTypePointer;
    typedef ExtractImageType::IndexType      ExtractImageIndexType;

    static const unsigned int SpaceDimension = 3;
    static const unsigned int SplineOrder = 3;
    typedef double CoordinateRepType;
    typedef itk::BSplineDeformableTransform<
        CoordinateRepType,
        SpaceDimension,
        SplineOrder >               TransformType;
    typedef TransformType::RegionType               TransformRegionType;
    typedef TransformRegionType::SizeType              TransformSizeType;
    typedef TransformType::SpacingType               TransformSpacingType;
    typedef TransformType::OriginType               TransformOriginType;
    typedef TransformType::ParametersType            TransformParametersType;

    typedef itk::LBFGSBOptimizer                     OptimizerType;


    typedef itk::MattesMutualInformationImageToImageMetric<
        FixedImageType,
        ExtractImageType >          MetricType;

    typedef itk:: LinearInterpolateImageFunction<
        ExtractImageType,
        double          >        InterpolatorType;

    typedef itk::ImageRegistrationMethod<
        FixedImageType,
        ExtractImageType >          RegistrationType;


    typedef TransformType::Pointer             TransformTypePointer;
    typedef MetricType::Pointer                  MetricTypePointer;
    typedef OptimizerType::Pointer               OptimizerTypePointer;
    typedef OptimizerType::ParametersType         OptimizerParameterType;
    typedef OptimizerType::ScalesType             OptimizerScalesType;
    typedef OptimizerType::BoundSelectionType        OptimizerBoundSelectionType;
    typedef OptimizerType::BoundValueType          OptimizerBoundValueType;

    typedef InterpolatorType::Pointer            InterpolatorTypePointer;
    typedef RegistrationType::Pointer            RegistrationTypePointer;





    /** Typedef of the bulk transform. */
    /*
    typedef itk::VersorRigid3DTransform< double >     BulkTransformType;
    typedef BulkTransformType::Pointer           BulkTransformPointer;
     */
    typedef Transform<CoordinateRepType,itkGetStaticConstMacro(SpaceDimension),
            itkGetStaticConstMacro(SpaceDimension)> BulkTransformType;
    typedef BulkTransformType::ConstPointer  BulkTransformPointer;
#if 0
    /*
    void SetBulkTransform( BulkTransformPointer &BulkTransform )
    {
    m_Output->SetBulkTransform( BulkTransform );
    }
     */

    /** ImageDimension constants * /
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
    itkTypeMacro(AnatomicalBSplineFilter, itk::Object);

    /* SetInput and GetOutput Macros */
    itkSetObjectMacro (FixedImage, FixedImageType);
    itkSetObjectMacro (MovingImage, MovingImageType);
    itkSetConstObjectMacro (BulkTransform, BulkTransformType);
    itkGetObjectMacro (Output, TransformType);



    itkSetMacro (SpatialSampleScale, int);
    itkSetMacro (MaximumNumberOfIterations, int);
    itkSetMacro (MaximumNumberOfEvaluations, int);
    itkSetMacro (MaximumNumberOfCorrections, int);
    itkSetMacro (BSplineHistogramBins, int);
    itkGetMacro (SpatialSampleScale, int);
    itkGetMacro (MaximumNumberOfIterations, int);
    itkGetMacro (MaximumNumberOfEvaluations, int);
    itkGetMacro (MaximumNumberOfCorrections, int);
    itkGetMacro (BSplineHistogramBins, int);
    itkSetMacro (GridXSize, int);
    itkSetMacro (GridYSize, int);
    itkSetMacro (GridZSize, int);
    itkSetMacro (GridBorderXSize, int);
    itkSetMacro (GridBorderYSize, int);
    itkSetMacro (GridBorderZSize, int);
    itkGetMacro (GridXSize, int);
    itkGetMacro (GridYSize, int);
    itkGetMacro (GridZSize, int);
    itkGetMacro (GridBorderXSize, int);
    itkGetMacro (GridBorderYSize, int);
    itkGetMacro (GridBorderZSize, int);
    itkSetMacro (BaseImage, int);
    itkGetMacro (BaseImage, int);
    itkSetMacro (CostFunctionConvergenceFactor, float);
    itkGetMacro (CostFunctionConvergenceFactor, float);
    itkSetMacro (ProjectedGradientTolerance, float);
    itkGetMacro (ProjectedGradientTolerance, float);
    itkSetMacro (BoundTypeX, int);
    itkGetMacro (BoundTypeX, int);
    itkSetMacro (BoundTypeY, int);
    itkGetMacro (BoundTypeY, int);
    itkSetMacro (BoundTypeZ, int);
    itkGetMacro (BoundTypeZ, int);
    itkSetMacro (LowerBoundX, float);
    itkGetMacro (LowerBoundX, float);
    itkSetMacro (LowerBoundY, float);
    itkGetMacro (LowerBoundY, float);
    itkSetMacro (LowerBoundZ, float);
    itkGetMacro (LowerBoundZ, float);
    itkSetMacro (UpperBoundX, float);
    itkGetMacro (UpperBoundX, float);
    itkSetMacro (UpperBoundY, float);
    itkGetMacro (UpperBoundY, float);
    itkSetMacro (UpperBoundZ, float);
    itkGetMacro (UpperBoundZ, float);

    void Update();

protected:
    AnatomicalBSplineFilter();
    ~AnatomicalBSplineFilter() {};


private:
    AnatomicalBSplineFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    /*** Input and Output Objects ***/
    FixedImagePointer            m_FixedImage;
    MovingImagePointer            m_MovingImage;
    BulkTransformPointer          m_BulkTransform;
    TransformTypePointer             m_Output;


    // Optional Flip - Used to fix problems with the Direction Cosines
    int   m_SpatialSampleScale;
    int   m_MaximumNumberOfIterations;
    int   m_MaximumNumberOfEvaluations;
    int   m_MaximumNumberOfCorrections;
    int   m_BSplineHistogramBins;
    int   m_BaseImage;
    int   m_GridXSize;
    int   m_GridYSize;
    int   m_GridZSize;
    int   m_GridBorderXSize;
    int   m_GridBorderYSize;
    int   m_GridBorderZSize;
    float m_CostFunctionConvergenceFactor;
    float m_ProjectedGradientTolerance;
    int   m_BoundTypeX;
    int   m_BoundTypeY;
    int   m_BoundTypeZ;
    float m_LowerBoundX;
    float m_LowerBoundY;
    float m_LowerBoundZ;
    float m_UpperBoundX;
    float m_UpperBoundY;
    float m_UpperBoundZ;




    } ; // end of class

} // end namespace itk


#endif


