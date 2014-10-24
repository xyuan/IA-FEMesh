#ifndef _ITKIO_H_
#define _ITKIO_H_

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkOrientImageFilter.h"
#include "itkSpatialOrientation.h"
#include "itkSpatialOrientationAdapter.h"
#include "itkAnalyzeImageIO.h"
#include "itkMetaDataObject.h"
#include "itkImageRegionIterator.h"
#include "itkThinPlateR2LogRSplineKernelTransform.h"
#include "itkInverseConsistentLandmarks.h"
#include "itkResampleImageFilter.h"
#include "itkImageDuplicator.h"
#include "Imgmath.h"

namespace itkUtil
{

  /** read an image using ITK -- image-based template */
  template <typename TImage>
  typename TImage::Pointer ReadImage( const std::string fileName, const bool zeroOrigin = true )
  {
    typedef itk::ImageFileReader<TImage> ReaderType;

    typename ReaderType::Pointer reader = ReaderType::New();
    {
    reader->SetFileName( fileName.c_str() );
    try
      {
      reader->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
      throw err;
      }
    catch(...)
      {
      std::cout << "Error while reading in image for patient " << fileName << std::endl;
      throw;
      }
    }
    typename TImage::Pointer image = reader->GetOutput();
    if(zeroOrigin)
      {
      double origin[TImage::ImageDimension];
      for(unsigned int i =0 ; i< TImage::ImageDimension ; i++)
        {
        origin[i]=0;
        }
      image->SetOrigin(origin);
      }
    return image;
  }


  template <class ImageType>
  typename ImageType::Pointer 
  ReadImageAndOrient(const std::string &filename,
                     itk::SpatialOrientation::ValidCoordinateOrientationFlags orient, 
                     const bool zeroOrigin = true )
  {
    typename ImageType::Pointer img =
      ReadImage<ImageType>(filename, zeroOrigin);

    typename itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter = 
      itk::OrientImageFilter<ImageType,ImageType>::New();
    
    orienter->SetDesiredCoordinateOrientation(orient);
    orienter->UseImageDirectionOn();
    orienter->SetInput(img);
    orienter->Update();

    typename ImageType::Pointer  image = orienter->GetOutput();
    if(zeroOrigin)
      {
      double origin[ImageType::ImageDimension];
      for(unsigned int i =0 ; i< ImageType::ImageDimension ; i++)
        {
        origin[i]=0;
        }
      image->SetOrigin(origin);
      }
    return image;
  }


  template <class ImageType>
  typename ImageType::Pointer 
  ReadImageAndOrient(const std::string &filename,
                     const typename itk::SpatialOrientationAdapter::DirectionType &dir, 
                     const bool zeroOrigin = true)
  {
    return ReadImageAndOrient<ImageType>
      (filename,itk::SpatialOrientationAdapter().FromDirectionCosines(dir),zeroOrigin);
  }


  template <typename  TReadImageType>
  typename TReadImageType::Pointer ReadImageCoronal( const std::string &fileName,const bool zeroOrigin = true)
  {
    //
    // TODO: is there any real reason to check after the read to see if the direction is correct?
    typename itk::SpatialOrientationAdapter::DirectionType CORdir=
      itk::SpatialOrientationAdapter().ToDirectionCosines(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    //   return ReadImageAndOrient<TReadImageType>(fileName,CORdir);
    return ReadImageAndOrient<TReadImageType>(fileName,CORdir,zeroOrigin);
  }

  template <class ImageType>
  void
  WriteImage(typename ImageType::Pointer &image ,
             const std::string &filename)
  {

    typedef itk::ImageFileWriter< ImageType > WriterType;
    typename  WriterType::Pointer writer = WriterType::New();

    writer->SetFileName(filename.c_str());

    writer->SetInput(image);

    try
      {
      writer->Update();
      }
    catch (itk::ExceptionObject &err) {
    std::cout << "Exception Object caught: " << std::endl;
    std::cout << err << std::endl;
    throw;
    }
  }


  template <class InputImageType,class OutputImageType>
  typename OutputImageType::Pointer 
  TypeCast(const typename InputImageType::Pointer &input)
  {

    typedef itk::CastImageFilter< InputImageType, OutputImageType> CastToRealFilterType;
    typename CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();

    toReal->SetInput(input);

    toReal->Update();

    return toReal->GetOutput();

  }



  template <class InputImageType, class OutputImageType>
  void 
  WriteCastImage(std::string &filename,
                 const typename InputImageType::Pointer &input )
  {
    typename OutputImageType::Pointer out =
      TypeCast<InputImageType,OutputImageType>(input);
    WriteImage<OutputImageType>(out,filename);
  }


  template <class ImageType>
  typename ImageType::Pointer 
  CopyImage(const typename ImageType::Pointer &input )
  {
    typedef itk::ImageDuplicator<ImageType> ImageDupeType;
    typename ImageDupeType::Pointer MyDuplicator = ImageDupeType::New();
    MyDuplicator->SetInputImage(input);
    MyDuplicator->Update();
    return MyDuplicator->GetOutput();
  }


  template <class ImageType>
  void Denormalize(typename ImageType::Pointer &input , int nx ,int ny , int nz )

  {
    typedef typename itk::ImageRegionIterator<ImageType> ConstIteratorType;
    ConstIteratorType in1(input , input->GetLargestPossibleRegion());

    for(in1.GoToBegin() ; !in1.IsAtEnd() ; ++in1)
      {
      in1.Set(in1.Get()*nx*ny*nz);
      }

  }


  template <class ImageType>
  void Normalize(typename ImageType::Pointer &input )

  {
    itk::ImageRegionIterator< ImageType >  in1(input , input->GetLargestPossibleRegion());

    for(in1.GoToBegin() ; !in1.IsAtEnd() ; ++in1)
      {
      const typename ImageType::PixelType value=static_cast<typename ImageType::PixelType>(static_cast<float>(in1.Get())/255.0F);
      in1.Set(value);
      }

  }


  template <class ImageType>
  typename ImageType::Pointer
  ImageReinitialize( int nx , double vx ,int ny, double vy , int nz , double vz , typename ImageType::DirectionType &direction )
  {

    typename ImageType::SizeType size;
    size[0] = nx;
    size[1] = ny;
    size[2] = nz;

    typename ImageType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    double space[ImageType::ImageDimension];
    space[0] = vx; space[1] = vy; space[2] = vz;

    typename ImageType::RegionType region;
    region.SetIndex( start );
    region.SetSize(  size  );

    typename ImageType::Pointer image =  ImageType::New();
    image->SetRegions(region);
    image->SetSpacing(space);
    image->SetDirection(direction);
    image->Allocate();
    image->FillBuffer(0.0F);
    return image;
  }

  inline double bigger(const double x)
  {
    const double y = vcl_ceil(x);
    return (y==x)?y+1.0:y;
  }

  template <class ImageType>
  typename ImageType::Pointer LinearRescaleImage(const typename ImageType::Pointer &input,
                                                 int newnx , int newny , int newnz )
  {
    //HACK:  This should really use the ITK image rescaling with linear rescaling.  This code is the old c version of the same thing.

    //        typename ImageType::PixelType minvalue = input1->GetPixel(pixelindex);
    const typename ImageType::SizeType InputImageSize = input->GetLargestPossibleRegion().GetSize();

    /* Compute scale factor for rows and columns */
    const double nxfactor = (newnx > 0 )?static_cast<double>(InputImageSize[0])/static_cast<double>(newnx):1.0;
    const double nyfactor = (newny > 0 )?static_cast<double>(InputImageSize[1])/static_cast<double>(newny):1.0;
    const double nzfactor = (newnz > 0 )?static_cast<double>(InputImageSize[2])/static_cast<double>(newnz):1.0;


    typename ImageType::Pointer output = ImageType::New();
    {
    typename ImageType::RegionType region;
    const typename ImageType::SizeType outputsize = {{ newnx, newny, newnz }};
    region.SetSize(outputsize);

    typename ImageType::IndexType start = {{0,0,0}};
    region.SetIndex(start);
    output->SetRegions(region);
    }
    {
    const itk::Vector<double, 3> InputImageSpacing = input->GetSpacing();
    double outspace[ImageType::ImageDimension];
    outspace[0]=InputImageSpacing[0]*nxfactor;
    outspace[1]=InputImageSpacing[1]*nyfactor;
    outspace[2]=InputImageSpacing[2]*nzfactor;
    output->SetSpacing(outspace);
    }
    output->SetOrigin(input->GetOrigin());
    output->SetDirection(input->GetDirection());
    output->Allocate();

    const bool nxscaledown = (nxfactor > 1.0) ? true:false;
    const bool nyscaledown = (nyfactor > 1.0) ? true:false;
    const bool nzscaledown = (nzfactor > 1.0) ? true:false;

    const double nxscale = ( nxscaledown ) ? 1.0/nxfactor: 1.0F;
    const double nyscale = ( nyscaledown ) ? 1.0/nyfactor: 1.0F;
    const double nzscale = ( nzscaledown ) ? 1.0/nzfactor: 1.0F;

    const double invvolume=nxscale*nyscale*nzscale;

    /* For each pixel in the new image compute a grey level
     *    based on interpolation from the original image       */

    //Could be determined automatically to allow arbitrary scaling.
    //NOTE: 25 is largest scale expansion factor allowed!
    const unsigned int scale_array_size=256;
    double  nxw[scale_array_size],
      nyw[scale_array_size],
      nzw[scale_array_size];
    assert(nxfactor+2 < scale_array_size);
    assert(nyfactor+2 < scale_array_size);
    assert(nzfactor+2 < scale_array_size);
    for(int k=0; k<newnz; k++)
      {
      const double z_oldpixel=static_cast<double>(k)*nzfactor;
      const double biggerz_oldpixel=bigger(z_oldpixel);
      /* Set up the nz re-scale */
      nzw[0] = biggerz_oldpixel - z_oldpixel;
      const int nzstart = static_cast<int>(vcl_floor(z_oldpixel));
      int nzwindex=1;
      if(nzscaledown)
        {
        double xf = nzfactor - nzw[0];
        while (xf >= 1.0)
          {
          nzw[nzwindex++] = 1.0;
          xf -= 1.0;
          }
        nzw[nzwindex] = xf;
        }
      else
        {
        nzw[1]=1.0-nzw[0];
        }
      for (int j=0; j<newny; j++)
        {
        const double y_oldpixel=static_cast<double>(j)*nyfactor;
        const double biggery_oldpixel=bigger(y_oldpixel);
        /* Set up the ny re-scale */
        nyw[0] = biggery_oldpixel - y_oldpixel;
        const int nystart = static_cast<int>(vcl_floor(y_oldpixel));
        int nywindex=1;
        if(nyscaledown)
          {
          double xf = nyfactor - nyw[0];
          while (xf >= 1.0)
            {
            nyw[nywindex++] = 1.0;
            xf -= 1.0;
            }
          nyw[nywindex] = xf;
          }
        else
          {
          nyw[1]=1.0-nyw[0];
          }

        typename ImageType::IndexType pixelindex;
        pixelindex[0]=0;
        pixelindex[1]=0;
        pixelindex[2]=0;
        for (int i=0; i<newnx; i++)
          {
          const double x_oldpixel=static_cast<double>(i)*nxfactor;
          const double biggerx_oldpixel=bigger(x_oldpixel);
          /* Set up the nx re-scale */
          nxw[0] = biggerx_oldpixel - x_oldpixel;
          const int nxstart = static_cast<int>(vcl_floor(x_oldpixel));
          int nxwindex=1;
          if(nxscaledown)
            {
            double xf = nxfactor - nxw[0];
            while (xf >= 1.0)
              {
              nxw[nxwindex++] = 1.0;
              xf -= 1.0;
              }
            nxw[nxwindex] = xf;
            }
          else
            {
            nxw[1]=1.0-nxw[0];
            }
          /* Collect and weight pixels from the original into the new pixel  */
          float accum = 0.0;
          for (int kk=0; kk <= nzwindex; kk++)
            {
            const unsigned int current_z=kk+nzstart;
            if (current_z >= InputImageSize[2])
              {
              continue;
              }
            for (int jj=0; jj<= nywindex; jj++)
              {
              const unsigned int current_y=jj+nystart;
              if (current_y >= InputImageSize[1])
                {
                continue;
                }
              for (int ii=0; ii <= nxwindex; ii++)
                {
                const unsigned int current_x=ii+nxstart;
                if (current_x >= InputImageSize[0])
                  {
                  continue;
                  }
                pixelindex[0]= current_x ; pixelindex[1]= current_y ; pixelindex[2]= current_z;
                typename ImageType::PixelType accumtemp = input->GetPixel(pixelindex);
                accum += nxw[ii]*nyw[jj]*nzw[kk]*accumtemp;
                }
              }
            }
          accum = accum*invvolume;

          //         if (accum > 255.0)
          //         {
          //           printf ("%lf at (%d,%d,%d)\n",accum,i,j,k);
          //         }
          pixelindex[0]= i ; pixelindex[1]= j ; pixelindex[2]= k;

          output->SetPixel(pixelindex , static_cast<typename ImageType::PixelType>(accum));
          }
        }
      }
    return output;
  }
  typedef itk::Image<itk::Vector<float,3>, 3> DeformationFieldType;
  typedef itk::Image<float,3> FloatImage3DType;

  /*This function takes in the moving image and resamples it to fixed image space. It uses the itk resample image filter to perform the function.*/
  template <typename TInputImage, typename  TOutputImage>
  typename TOutputImage::Pointer
  ResampleImage( typename TInputImage::Pointer &input,
                 const typename TInputImage::SizeType Size,
                 const typename TInputImage::SpacingType Spacing ,
                 const typename TInputImage::PointType Origin,
                 const typename TInputImage::DirectionType Direction,
                 const typename TOutputImage::PixelType BackgroundValue=itk::NumericTraits<typename TOutputImage::PixelType>::Zero)
  {

    typedef itk::ResampleImageFilter < TInputImage , TOutputImage > ResampleImageType;
    typename ResampleImageType::Pointer Resamplefilter = ResampleImageType::New ();

    //Set Space
    std::cout<<"Setting Moving Image spacing to Fixed Image spacing. New Space = "
             << Spacing << " Old Space= " << input->GetSpacing() <<std::endl;
    Resamplefilter->SetOutputSpacing( Spacing );

    //Set Size
    std::cout<<"Setting Moving Image size to Fixed Image size. New Size = "
             << Size
             << " Old Size= " << input->GetLargestPossibleRegion().GetSize()<< std::endl;
    std::cout << "Original Origins input: " << input->GetOrigin()
              << " Fixed Image: " << Origin << std::endl;
    Resamplefilter->SetSize(Size );
    Resamplefilter->SetOutputDirection(Direction);

    //Set Origin
    std::cout<<" Setting origin to half the physical distance between images in each dimension" <<std::endl;
    double origin[TOutputImage::ImageDimension];

    for(unsigned int i=0 ; i<TOutputImage::ImageDimension ; i++)
      {
      origin[i]= ((input->GetSpacing()[i]*input->GetLargestPossibleRegion().GetSize()[i])
                  -(Spacing[i]*Size[i]))/2.0F;
      std::cout<< "Resampled data for each dimension. " << std::endl;
      std::cout<<"Dimension = " << i+1 << "  Origin = "<< origin[i]
               << "  Size =  " << Size[i]
               << "  Spacing =  " << Spacing[i] <<std::endl;
      }
    Resamplefilter->SetOutputOrigin( origin );

    //set Interpolation
    typename itk::LinearInterpolateImageFunction< TOutputImage , double >::Pointer interpolator
      = itk::LinearInterpolateImageFunction< TOutputImage , double >::New();
    Resamplefilter->SetInterpolator( interpolator );

    //set Transform
    typename itk::AffineTransform< double, TOutputImage::ImageDimension >::Pointer transform
      = itk::AffineTransform< double, TOutputImage::ImageDimension >::New();
    Resamplefilter->SetTransform( transform );
    Resamplefilter->SetDefaultPixelValue(BackgroundValue);
    Resamplefilter->SetInput( input );

    try
      {
      Resamplefilter->UpdateLargestPossibleRegion();
      Resamplefilter->Update();
      }
    catch (itk::ExceptionObject & excp)
      {
      std::cerr << "Error reading the series " << std::endl;
      std::cerr << excp << std::endl;
      exit (-1);
      }
    //We have resampled the input image and set the output to the input pointer
    typename TOutputImage::Pointer output = Resamplefilter->GetOutput();

    for(unsigned int i =0 ; i< TOutputImage::ImageDimension ; i++)
      {
      origin[i]=0;
      }
    output->SetOrigin(origin);

    return output;
  }


  template <class ImageType>
  void ImageClear(typename ImageType::Pointer &input )
  {
    typedef itk::ImageRegionIterator<ImageType> ConstIteratorType;
    ConstIteratorType in1(input , input->GetLargestPossibleRegion());

    for(in1.GoToBegin() ; !in1.IsAtEnd() ; ++in1)
      {
      in1.Set(0.0);
      }
  }



  template <class ImageType>
  float GetMax( const typename ImageType::Pointer &rhs)
  {

    typedef itk::ImageRegionIterator< ImageType > ConstIteratorType;
    ConstIteratorType in1(rhs , rhs->GetLargestPossibleRegion());

    in1.GoToBegin();
    typename ImageType::PixelType maxvalue = in1.Get();
    // first element read above, skip it and look at rest of image.
    for(++in1; !in1.IsAtEnd() ; ++in1)
      {
      typename ImageType::PixelType current = in1.Get();
      maxvalue = (current > maxvalue) ? current : maxvalue;
      }
    return maxvalue;
  }



  template <class ImageType>
  float GetMin( const typename ImageType::Pointer &rhs)
  {

    typedef itk::ImageRegionIterator< ImageType > ConstIteratorType;
    ConstIteratorType in1(rhs , rhs->GetLargestPossibleRegion());

    in1.GoToBegin();
    typename ImageType::PixelType minvalue=in1.Get();

    // first element read above, skip it and look at rest of image.
    for(++in1; !in1.IsAtEnd() ; ++in1)
      {
      const typename ImageType::PixelType current=in1.Get();
      minvalue=(current < minvalue)?current:minvalue;
      }
    return minvalue;
  }

  // versions of GetMin and GetMax for images with vector pixeltype
  template <class ImageType>
  float GetMax( const typename ImageType::Pointer &rhs,unsigned Index)
  {
    typedef itk::ImageRegionIterator< ImageType > ConstIteratorType;
    ConstIteratorType in1(rhs , rhs->GetLargestPossibleRegion());

    in1.GoToBegin();
    typename ImageType::PixelType maxvalue=in1.Get();
    for(++in1 ; !in1.IsAtEnd() ; ++in1)
      {
      typename ImageType::PixelType current = in1.Get();
      maxvalue[Index] = 
        (current[Index] > maxvalue[Index]) ? current[Index] : maxvalue[Index];
      }
    return maxvalue[Index];
  }



  template <class ImageType>
  float GetMin( const typename ImageType::Pointer &rhs,unsigned Index)
  {

    typedef itk::ImageRegionIterator< ImageType > ConstIteratorType;
    ConstIteratorType in1(rhs , rhs->GetLargestPossibleRegion());

    in1.GoToBegin();
    typename ImageType::PixelType minvalue=in1.Get();

    for(++in1; !in1.IsAtEnd() ; ++in1)
      {
      const typename ImageType::PixelType current=in1.Get();
      minvalue[Index]  = 
        (current[Index] < minvalue[Index]) ? current[Index] : minvalue[Index];
      }
    return minvalue[Index];
  }

}

// a nasty hack to get around recursive dependencies between
// itkIO.h and TransformToDeformationField.h
#include "TransformToDeformationField.h"

namespace itkUtil
{
  template <typename DeformationFieldType, typename ImageType>
  typename DeformationFieldType::Pointer
  RegisterLandmarksToDeformationField(typename ImageType::Pointer &InputImage,
                                      const std::string &InputLandmarkFilename,
                                      const std::string &TemplateLandmarkFilename,
                                      typename ImageType::Pointer &TemplateImage)
  {
    try
      {
      typedef itk::ThinPlateR2LogRSplineKernelTransform<double,3>  ThinPlateSplineTransformType;
      typedef ThinPlateSplineTransformType::Pointer ThinPlateSplineTransformPointer;
      typedef ThinPlateSplineTransformType::PointSetType PointSetType;
      typedef itk::InverseConsistentLandmarks<double, PointSetType> LandmarksType;

      ThinPlateSplineTransformPointer ThinPlateSplineTransformITK = ThinPlateSplineTransformType::New();

      typename ImageType::SizeType OutputSize = TemplateImage->GetLargestPossibleRegion().GetSize();
      typename ImageType::SpacingType OutputSpacing = TemplateImage->GetSpacing();

      typename ImageType::SizeType InputSize = InputImage->GetLargestPossibleRegion().GetSize();
      typename ImageType::SpacingType InputSpacing = InputImage->GetSpacing();

      LandmarksType InputLandmarks;
      LandmarksType TemplateLandmarks;
      unsigned int InputLandmarkSize;
      unsigned int TemplateLandmarkSize;
      bool landmarkReadTest = InputLandmarks.ReadPointTypes(InputLandmarkFilename.c_str());
      if(!landmarkReadTest)
        {
        std::cout << "Error in Input Landmark File Read." << std::endl;
        exit(-1);
        }
      landmarkReadTest = TemplateLandmarks.ReadPointTypes(TemplateLandmarkFilename.c_str());
      if(!landmarkReadTest)
        {
        std::cout << "Error in Template Landmark File Read." << std::endl;
        exit(-1);
        }

      InputLandmarks.RemoveUnmatchedPoints(TemplateLandmarks);
      TemplateLandmarks.RemoveUnmatchedPoints(InputLandmarks);

      InputLandmarkSize = InputLandmarks.size();
      TemplateLandmarkSize = TemplateLandmarks.size();

      double Origin[3] = {0,0,0};
      double ImageOrigin[3] = {0,0,0};
      for(int i=0; i<3; i++)
        {
        Origin[i] = (InputSize[i]*InputSpacing[i]-OutputSize[i]*OutputSpacing[i])/2;
        ImageOrigin[i] = (OutputSize[i]*OutputSpacing[i] - InputSize[i]*InputSpacing[i])/2;
        }
      double ZeroOrigin[3] = {0,0,0};

      ThinPlateSplineTransformITK->SetSourceLandmarks(TemplateLandmarks.GetPointSet(ZeroOrigin));
      ThinPlateSplineTransformITK->SetTargetLandmarks(InputLandmarks.GetPointSet(Origin));
      ThinPlateSplineTransformITK->ComputeWMatrix();
      return TransformToDeformationField
        <DeformationFieldType,ThinPlateSplineTransformType>
        (TemplateImage->GetLargestPossibleRegion().GetSize(),
         TemplateImage->GetSpacing(),
         ThinPlateSplineTransformITK);
      }
    catch(...)                  // if anything fails, return empty smart pointer.
      {
      return typename DeformationFieldType::Pointer();
      }
  }
  template <typename DeformationFieldType, typename ImageType>
  typename DeformationFieldType::Pointer
  RegisterLandmarksToDeformationField(const std::string &InputImageFilename,
                                      const std::string &InputLandmarkFilename,
                                      const std::string &TemplateLandmarkFilename,
                                      const std::string &TemplateImageFilename)
  {
    typename ImageType::Pointer TemplateImage;

    typename ImageType::Pointer InputImage;
    try
      {
      TemplateImage = itkUtil::ReadImageCoronal<ImageType>(TemplateImageFilename.c_str(),true);
      InputImage = itkUtil::ReadImageCoronal<ImageType>(InputImageFilename.c_str(),true);
      }
    catch(...)
      {
      return typename DeformationFieldType::Pointer();
      }

    return RegisterLandmarksToDeformationField
      <DeformationFieldType,ImageType>(InputImage,
                                       InputLandmarkFilename,
                                       TemplateLandmarkFilename,
                                       TemplateImage);
  }

  /** Common code for allocating an image, allowing the region and spacing to be
   * explicitly set.
   */
  template <class TemplateImageType, class OutputImageType>
  typename OutputImageType::Pointer 
  AllocateImageFromRegionAndSpacing(const typename TemplateImageType::RegionType &region,
                                    const typename TemplateImageType::SpacingType &spacing)
  {
    typename OutputImageType::Pointer rval;
    rval = OutputImageType::New();
    rval->SetSpacing(spacing);
    //    rval->SetLargestPossibleRegion(region);
    //    rval->SetBufferedRegion(region);
    rval->SetRegions(region);
    rval->Allocate();
    return rval;
  }

  template <class ImageType>
  typename ImageType::Pointer
  AllocateImageFromRegionAndSpacing(const typename ImageType::RegionType &region,
                                    const typename ImageType::SpacingType &spacing)
  {
    return AllocateImageFromRegionAndSpacing<ImageType,ImageType>
      (region,spacing);
  }
  /** AllocateImageFromExample creates and allocates an image of the type OutputImageType,
   * using TemplateImageType as the source of size and spacing...
   *
   */
  template <class TemplateImageType, class OutputImageType>
  typename OutputImageType::Pointer 
  AllocateImageFromExample(const typename TemplateImageType::Pointer &TemplateImage)
  {
    typename OutputImageType::Pointer rval =
      AllocateImageFromRegionAndSpacing<TemplateImageType,OutputImageType>
      (TemplateImage->GetLargestPossibleRegion(),
       TemplateImage->GetSpacing());
    rval->SetDirection(TemplateImage->GetDirection());
    rval->SetOrigin(TemplateImage->GetOrigin());
    return rval;
  }
  //
  // convenience function where template and output images type are the same
  template <class ImageType>
  typename ImageType::Pointer
  AllocateImageFromExample(const typename ImageType::Pointer &TemplateImage)
  {
    return AllocateImageFromExample<ImageType,ImageType>(TemplateImage);
  }

  template <class ScalarType,class VectorType>
  typename ScalarType::Pointer 
  VectorImageToScalarImage(const typename VectorType::Pointer &input, 
                           unsigned index)
  {
    typedef typename itk::ImageRegionIterator<ScalarType> ScalarIterator;
    typedef typename itk::ImageRegionConstIterator<VectorType> VectorIterator;

    typename ScalarType::Pointer scalarImage = 
      AllocateImageFromExample<VectorType,ScalarType> (input);

    ScalarIterator scalarIt(scalarImage,scalarImage->GetLargestPossibleRegion());
    VectorIterator vectorIt(input,input->GetLargestPossibleRegion());

    for(scalarIt.Begin(),vectorIt.Begin(); !vectorIt.IsAtEnd(); ++scalarIt, ++vectorIt)
      {
      scalarIt.Set(vectorIt.Value()[index]);
      }

    return scalarImage;
  }

  template <class ScalarType,class VectorType>
  typename VectorType::Pointer 
  ScalarImagesToVectorImage(const typename ScalarType::Pointer &input1,
                            const typename ScalarType::Pointer &input2,
                            const typename ScalarType::Pointer &input3)
  {
    typedef typename itk::ImageRegionConstIterator<ScalarType> ScalarIterator;
    typedef typename itk::ImageRegionIterator<VectorType> VectorIterator;

    typename VectorType::Pointer vectorImage = 
      AllocateImageFromExample<ScalarType,VectorType> (input1);
    
    ScalarIterator scalarIt1(input1,input1->GetLargestPossibleRegion());
    ScalarIterator scalarIt2(input2,input2->GetLargestPossibleRegion());
    ScalarIterator scalarIt3(input3,input3->GetLargestPossibleRegion());
    VectorIterator vectorIt(vectorImage,vectorImage->GetLargestPossibleRegion());

    for(scalarIt1.Begin(),
          scalarIt2.Begin(),
          scalarIt3.Begin(),
          vectorIt.Begin(); 
        !vectorIt.IsAtEnd(); 
        ++scalarIt1,
          ++scalarIt2,
          ++scalarIt3,
          ++vectorIt)
      {
      typename VectorType::PixelType pixel;
      pixel[0] = scalarIt1.Value();
      pixel[1] = scalarIt2.Value();
      pixel[2] = scalarIt3.Value();
      vectorIt.Set(pixel);
      }

    return vectorImage;
  }

  inline
  DeformationFieldType::Pointer 
  LinearRescaleImage(const DeformationFieldType::Pointer &input,
                     int newnx , int newny , int newnz )
  {
    FloatImage3DType::Pointer rescaled[3];
    for(unsigned i = 0; i < 3; i++)
      {
      FloatImage3DType::Pointer tmp = VectorImageToScalarImage<FloatImage3DType,DeformationFieldType>(input,i);
      rescaled[i] = LinearRescaleImage<FloatImage3DType>(tmp,newnx,newny,newnz);
      }
    return itkUtil::ScalarImagesToVectorImage<FloatImage3DType,DeformationFieldType>(rescaled[0],
                                                                                     rescaled[1],
                                                                                     rescaled[2]);
  }

}

#endif


