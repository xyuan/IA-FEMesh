#ifndef _ThirionRegistration_txx
#define _ThirionRegistration_txx

#include "ThirionRegistration.h"

namespace itk
{

    template <typename TImage, typename TRealImage, typename TOutputImage >
        ThirionRegistration<TImage,TRealImage, TOutputImage >
        ::ThirionRegistration()
            {
            m_TheMovingImageFileName = "";
            m_TheFixedImageFileName = "";

            m_ParameterFileName = "";
            m_OutputFileName = "";
            m_AppendOutputFile = true;
            m_WarpedImageName="none";
            m_CheckerBoardFileName="none";
            m_DeformationFieldOutputName="none";
            m_DisplacementBaseName="none";
            m_CheckerBoardPattern.Fill( 4 ); 
            m_Lower = NumericTraits<PixelType>::NonpositiveMin();
            m_Upper = NumericTraits<PixelType>::max();
            m_DefaultPixelValue = NumericTraits<PixelType>::Zero;
            m_Radius.Fill(1);
            m_BOBFTargetMask = "none";
            m_BOBFTemplateMask = "none";
            m_OutNormalized = "OFF";
            m_OutDebug = "OFF";
            m_MedianFilterRadius = 0;
            for(unsigned i =0; i < ImageType::ImageDimension; i++)
              {
              m_Seed[i] = 0;
              }
            m_InitialTransformFileName = "";
            }

/*This method initializes the input parser which reads in the moving image, fixed image and parameter file.*/

    template <typename TImage, typename TRealImage, typename TOutputImage >
        void
        ThirionRegistration<TImage,TRealImage, TOutputImage >
        ::InitializeParser()
            {
            this->m_Parser->SetTheMovingImageFileName( this->m_TheMovingImageFileName.c_str() );

            this->m_Parser->SetTheFixedImageFileName( this->m_TheFixedImageFileName.c_str() );

            this->m_Parser->SetParameterFileName( this->m_ParameterFileName.c_str() );
            }


/*This method initializes the preprocessor which processes the moving and fixed images before registration. The image files which are read in using the parser are given to the preprocessor.*/

    template <typename TImage, typename TRealImage, typename TOutputImage >
        void
        ThirionRegistration<TImage,TRealImage, TOutputImage >
        ::InitializePreprocessor()
            {
            this->m_Preprocessor->SetInputFixedImage( this->m_Parser->GetTheFixedImage() );
            this->m_Preprocessor->SetInputMovingImage( this->m_Parser->GetTheMovingImage() );

            this->m_Preprocessor->SetNumberOfHistogramLevels( this->m_Parser->GetNumberOfHistogramLevels() );
            this->m_Preprocessor->SetNumberOfMatchPoints( this->m_Parser->GetNumberOfMatchPoints() );
            this->m_Preprocessor->SetBOBFTargetMask(this->GetBOBFTargetMask());
            this->m_Preprocessor->SetBOBFTemplateMask(this->GetBOBFTemplateMask());
            this->m_Preprocessor->SetLower(this->GetLower());
            this->m_Preprocessor->SetUpper(this->GetUpper());
            this->m_Preprocessor->SetRadius(this->GetRadius());
            this->m_Preprocessor->SetDefaultPixelValue(this->GetDefaultPixelValue());
            this->m_Preprocessor->SetSeed(this->GetSeed());
            this->m_Preprocessor->SetOutDebug(this->GetOutDebug());
            this->m_Preprocessor->SetMedianFilterRadius(this->GetMedianFilterRadius());
            }


/*This method initializes the registration process. The preprocessed output files are passed to the registrator.*/

    template <typename TImage, typename TRealImage, typename TOutputImage >
        void
        ThirionRegistration<TImage,TRealImage,  TOutputImage >
        ::InitializeRegistrator()
            {
            this->m_Registrator->SetDisplacementBaseName(this->GetDisplacementBaseName());
            this->m_Registrator->SetWarpedImageName(this->GetWarpedImageName());
            this->m_Registrator->SetCheckerBoardFileName(this->GetCheckerBoardFileName());
            this->m_Registrator->SetDeformationFieldOutputName(this->GetDeformationFieldOutputName());
            this->m_Registrator->SetCheckerBoardPattern(this->GetCheckerBoardPattern());
            this->m_Registrator->SetFixedImage( this->m_Preprocessor->GetOutputFixedImage() );
            this->m_Registrator->SetMovingImage( this->m_Preprocessor->GetOutputMovingImage() );
            this->m_Registrator->SetUnNormalizedMovingImage( this->m_Preprocessor->GetUnNormalizedMovingImage()  );
            this->m_Registrator->SetUnNormalizedFixedImage( this->m_Preprocessor->GetUnNormalizedFixedImage()  );
            this->m_Registrator->SetDefaultPixelValue( this->m_Preprocessor->GetDefaultPixelValue()  );

            this->m_Registrator->SetNumberOfLevels( this->m_Parser->GetNumberOfLevels() );
            this->m_Registrator->SetNumberOfIterations( this->m_Parser->GetNumberOfIterations() );

            this->m_Registrator->SetFixedImageShrinkFactors( this->m_Parser->GetTheFixedImageShrinkFactors() );
            this->m_Registrator->SetMovingImageShrinkFactors( this->m_Parser->GetTheMovingImageShrinkFactors() );

            this->m_Registrator->SetOutNormalized(this->GetOutNormalized());
            this->m_Registrator->SetOutDebug(this->GetOutDebug());
            //this->m_Registrator->SetInitialDeformationField(this->m_InitialDeformationField);
            this->m_Registrator->SetDeformationFieldOutputName(this->m_DeformationFieldOutputName);
            this->m_Registrator->SetInitialTransformFileName(this->m_InitialTransformFileName);
            }

} // namespace itk

#endif
