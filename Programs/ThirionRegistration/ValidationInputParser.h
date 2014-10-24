
#ifndef _ValidationInputParser_h
#define _ValidationInputParser_h

#include "itkObjectFactory.h"
#include "itkObject.h"
#include "itkFixedArray.h"
#include "itkArray.h"

namespace itk
{
    /** \class ValidationInputParser
     *
     * This component parse an input parameter file for a simple 
     * atlas based segmentation application.
     *
     * This class is activated by method Execute().
     *
     * Inputs:
     *  - altas image name
     *  - subject image name
     *  - the  parameter filename
     *
     *
     * Outputs:
     *  - pointer to the subject (fixed) image
     *  - pointer to the atlas (moving) image
     *  - the number of histogram levels to use
     *  - the number of histogram match points to use
     *  - the number of levels
     *  - the number of iterations at each level
     *  - the fixed image starting shrink factors
     *  - the moving image starting shrink factors
     *
     */

    template <typename TImage>
        class ITK_EXPORT ValidationInputParser : public Object
            {
        public:

            /** Standard class typedefs. */
            typedef ValidationInputParser Self;
            typedef Object Superclass;
            typedef SmartPointer<Self> Pointer;
            typedef SmartPointer<const Self>  ConstPointer;

            /** Run-time type information (and related methods). */
            itkTypeMacro(ValidationInputParser, Object);

            /** Method for creation through the object factory. */
            itkNewMacro(Self);

            /** Image Type. */
            typedef TImage ImageType;
            typedef typename ImageType::Pointer ImagePointer;

            /** Image dimension enumeration. */
            itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);

            /** ShrinkFactors type. */
            typedef FixedArray<unsigned int, itkGetStaticConstMacro(ImageDimension)> ShrinkFactorsType;

            /** IterationArray type. */
            typedef Array<unsigned int> IterationsArrayType;

            /** Set the atlas patient. */
            itkSetStringMacro( TheMovingImageFileName );

            /** Set the subject patient. */
            itkSetStringMacro( TheFixedImageFileName );

            /** Set input parameter file name. */
            itkSetStringMacro( ParameterFileName );

            /** Parse the input file. */
            void Execute();

            /** Get pointer to the atlas image. */
            itkGetObjectMacro( TheMovingImage, ImageType );

            /** Get pointer to the subject image. */
            itkGetObjectMacro( TheFixedImage, ImageType );

            /** Get the number of histogram bins. */
            itkGetConstMacro( NumberOfHistogramLevels, unsigned long );

            /** Get the number of match points. */
            itkGetConstMacro( NumberOfMatchPoints, unsigned long );

            /** Get the number of levels. */
            itkGetMacro( NumberOfLevels, unsigned short );

            /** Get the atlas image starting shrink factors. */
            itkGetConstReferenceMacro( TheMovingImageShrinkFactors, ShrinkFactorsType );

            /** Get the subject image starting shrink factors. */
            itkGetConstReferenceMacro( TheFixedImageShrinkFactors, ShrinkFactorsType );

            /** Get the number of iterations at each level. */
            itkGetConstReferenceMacro( NumberOfIterations, IterationsArrayType );

        protected:
            ValidationInputParser();
            ~ValidationInputParser(){};


        private:
            ValidationInputParser( const Self& ); //purposely not implemented
            void operator=( const Self& ); //purposely not implemented

            std::string                   m_TheMovingImageFileName;
            std::string                   m_TheFixedImageFileName;
            std::string                   m_ParameterFileName;

            typename ImageType::Pointer   m_TheMovingImage;
            typename ImageType::Pointer   m_TheFixedImage;


            unsigned long                 m_NumberOfHistogramLevels;
            unsigned long                 m_NumberOfMatchPoints;
            unsigned short                m_NumberOfLevels;
            ShrinkFactorsType             m_TheMovingImageShrinkFactors;
            ShrinkFactorsType             m_TheFixedImageShrinkFactors;
            IterationsArrayType           m_NumberOfIterations;
            };
} // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "ValidationInputParser.txx"
#endif

#endif
