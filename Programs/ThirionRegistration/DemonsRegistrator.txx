
#ifndef _DemonsRegistrator_txx
#define _DemonsRegistrator_txx

#include "DemonsRegistrator.h"
#include "itkCommand.h"
#include "ApplyField.h"
#include "itkStatisticsImageFilter.h"
#include "itkMetaImageIO.h"
#include "itkMetaDataObject.h"
#include "itkIOCommon.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "vector"
#include "itkCheckerBoardImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkIO.h"
#include "itkTransformFileReader.h"
#include "itkVersorRigid3DTransform.h"
#include "itkThinPlateR2LogRSplineKernelTransform.h"
#include "itkImage.h"


namespace itk
{
  /*This function writes the displacement fields of the Deformation.*/
  template < typename TRealImage, typename TOutputImage,
  typename TFieldValue > void DemonsRegistrator < TRealImage, TOutputImage,
  TFieldValue >::WriteDisplacementComponents ()
    {
    typedef itk::Image < float, 3 > OutputImageType;

    m_DefaultPixelValue = NumericTraits<PixelType>::One;

    OutputImageType::Pointer Disp[3];
    Disp[0] = OutputImageType::New ();
    Disp[1] = OutputImageType::New ();
    Disp[2] = OutputImageType::New ();

    //we use the vector index selection filter to break the deformation field into x,y,z components.
    typedef itk::VectorIndexSelectionCastImageFilter < DeformationFieldType,
    itk::Image < FieldValueType, 3 > >ComponentFilterType;

    std::string CurrentComponentFilename;
    try
      {
      char ext[3][14] = { "_xdisp.nii.gz", "_ydisp.nii.gz", "_zdisp.nii.gz" };

      typename ComponentFilterType::Pointer myComponentFilter =
        ComponentFilterType::New ();
      myComponentFilter->SetInput (m_DeformationField);

      for (unsigned int extiter = 0; extiter < 3; extiter++)
        {
        CurrentComponentFilename = m_DisplacementBaseName + ext[extiter];
        std::
          cout << "Writing Transform Image: " << CurrentComponentFilename <<
          std::endl;

        myComponentFilter->SetIndex (extiter);

        typename OutputImageType::Pointer DisplacementComponentImagePtr =
          myComponentFilter->GetOutput ();

        itkUtil::WriteImage<OutputImageType>(DisplacementComponentImagePtr,CurrentComponentFilename);
#if 0
        typedef itk::ImageFileWriter < OutputImageType > FileWriterType;
        FileWriterType::Pointer DisplacementImageWriter =
          FileWriterType::New ();
        DisplacementImageWriter->SetInput (DisplacementComponentImagePtr);
        DisplacementImageWriter->SetFileName (CurrentComponentFilename.c_str ());
        DisplacementImageWriter->Update ();
#endif
        }
      }
    catch (itk::ExceptionObject & e)
      {
      std::cerr << "exception in file Displacement File Writer(" <<
        CurrentComponentFilename << ")" << std::endl;
      std::cerr << e.GetDescription () << std::endl;
      std::cerr << e.GetLocation () << std::endl;
      exit (-1);
      }
    }

  /*Constructor to initialize the parameters.*/
  template < typename TRealImage, typename TOutputImage,
  typename TFieldValue > DemonsRegistrator < TRealImage, TOutputImage,
  TFieldValue >::DemonsRegistrator ()
    {
    // Images need to be set from the outside
    m_FixedImage = NULL;
    m_MovingImage = NULL;
    m_DeformationField = NULL;

    // Set up internal registrator with default components
    m_FixedImagePyramid = FixedImagePyramidType::New ();
    m_MovingImagePyramid = MovingImagePyramidType::New ();
    m_Registration = RegistrationType::New ();

    m_Registration->SetFixedImagePyramid (m_FixedImagePyramid);
    m_Registration->SetMovingImagePyramid (m_MovingImagePyramid);

    m_DefaultPixelValue =  NumericTraits<typename RealImageType::PixelType>::Zero;
    // Setup an registration observer
    typedef SimpleMemberCommand < Self > CommandType;
    typename CommandType::Pointer command = CommandType::New ();
    command->SetCallbackFunction (this, &Self::StartNewLevel);

    m_Tag = m_Registration->AddObserver (IterationEvent (), command);


    // Default parameters
    m_NumberOfLevels = 1;

    m_FixedImageShrinkFactors.Fill (1);
    m_MovingImageShrinkFactors.Fill (1);

    m_NumberOfIterations = UnsignedIntArray (1);
    m_NumberOfIterations.Fill (10);
    m_WarpedImageName = "none";
    m_DisplacementBaseName = "none";
    m_CheckerBoardFileName = "none";
    m_DeformationFieldOutputName = "none";
    m_CheckerBoardPattern.Fill( 4 );
    m_OutNormalized  = "OFF";
    m_OutDebug  = "OFF";
    }


  template < typename TRealImage, typename TOutputImage,
  typename TFieldValue > DemonsRegistrator < TRealImage, TOutputImage,
  TFieldValue >::~DemonsRegistrator ()
    {

    if (m_Tag)
      {
      m_Registration->RemoveObserver (m_Tag);
      }

    }

  /*Perform the registration of preprocessed images.*/
  template < typename TRealImage, typename TOutputImage,
  typename TFieldValue > void DemonsRegistrator < TRealImage, TOutputImage,
  TFieldValue >::Execute ()
    {

    // Setup the image pyramids
    m_FixedImagePyramid->SetNumberOfLevels (m_NumberOfLevels);
    m_FixedImagePyramid->SetStartingShrinkFactors (m_FixedImageShrinkFactors.
      GetDataPointer ());

    m_MovingImagePyramid->SetNumberOfLevels (m_NumberOfLevels);
    m_MovingImagePyramid->
      SetStartingShrinkFactors (m_MovingImageShrinkFactors.GetDataPointer ());

    // Setup the registrator
    m_Registration->SetFixedImage (m_FixedImage);
    m_Registration->SetMovingImage (m_MovingImage);
    m_Registration->SetNumberOfLevels (m_NumberOfLevels);
    m_Registration->SetNumberOfIterations (m_NumberOfIterations.
      data_block ());

    // Setup the initial deformation field
    //     if(this->m_InitialDeformationField.IsNotNull())
    //       {
    //       m_Registration->SetInitialDeformationField(this->m_InitialDeformationField);
    //       }
    if( this->m_InitialTransformFileName != "" )
      {
      std::cout << "Use Initial Transform: " << m_InitialTransformFileName << std::endl;
      /* Load the Transform */
      itk::TransformFileReader::Pointer transformReader =  itk::TransformFileReader::New();
      transformReader->SetFileName( m_InitialTransformFileName.c_str() );
      try 
        {
        transformReader->Update( );  
        }
      catch (itk::ExceptionObject &ex)
        {
        std::cout << ex << std::endl;
        throw;
        }
      typename DeformationFieldType::Pointer IntialDeformation;
      typename RealImageType::RegionType imageRegion = m_FixedImage->GetLargestPossibleRegion();
      typename RealImageType::RegionType::SizeType imageSize = imageRegion.GetSize();
      typename RealImageType::SpacingType imageSpacing = m_FixedImage->GetSpacing();
      typename RealImageType::PointType imageOrigin = m_FixedImage->GetOrigin();

      std::string readTransformType = (transformReader->GetTransformList()->back())->GetTransformTypeAsString();
      if ( strcmp(readTransformType.c_str(),"VersorRigid3DTransform_double_3_3") == 0)
        {
        typedef itk::VersorRigid3DTransform< double >     BulkTransformType;
        BulkTransformType::Pointer bulkTransform = BulkTransformType::New();
        bulkTransform->SetIdentity();
        bulkTransform->SetParameters(
                (*transformReader->GetTransformList()->begin())->GetParameters() );
        bulkTransform->SetFixedParameters(
                (*transformReader->GetTransformList()->begin())->GetFixedParameters() );
        IntialDeformation = TransformToDeformationField<DeformationFieldType,BulkTransformType>
                            (imageSize, imageSpacing, bulkTransform);
        }
      else if ( strcmp(readTransformType.c_str(),"ThinPlateR2LogRSplineKernelTransform_double_3_3") == 0)
        {
        std::cout << "Initialize with  ThinPlateR2LogRSplineKernelTransform!" << std::endl;
        typedef itk::ThinPlateR2LogRSplineKernelTransform<double, 3>  BulkTransformType;
        BulkTransformType::Pointer bulkTransform = BulkTransformType::New();
        bulkTransform->SetParameters(
                (*transformReader->GetTransformList()->begin())->GetParameters() );
        bulkTransform->SetFixedParameters(
                (*transformReader->GetTransformList()->begin())->GetFixedParameters() );
        bulkTransform->ComputeWMatrix( );
        IntialDeformation = TransformToDeformationField<DeformationFieldType,BulkTransformType>
                            (imageSize, imageSpacing, imageOrigin, bulkTransform);
        }
      else
        {
        std::cout << "Error: Invalid Bulk Transform Type! " << std::endl;
        std::cout << "Only the VersorRigid3DTransform_double_3_3 and ThinPlateR2LogRSplineKernelTransform_double_3_3 transforms are currently supported." << std::endl;
        throw;
        }
      std::cout << "Set Initial Field: " << std::endl;
      //std::cout << IntialDeformation << std::endl;
/*     
      itkUtil::WriteImage<DeformationFieldType>(IntialDeformation,"/tmp/LandmarkDef.mhd");
      std::cout << "---Wrote Landmark Deformation field" << "--" << std::endl; 
*/
      m_Registration->SetInitialDeformationField(IntialDeformation);
      //m_Registration->SetInput(IntialDeformation);
      std::cout << "Set Field Complete " << std::endl;
      }
    //Perform the registration.
    try
      {
      std::cout << "Run Registration" << std::endl;
      m_Registration->Update ();
      }
    catch (itk::ExceptionObject & err)
      {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
      throw err;
      throw err;
      }
    catch (...)
      {
      std::
        cout << "Caught a non-ITK exception " << __FILE__ << " " << __LINE__
        << std::endl;
      }
    std::cout << "Moving image shrink factors used in each level of MultiResolution Schedule\n" << m_MovingImagePyramid->GetSchedule() << std::endl;
    std::cout << "Fixed image shrink factors used in each level of MultiResolution Schedule\n" << m_FixedImagePyramid->GetSchedule() << std::endl;

    try
      {
      m_DeformationField = m_Registration->GetOutput ();
      //Get the deformation field produced by the registration.
      //Get the orientation of the fixed image.
      m_DeformationField->SetDirection(m_FixedImage->GetDirection());
      if ( m_DeformationField->GetDirection() != m_FixedImage->GetDirection())
        {
        std::cout << "ERROR Directions don't match\n"
          << m_DeformationField->GetDirection()
          << "\n"
          << m_FixedImage->GetDirection()
          << std::endl;
        exit(-1);
        }
      if (m_Tag)
        {
        m_Registration->RemoveObserver (m_Tag);
        m_Tag = 0;
        }
      m_Registration = NULL;
      }
    catch (itk::ExceptionObject & err)
      {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
      throw err;
      }
    catch (...)
      {
      std::
        cout << "Caught a non-ITK exception " << __FILE__ << " " << __LINE__
        << std::endl;
      }

    //Write the output deformation fields if specified by the user.
    if (this->m_DeformationFieldOutputName != std::string ("none") &&
        this->m_DeformationFieldOutputName != std::string (""))
      {
      itkUtil::WriteImage<DeformationFieldType>(m_DeformationField,this->m_DeformationFieldOutputName);
      std::cout << "---Deformation field has been written " << this->m_DeformationFieldOutputName << "--" << std::endl;
      //m_DeformationField=0;
      }
    typename DeformationFieldType::Pointer DeformationFieldAlias;
    if(this->m_DeformationFieldOutputName != "none" && 
       this->m_DeformationFieldOutputName != "")
      {
        DeformationFieldAlias=
        itkUtil::ReadImage<DeformationFieldType>(this->m_DeformationFieldOutputName,true);
      }

    //  Write out the displacement fields specified by the user.
    if (this->m_DisplacementBaseName != std::string ("none"))
      {
      WriteDisplacementComponents ();
      }


    if (this->m_WarpedImageName != std::string ("none")
      || this->m_CheckerBoardFileName != std::string ("none"))
      {
      /*Warp the image with the generated deformation field.*/
      typename RealImageType::Pointer DeformedMovingImagePtr(0);
      try
        {
        typename itk::ApplyField<DeformationFieldType, RealImageType, RealImageType>::Pointer applyfield =
          itk::ApplyField<DeformationFieldType, RealImageType, RealImageType>::New();

        if (this->m_OutNormalized == std::string ("ON"))
          {
          std::cout<<"---Warping the Normalized template image."<<std::endl;
          applyfield->SetInputImage(m_MovingImage);
          }
        else
          {
          applyfield->SetInputImage(m_UnNormalizedMovingImage);
          }

        std::cout << "---Reading deformation fields " << std::endl;
        std::cout << m_DeformationField /*DeformationFieldAlias*/ << std::endl;
        applyfield->SetDeformationField(m_DeformationField /*DeformationFieldAlias*/);

        std::cout << "Setting Padding Value: " << static_cast<float>(m_DefaultPixelValue) << "." << std::endl;
        applyfield->SetDefaultPixelValue(m_DefaultPixelValue);

        applyfield->Execute();
        std::cout << "Mask Registration Complete." <<std::endl;
        std::cout << "---Warp Done" << std::endl;
        DeformedMovingImagePtr=applyfield->GetOutputImage();
        }
      catch (itk::ExceptionObject & e)
        {
        std::cerr << "Exception in Registration." << std::endl;
        std::cerr << e.GetDescription() << std::endl;
        std::cerr << e.GetLocation() << std::endl;
        exit(-1);
        }
/*
      std::cout << "-----Direction of output warped image\n" << DeformedMovingImagePtr->GetDirection()
        << "\n-----Direction of deformation field\n" << DeformationFieldAlias->GetDirection() <<std::endl;
*/
      /*Write the output image.*/
      if (this->m_WarpedImageName != std::string ("none"))
        {
        typedef itk::CastImageFilter< RealImageType, OutputImageType> CastToRealFilterType;
        typename CastToRealFilterType::Pointer castimage = CastToRealFilterType::New();
        castimage->SetInput(DeformedMovingImagePtr);
        castimage->Update();
        typename OutputImageType::Pointer CastImageSptr = castimage->GetOutput ();

        itkUtil::WriteImage<OutputImageType>(CastImageSptr,this->m_WarpedImageName);
        std::cout << "---Deformed Image has been written" << std::endl;
        }
      /*Write the checkerboard image of the fixed image and the output image.*/
      if (this->m_CheckerBoardFileName != std::string ("none"))
        {
        typedef itk::CheckerBoardImageFilter < RealImageType > Checkerfilter;
        typename Checkerfilter::Pointer checker = Checkerfilter::New ();
        if (this->m_OutNormalized == std::string ("ON"))
          {
          checker->SetInput1 (m_FixedImage);
          }
        else
          {
          checker->SetInput1 (m_UnNormalizedFixedImage);
          }
        checker->SetInput2 (DeformedMovingImagePtr);
        checker->SetCheckerPattern(this->GetCheckerBoardPattern());
        try
          {
          checker->Update ();
          }
        catch (itk::ExceptionObject & err)
          {
          std::cout << "Caught an ITK exception: " << std::endl;
          std::cout << err << " " << __FILE__ << " " << __LINE__ << std::
            endl;
          throw err;
          }

        typename RealImageType::Pointer CheckerImagePtr=checker->GetOutput();
        itkUtil::WriteImage<RealImageType>(CheckerImagePtr, this->m_CheckerBoardFileName);
        std::cout << "---Checker Board Image has been written" << std::endl;
        }
      }
    }

  //Print out the present registration level.
  template < typename TRealImage, typename TOutputImage,
  typename TFieldValue > void DemonsRegistrator < TRealImage, TOutputImage,
  TFieldValue >::StartNewLevel ()
    {
    std::cout << "--- Starting level " << m_Registration->GetCurrentLevel ()
      << std::endl;
    }
}// namespace itk
#endif
