
#ifndef _ValidationInputParser_txx
#define _ValidationInputParser_txx

#include "ValidationInputParser.h"
#include "itkMetaDataObject.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"
#include "itkImage.h"
#include "itkAffineTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkIO.h"


namespace itk
{

    template <typename TImage>
        ValidationInputParser<TImage>
        ::ValidationInputParser()
            {

            m_TheMovingImageFileName = "";
            m_TheFixedImageFileName = "" ;

            m_ParameterFileName = "";

            m_TheMovingImage = NULL;
            m_TheFixedImage = NULL;

            m_NumberOfHistogramLevels = 1024;
            m_NumberOfMatchPoints = 7;

            m_NumberOfLevels = 1;
            m_TheMovingImageShrinkFactors.Fill( 1 );
            m_TheFixedImageShrinkFactors.Fill( 1 );

            m_NumberOfIterations = IterationsArrayType(1);
            m_NumberOfIterations.Fill( 10 );

            }



    template <typename TImage>
        void
        ValidationInputParser<TImage>
        ::Execute()
            {

            /*************************
             * Read in the images
             *************************/
              m_TheFixedImage=itkUtil::ReadImageCoronal<TImage>( m_TheFixedImageFileName);
              m_TheMovingImage=itkUtil::ReadImageCoronal<TImage>( m_TheMovingImageFileName);

            /*************************
             * Read in algorithm parameters from file
             *************************/
            FILE * paramFile;
            paramFile = fopen( m_ParameterFileName.c_str(), "r" );
            if( !paramFile )
                {
                itkExceptionMacro( << "Could not open parameter file. " );
                }

            // parse the file
            unsigned int uNumber;

            // get number of histogram levels
            if( fscanf( paramFile, "%d", &uNumber) != 1 )
                {
                itkExceptionMacro( << "Could not find the number of histogram levels." );
                }
            m_NumberOfHistogramLevels = uNumber;


            // get number of match points
            if( fscanf( paramFile, "%d", &uNumber) != 1 )
                {
                itkExceptionMacro( << "Could not find the number of match points." );
                }
            m_NumberOfMatchPoints = uNumber;


            // get number of levels
            if( fscanf( paramFile, "%d", &uNumber) != 1 )
                {
                itkExceptionMacro( << "Could not find the number of levels." );
                }
            m_NumberOfLevels = uNumber;

            // get number of iterations
                {
                itk::Array<unsigned int> temp( m_NumberOfLevels );
                temp.Fill( 0 );
                m_NumberOfIterations = temp;
                }
            for( unsigned int j = 0; j < m_NumberOfLevels; j++ )
                {
                if( fscanf( paramFile, "%d", &uNumber ) != 1 )
                    {
                    itkExceptionMacro( << "Could not find number of iterations per level. " );
                    }
                m_NumberOfIterations[j] = uNumber;
                }

            // get altas shrink factors
            for( unsigned int j = 0; j < ImageDimension; j++ )
                {
                if( fscanf( paramFile, "%d", &uNumber ) != 1 )
                    {
                    itkExceptionMacro( << "Could not find atlas starting shrink factor. " );
                    }
                m_TheMovingImageShrinkFactors[j] = uNumber;
                }

            // get subject shrink factors
            for( unsigned int j = 0; j < ImageDimension; j++ )
                {
                if( fscanf( paramFile, "%d", &uNumber ) != 1 )
                    {
                    itkExceptionMacro( << "Could not find subject starting shrink factor. " );
                    }
                m_TheFixedImageShrinkFactors[j] = uNumber;
                }


            //Print out the parameters.

            std::cout << "NumberOfHistogramLevels : " << m_NumberOfHistogramLevels << std::endl;
            std::cout << "NumberOfMatchPoints : " << m_NumberOfMatchPoints << std::endl;
            std::cout << "NumberOfLevels : " << m_NumberOfLevels << std::endl;
            std::cout << "NumberOfIterations : " << m_NumberOfIterations << std::endl;
            std::cout << "TheMovingImageShrinkFactors : " << m_TheMovingImageShrinkFactors << std::endl;
            std::cout << "TheFixedImageShrinkFactors : " << m_TheFixedImageShrinkFactors << std::endl;

            }


} // namespace itk

#endif
