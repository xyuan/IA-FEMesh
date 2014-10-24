#include "itkBrains2MaskImageIO.h"
#include "itkBrains2MaskImageIOFactory.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkSpatialOrientation.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

/*This program takes in a deformation field and an input image and outputs the warped image.*/

    template <typename InterpolatorType, typename ImageType>
int  WarpFunction(std::string inputfilename, std::string outputfilename,   itk::Image< itk::Vector< float , 3 > ,  3 >::ConstPointer deformationField )
{
    typedef   float  PixelType;
    const     unsigned int   Dimension = 3;
    typedef   float VectorComponentType;
    typedef   itk::Vector< VectorComponentType, Dimension > VectorPixelType;
    typedef   itk::Image< VectorPixelType,  Dimension >   DeformationFieldType;

    typedef   itk::ImageFileReader< ImageType >  ReaderType;

   typename ReaderType::Pointer reader = ReaderType::New();

    reader->SetFileName( inputfilename.c_str() );
    try
        {
        reader->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        }
    
    //Set the warping filter.
    typedef typename itk::WarpImageFilter< ImageType, 
            ImageType, 
            DeformationFieldType  >  FilterType;

    typename FilterType::Pointer filter = FilterType::New();


    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();

//    filter->SetInterpolator( interpolator );
    filter->SetOutputSpacing( deformationField->GetSpacing() );
    filter->SetOutputOrigin(  deformationField->GetOrigin() );
    filter->SetEdgePaddingValue(  0 );
    filter->SetDeformationField( deformationField );
    filter->SetInput( reader->GetOutput() );

    try
        {
        filter->Update ();
        }
    catch (itk::ExceptionObject & err)
        {
        std::cout << "Caught an ITK exception: " << std::endl;
        std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
        throw err;
        }

   typename ImageType::Pointer image = ImageType::New ();
   image = filter->GetOutput ();
   image->SetDirection( deformationField->GetDirection( ) );

    //Write the Image.
    typedef   itk::ImageFileWriter< ImageType >  WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputfilename.c_str() );
    writer->SetInput(image);
    try
        {
        writer->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        }

    return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
    if( argc != 6 )
        {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << "  inputImageFile  inputDeformationField  outputImageFile interptype imagetype" << std::endl;
        std::cerr << "Interpolation types:" << std::endl;
        std::cerr << " 'l' - Linear Interpolation" << std::endl;
        std::cerr << " 'n' - Nearest Neighborhood Interpolation" << std::endl;
        std::cerr << " 'b' - Bspline Interpolation. " << std::endl;
        std::cerr << "Image types:" << std::endl;
        std::cerr << " 'UCHAR' - Unsigned 8 bit" << std::endl;
        std::cerr << " 'SHORT' - Signed 16 bit" << std::endl;
        std::cerr << " 'FLOAT' - Single precesion floating point" << std::endl;
        return EXIT_FAILURE;
        }
    itk::Brains2MaskImageIOFactory::RegisterOneFactory();

    const     unsigned int   Dimension = 3;
    typedef   float VectorComponentType;
    typedef   itk::Vector< VectorComponentType, Dimension > VectorPixelType;
    typedef   itk::Image< VectorPixelType,  Dimension >   DeformationFieldType;



    //Read the deformation field
    typedef   itk::ImageFileReader< DeformationFieldType >  FieldReaderType;
    FieldReaderType::Pointer fieldReader = FieldReaderType::New();
    fieldReader->SetFileName( argv[2] );
    fieldReader->Update();
    DeformationFieldType::ConstPointer deformationField = fieldReader->GetOutput();

    int status=-1;
    if(std::string(argv[4]) == "l")
        {
        if (std::string(argv[5]) == "UCHAR")
          {
          typedef   itk::Image< unsigned char ,  Dimension >   ImageType;
          typedef itk::LinearInterpolateImageFunction< ImageType, float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField);
          }
        else if (std::string(argv[5]) == "SHORT")
          {
          typedef   itk::Image< signed short ,  Dimension >   ImageType;
          typedef itk::LinearInterpolateImageFunction< ImageType, float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField); 
          }
        else if (std::string(argv[5]) == "FLOAT")
          {
          typedef   itk::Image< float ,  Dimension >   ImageType;
          typedef itk::LinearInterpolateImageFunction< ImageType, float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField); 
          }
        else
          {
          std::cerr << "Invalid data type specified. Must be UCHAR, SHORT, or FLOAT" << std::endl;
          EXIT_FAILURE;
          }
        }
    else if(std::string(argv[4]) == "n")
        { 
        if (std::string(argv[5]) == "UCHAR")
          {
          typedef   itk::Image< unsigned char ,  Dimension >   ImageType;
          typedef itk::NearestNeighborInterpolateImageFunction< ImageType, float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField);
          }
        else if (std::string(argv[5]) == "SHORT")
          {
          typedef   itk::Image< signed short ,  Dimension >   ImageType;
          typedef itk::NearestNeighborInterpolateImageFunction< ImageType, float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField); 
          }
        else if (std::string(argv[5]) == "FLOAT")
          {
          typedef   itk::Image< float ,  Dimension >   ImageType;
          typedef itk::NearestNeighborInterpolateImageFunction< ImageType, float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField); 
          }
        else
          {
          std::cerr << "Invalid data type specified. Must be UCHAR, SHORT, or FLOAT" << std::endl;
          EXIT_FAILURE;
          }
        }
    else if(std::string(argv[4]) == "b")
        {       
        if (std::string(argv[5]) == "UCHAR")
          {
          typedef   itk::Image< unsigned char ,  Dimension >   ImageType;
          typedef itk::BSplineInterpolateImageFunction< ImageType, float , float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField);
          }
        else if (std::string(argv[5]) == "SHORT")
          {
          typedef   itk::Image< signed short ,  Dimension >   ImageType;
          typedef itk::BSplineInterpolateImageFunction< ImageType, float , float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField); 
          }
        else if (std::string(argv[5]) == "FLOAT")
          {
          typedef   itk::Image< float ,  Dimension >   ImageType;
          typedef itk::BSplineInterpolateImageFunction< ImageType, float , float >  InterpolatorType;
          status= WarpFunction<InterpolatorType, ImageType>(argv[1], argv[3], deformationField); 
          }
        else
          {
          std::cerr << "Invalid data type specified. Must be UCHAR, SHORT, or FLOAT" << std::endl;
          EXIT_FAILURE;
          }
        } 
    else
        {
        std::cerr << "Please set the Interpolation as one of these types" << std::endl;
        std::cerr << " 'l' - Linear Interpolation" << std::endl;
        std::cerr << " 'n' - Nearest Neighborhood Interpolation" << std::endl;
        std::cerr << " 'b' - Bspline Interpolation. " << std::endl;
        return EXIT_FAILURE;
        }    

    return status;
}



