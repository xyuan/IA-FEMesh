#ifndef _ApplyField_txx
#define _ApplyFIeld_txx
#include "ApplyField.h"
#include "itkImage.h"
#include "itkWarpImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkReinitializeLevelSetImageFilter.h"
#include "itkIO.h"

namespace itk
{
  template <typename TDeformationField, typename TInputImage, typename TOutputImage> ApplyField <TDeformationField, TInputImage, TOutputImage>::ApplyField():
    m_InputImage(0),
  m_OutputImage(0),
  m_DeformationField(0),
  m_InputImageFilename(""),
  m_DefaultPixelValue(0)
    {
    }

  template <typename TDeformationField, typename TInputImage, typename TOutputImage> ApplyField <TDeformationField, TInputImage, TOutputImage>::~ApplyField()
    {
    }

  template <typename TDeformationField, typename TInputImage, typename TOutputImage> void ApplyField <TDeformationField, TInputImage, TOutputImage>::Execute()
    {
    //Use the filename provided if it is given with the SetInputImageFilename member function, and overwrite current internal m_InputImage.
    if ( m_InputImageFilename != "" )
      {
      m_InputImage= itkUtil::ReadImageCoronal<InputImageType>(m_InputImageFilename);
      }
    // else assume that the InputImage was provided with the SetInputImage member function.
    if ( m_InputImage.IsNull() )
      {
      std::cout << "ERROR:  No Input image give.! " << std::endl;
      }

    //Need to resample the input image to be the same size as the deformation feild.
      typename TInputImage::Pointer ResampledImage= itkUtil::ResampleImage<TInputImage,TInputImage>(m_InputImage,
      m_DeformationField->GetLargestPossibleRegion().GetSize(),
      m_DeformationField->GetSpacing(),
      m_DeformationField->GetOrigin(),
      m_DeformationField->GetDirection(),
      this->m_DefaultPixelValue
    );

    typedef WarpImageFilter<InputImageType,OutputImageType,DeformationFieldType> WarperType;
    typename WarperType::Pointer warper = WarperType::New();
    warper->SetInput(ResampledImage);
    warper->SetDeformationField(m_DeformationField);
    warper->SetOutputOrigin(m_DeformationField->GetOrigin());
    warper->SetOutputSpacing(m_DeformationField->GetSpacing());
    warper->SetEdgePaddingValue(m_DefaultPixelValue);
    warper->Update();
    std::cout << "  Registration Applied" << std::endl;
    m_OutputImage = warper->GetOutput();
    m_OutputImage->SetDirection(m_DeformationField->GetDirection());
    }

  template <typename TDeformationField, typename TInputImage, typename TOutputImage> void ApplyField <TDeformationField, TInputImage, TOutputImage>::ReleaseDataFlagOn()
    {
    m_InputImage->DisconnectPipeline();
    m_InputImageFilename = "";
    m_DeformationField->DisconnectPipeline();
    }
}
#endif
