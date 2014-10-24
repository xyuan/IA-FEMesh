#ifndef _DemonsPreprocessor_txx
#define _DemonsPreprocessor_txx
#include "DemonsPreprocessor.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkBOBFFilter.h"
#include "itkAffineTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkIO.h"
#include "itkMedianImageFilter.h"


namespace itk
{

  template <typename TInputImage, typename TOutputImage>
  DemonsPreprocessor<TInputImage,TOutputImage>
  ::DemonsPreprocessor()
  {
    m_NumberOfHistogramLevels = 256;
    m_NumberOfMatchPoints = 1;

    m_FixedImageMinimum = NumericTraits<InputPixelType>::NonpositiveMin();
    m_MovingImageMinimum = NumericTraits<InputPixelType>::NonpositiveMin();

    m_BOBFTargetMask = "none";
    m_BOBFTemplateMask = "none";

    //    m_Seed =  NumericTraits<IndexType>::Zero;
    for(unsigned i = 0; i < TInputImage::ImageDimension; i++)
      {
      m_Seed[i] = 0;
      }
    m_Lower = NumericTraits<PixelType>::NonpositiveMin();
    m_Upper = NumericTraits<PixelType>::max();

    m_DefaultPixelValue = NumericTraits<PixelType>::One;
    m_Radius.Fill(1);
    m_OutDebug = "OFF";
    m_MedianFilterRadius = 0;
  }



  template <typename TInputImage, typename TOutputImage>
  void
  DemonsPreprocessor<TInputImage,TOutputImage>
  ::Execute()
  {
    if(m_MedianFilterRadius != 0)
      {
      typedef typename itk::MedianImageFilter<TInputImage,TInputImage> MedianImageFilterType;
      typename MedianImageFilterType::Pointer medianFilter =
        MedianImageFilterType::New();
      typename TInputImage::SizeType radius;
      radius[0] = radius[1] = radius[2] = m_MedianFilterRadius;
      medianFilter->SetRadius(radius);
      medianFilter->SetInput(m_InputFixedImage);
      medianFilter->Update();
      m_InputFixedImage = medianFilter->GetOutput();
      //
      // reinitialize
      medianFilter = MedianImageFilterType::New();
      medianFilter->SetRadius(radius);
      medianFilter->SetInput(m_InputMovingImage);
      medianFilter->Update();
      m_InputMovingImage = medianFilter->GetOutput();
      }
    typedef itk::CastImageFilter< TInputImage , TOutputImage> CastToOutputFilterType;
    {
    //Get The UnNormalized Fixed Image.
    typename CastToOutputFilterType::Pointer CastInput  = CastToOutputFilterType::New();
    CastInput->SetInput(m_InputFixedImage);
    CastInput->Update();

    m_UnNormalizedFixedImage=CastInput->GetOutput();
    }

    /*TODO: If a reference histogram is input then Output fixed image will be normalized to the histogram otherwise it will be same as the unNormalized fixed image.*/

    {
    //Get The UnNormalized Moving Image.
    typename CastToOutputFilterType::Pointer CastOutput = CastToOutputFilterType::New();
    CastOutput->SetInput(m_InputMovingImage);
    CastOutput->Update();
    typename TOutputImage::Pointer OutputCastImage=CastOutput->GetOutput();

    if( (m_UnNormalizedFixedImage->GetLargestPossibleRegion().GetSize()!=OutputCastImage->GetLargestPossibleRegion().GetSize())
        || (m_UnNormalizedFixedImage->GetSpacing()!=OutputCastImage->GetSpacing()))
      {
    m_UnNormalizedMovingImage= itkUtil::ResampleImage<TOutputImage,TOutputImage>(OutputCastImage,
        m_UnNormalizedFixedImage->GetLargestPossibleRegion().GetSize(),
        m_UnNormalizedFixedImage->GetSpacing(),
        m_UnNormalizedFixedImage->GetOrigin(),
        m_UnNormalizedFixedImage->GetDirection(),
        this->m_DefaultPixelValue
        ) ;
      if (this->m_OutDebug == std::string ("ON"))
        {
        std::cout<<"Writing Resampled Moving image"<<std::endl;
        itkUtil::WriteImage<TOutputImage>(m_UnNormalizedMovingImage , "ResampledMovingImage.nii.gz" );
        }
      }
    else
      {
      m_UnNormalizedMovingImage=OutputCastImage;
      }
    }

    {
    //+DANGER: ALIASING:  m_OutputMovingImage EQ m_UnNormalizedMovingImage by design.

    //Histogram match the fixed and moving image.
    typedef HistogramMatchingImageFilter<OutputImageType,OutputImageType> HistogramMatchingFilterType;
    typename HistogramMatchingFilterType::Pointer histogramfilter = HistogramMatchingFilterType::New();
    std::cout << "Performing Histogram Matching \n";

    histogramfilter->SetInput( m_UnNormalizedFixedImage  );
    histogramfilter->SetReferenceImage( m_UnNormalizedMovingImage);
    histogramfilter->SetNumberOfHistogramLevels( m_NumberOfHistogramLevels );
    histogramfilter->SetNumberOfMatchPoints( m_NumberOfMatchPoints );
    histogramfilter->ThresholdAtMeanIntensityOn();
    histogramfilter->Update();
    m_OutputFixedImage  = histogramfilter->GetOutput();
    m_OutputMovingImage = m_UnNormalizedMovingImage;
    }

    if (this->m_OutDebug == std::string ("ON"))
      {
      std::cout<<"Writing Histogram equalized image"<<std::endl;
      itkUtil::WriteImage< TOutputImage >(m_OutputFixedImage , "HistogramModifiedFixedImage.nii.gz");
      itkUtil::WriteImage< TOutputImage >(m_UnNormalizedMovingImage , "HistogramReferenceMovingImage.nii.gz");
      }


    //Make BOBF Images if specified
    if(this->m_BOBFTargetMask != std::string ("none"))
      {
      std::cout << "Making BOBF \n";
      std::cout<<"PRE Fixed Origin"<< m_OutputFixedImage->GetOrigin() <<std::endl;
      m_OutputFixedImage = this->MakeBOBFImage( m_OutputFixedImage , m_BOBFTargetMask );
      std::cout<<"Fixed Origin"<< m_OutputFixedImage->GetOrigin() <<std::endl;
      std::cout<<"PRE Moving Origin"<< m_OutputMovingImage->GetOrigin() <<std::endl;
      m_OutputMovingImage = this->MakeBOBFImage( m_OutputMovingImage , m_BOBFTemplateMask);
      std::cout<<"Moving Origin"<< m_OutputMovingImage->GetOrigin() <<std::endl;

      if (this->m_OutDebug == std::string ("ON"))
        {
        std::cout<<"Writing Brain Only Background Filled Moving image"<<std::endl;
        itkUtil::WriteImage< TOutputImage >(m_OutputMovingImage, "BOBF_Moving.nii.gz");
        itkUtil::WriteImage< TOutputImage >(m_OutputFixedImage , "BOBF_Fixed.nii.gz");
        }
      }
    m_InputMovingImage = NULL;
    m_InputFixedImage = NULL;
  }

  /*This function takes in a brain image and a whole brain mask and strips the skull of the image. It uses the BOBF filter to perform the skull stripping.*/

  template <typename TInputImage, typename  TOutputImage>
  typename DemonsPreprocessor<TInputImage,TOutputImage>::OutputImagePointer DemonsPreprocessor<TInputImage,TOutputImage>
  ::MakeBOBFImage( OutputImagePointer input, std::string MaskName )
  {
    OutputImagePointer Mask = itkUtil::ReadImageCoronal<OutputImageType>(MaskName);

    if( (m_UnNormalizedFixedImage->GetLargestPossibleRegion().GetSize() != Mask->GetLargestPossibleRegion().GetSize()) || (m_UnNormalizedFixedImage->GetSpacing() != Mask->GetSpacing()))
      {
      Mask = itkUtil::ResampleImage<TOutputImage,TOutputImage>(Mask,
        m_UnNormalizedFixedImage->GetLargestPossibleRegion().GetSize(),
        m_UnNormalizedFixedImage->GetSpacing(),
        m_UnNormalizedFixedImage->GetOrigin(),
        m_UnNormalizedFixedImage->GetDirection(),
        this->m_DefaultPixelValue
      ) ;
      if (this->m_OutDebug == std::string ("ON"))
        {
        std::cout<<"Writing Resampled Output image"<<std::endl;
        itkUtil::WriteImage<TOutputImage>(Mask , "Resampled.mask");
        }

      }

    typedef BOBFFilter<OutputImageType,OutputImageType> BOBFFilterType;
    typename BOBFFilterType::Pointer BOBFfilter = BOBFFilterType::New();

    std::cout << "Making Brain only Background filled image with the following parameters. " << std::endl;

    std::cout << "Lower Threshold:  " << m_Lower << std::endl;
    std::cout << "Upper Threshold:  " << m_Upper << std::endl;
    std::cout << "Neighborhood:  " << m_Radius << std::endl;
    std::cout << "Background fill Value:  " << m_DefaultPixelValue << std::endl;
    std::cout << "Seed :  " << m_Seed  << std::endl;


    BOBFfilter->SetLower(m_Lower );
    BOBFfilter->SetUpper( m_Upper );
    BOBFfilter->SetRadius( m_Radius );
    BOBFfilter->SetReplaceValue( m_DefaultPixelValue );
    BOBFfilter->SetSeed( m_Seed );
    BOBFfilter->SetInputImage( input );
    BOBFfilter->SetInputMask( Mask );
    try
      {
      BOBFfilter->Update();
      }
    catch (itk::ExceptionObject & err)
      {
      std::cout << "Exception Object caught: " << std::endl;
      std::cout << err << std::endl;
      exit (-1);
      }

    OutputImagePointer output = BOBFfilter->GetOutput();
    return output;
  }


} // namespace itk

#endif
