/**
 * \defgroup AF Apply Field
 * \ingroup Reg
 */
#ifndef _ApplyField_h
#define _ApplyField_h

#include "itkObjectFactory.h"
#include "itkObject.h"

namespace itk
{
    template < typename DeformationField, typename TInputImage, typename TOutputImage>
        class ApplyField : public Object
            {
        public:
            typedef ApplyField Self;
            typedef Object Superclass;
            typedef SmartPointer<Self> Pointer;
            typedef SmartPointer<const Self>  ConstPointer;

            itkTypeMacro(MIMApplication, Object);

            itkNewMacro(Self);

            typedef TInputImage InputImageType;
            typedef TOutputImage OutputImageType;
            typedef typename OutputImageType::PixelType PixelType;
            typedef typename InputImageType::Pointer ImagePointer;

            itkStaticConstMacro(ImageDimension, unsigned int, TInputImage::ImageDimension);
            typedef DeformationField DeformationFieldType;

            itkSetObjectMacro(InputImage, InputImageType);
            itkGetObjectMacro(InputImage, InputImageType);
            itkGetObjectMacro(OutputImage, OutputImageType);
            itkSetStringMacro(InputImageFilename);

            /** Set/Get value to replace thresholded pixels. Pixels that lie *
             *  within Lower and Upper (inclusive) will be replaced with this
             *  value. The default is 1. */
            itkSetMacro(DefaultPixelValue,  PixelType);
            itkGetMacro(DefaultPixelValue,  PixelType);

            itkSetObjectMacro(DeformationField, DeformationFieldType);

            void Execute();
            void ReleaseDataFlagOn();
        protected:
            ApplyField();
            virtual ~ApplyField();
#if 0
            //Use read image from Utilities above
            void ReadImageCoronal( const std::string FileName, ImagePointer &image);
#endif
        private:
            typename InputImageType::Pointer        m_InputImage;
            typename OutputImageType::Pointer       m_OutputImage;
            typename DeformationFieldType::Pointer  m_DeformationField;
            std::string                             m_InputImageFilename;
            PixelType                               m_DefaultPixelValue;
            };
}
#ifndef ITK_MANUAL_INSTANTIATION
#include "ApplyField.txx"
#endif
#endif
