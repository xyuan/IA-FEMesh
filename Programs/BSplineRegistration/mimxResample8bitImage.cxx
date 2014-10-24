/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: mimxResample8bitImage.cxx,v $
Language:  C++
Date:      $Date: 2007/06/17 21:11:13 $
Version:   $Revision: 1.1 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <fstream>

#include <itkImage.h>
#include <metaCommand.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineDeformableTransform.h>
#include <itkTransformFactory.h>
#include <itkVersorRigid3DTransform.h>
#include <itkOrientImageFilter.h>
#include <itkTransformFileWriter.h>
#include <itkTransformFileReader.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>


int main (int argc, char **argv)
{
  typedef double BSplineCoordinateRepType;
  typedef itk::VersorRigid3DTransform< double >     RigidTransformType;
  typedef itk::BSplineDeformableTransform<
                            BSplineCoordinateRepType,
                            3,
                            3 >     BSplineTransformType;
  itk::TransformFactory<BSplineTransformType>::RegisterTransform();
  itk::TransformFactory<RigidTransformType>::RegisterTransform();
  
  MetaCommand command;

  command.SetOption("MovingImageFilename","m",false,"Moving Image Filename");
  command.AddOptionField("MovingImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("FixedImageFilename","f",false,"Fixed Image Filename");
  command.AddOptionField("FixedImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputImageFilename","o",false,"Output Image Filename");
  command.AddOptionField("OutputImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("InputTransformFilename","x",false,"Input Transform Filename");
  command.AddOptionField("InputTransformFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("type","t",false,"Transform type: 1=Rigid, 2=B-Spline");
  command.AddOptionField("type","option",MetaCommand::INT,false,"1");

  command.SetOption("reorient","r",false,"Reorient anatomical image [0=Off, 1=On]");
  command.AddOptionField("reorient","option",MetaCommand::INT,false,"0");
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    

  std::string movingImageFilename = command.GetValueAsString("MovingImageFilename","filename").c_str();
  std::string fixedImageFilename = command.GetValueAsString("FixedImageFilename","filename").c_str();
  std::string transformFilename = command.GetValueAsString("InputTransformFilename","filename").c_str();
  std::string outputImageFilename = command.GetValueAsString("OutputImageFilename","filename").c_str();
  
  int type = command.GetValueAsInt("type","option");
  int orientAnatomicalImage = command.GetValueAsInt("reorient","option");


  std::cout << "Moving Image: " <<  movingImageFilename << std::endl; 
  std::cout << "Output Image: " <<  outputImageFilename << std::endl; 
  std::cout << "Fixed File: " <<  fixedImageFilename << std::endl; 
  std::cout << "Transform File: " << transformFilename << std::endl;
  std::cout << "Type: " << type << std::endl;
  std::cout << "Reorient Anatomical Image: " << orientAnatomicalImage << std::endl; 

  
  typedef itk::Image<unsigned char, 3> ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;

  /************* Read the Moving Image *************/
  ImageReaderType::Pointer movingImageReader =  ImageReaderType::New();
  movingImageReader->SetFileName( movingImageFilename.c_str() );
  try 
    {     
    movingImageReader->Update();  
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    throw;
    }

  typedef itk::OrientImageFilter< ImageType, ImageType>  OrientFilterType;
  ImageType::Pointer movingImage;
  if ( orientAnatomicalImage )
    {
    OrientFilterType::Pointer orientImageFilter = OrientFilterType::New();
    orientImageFilter->SetInput( movingImageReader->GetOutput() );
    orientImageFilter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    orientImageFilter->UseImageDirectionOn();
    orientImageFilter->Update();
    movingImage = orientImageFilter->GetOutput();
    ImageType::PointType  fixedOrigin  = movingImage->GetOrigin();
    fixedOrigin.Fill(0);
    movingImage->SetOrigin( fixedOrigin );
    }
  else
    {
    movingImage = movingImageReader->GetOutput();
    }


  /************* Read the Fixed Image *************/
  ImageReaderType::Pointer fixedImageReader =  ImageReaderType::New();
  fixedImageReader->SetFileName( fixedImageFilename.c_str() );
  try 
    {     
    fixedImageReader->Update();  
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    throw;
    }

  ImageType::Pointer fixedImage;
  if ( orientAnatomicalImage )
    {
    OrientFilterType::Pointer orientImageFilter = OrientFilterType::New();
    orientImageFilter->SetInput( fixedImageReader->GetOutput() );
    orientImageFilter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    orientImageFilter->UseImageDirectionOn();
    orientImageFilter->Update();
    fixedImage = orientImageFilter->GetOutput();
    ImageType::PointType  fixedOrigin  = fixedImage->GetOrigin();
    fixedOrigin.Fill(0);
    fixedImage->SetOrigin( fixedOrigin );
    }
  else
    {
    fixedImage = fixedImageReader->GetOutput();
    }


  /************* Read the Transform *************/
  typedef itk::TransformFileReader  TransformReaderType;
  TransformReaderType::Pointer      transformReader =  TransformReaderType::New();
  transformReader->SetFileName( transformFilename.c_str() );
  std::cout << "Reading ITK transform file: " << transformFilename << " ..." << std::endl;
  try
    {
    transformReader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Failed to load Transform: " << std::endl;
    std::cerr << err << std::endl;
    throw;
    }

  std::cout << "Read ITK transform file: " << transformFilename << std::endl;

  static const unsigned int SpaceDimension = 3;
  static const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;
  typedef itk::BSplineDeformableTransform<
               CoordinateRepType,
               SpaceDimension,
               SplineOrder >              BsplineTransformType;
  typedef itk::VersorRigid3DTransform< CoordinateRepType >     RigidTransformType;

  RigidTransformType::Pointer rigidTransform = RigidTransformType::New();
  BsplineTransformType::Pointer bslineTransform = BsplineTransformType::New();

  std::string readTransformType = (transformReader->GetTransformList()->back())->GetTransformTypeAsString();

  if ( ( strcmp(readTransformType.c_str(),"VersorRigid3DTransform_double_3_3") == 0) && ( type == 1 ) )
    {
    // Load Versor Rigid Transform
    rigidTransform->SetIdentity();
    rigidTransform->SetParameters(
                (*transformReader->GetTransformList()->begin())->GetParameters() );
    rigidTransform->SetFixedParameters(
                (*transformReader->GetTransformList()->begin())->GetFixedParameters() );
            
    }
  else if ( ( strcmp(readTransformType.c_str(),"BSplineDeformableTransform_double_3_3") == 0) && 
            ( type == 2 ) )
    {
    bslineTransform->SetFixedParameters(
                (transformReader->GetTransformList()->back())->GetFixedParameters() );
    bslineTransform->SetParametersByValue(
                (transformReader->GetTransformList()->back())->GetParameters() );
    std::string initTransformType = (*transformReader->GetTransformList()->begin())->GetTransformTypeAsString();
    if ( strcmp(initTransformType.c_str(),"VersorRigid3DTransform_double_3_3") == 0)
      {
      rigidTransform->SetIdentity();
      rigidTransform->SetParameters(
                    (*transformReader->GetTransformList()->begin())->GetParameters() );
      rigidTransform->SetFixedParameters(
                    (*transformReader->GetTransformList()->begin())->GetFixedParameters() );
      bslineTransform->SetBulkTransform( rigidTransform );
      }
    else
      {
      std::cout << "Error: Invalid Bulk transform..." << std::endl;
      std::cout << "... Versor Rigid required" << std::endl;
      return (1);
      }
    }
  else
    {
    std::cout << "Error: Invalid transform specified..." << std::endl;
    if (type == 1)
      {
      std::cout << "... Versor Rigid Transform required" << std::endl;
      }
    else if (type == 2)
      {
      std::cout << "... B-Spline Transform required" << std::endl;
      }
    else
      {
      std::cout << "... Invalid transform type" << std::endl;
      }
    return (1);
    }
           

	
  typedef itk::ResampleImageFilter< ImageType, ImageType >    ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  switch (type)
    {
	  case 1:
		  resample->SetTransform( rigidTransform );
		  break;
	  case 2:
		  resample->SetTransform( bslineTransform );
		  break;
    }

  resample->SetInput( movingImage );
  resample->SetUseReferenceImage ( true );
  resample->SetReferenceImage( fixedImage );
  resample->SetDefaultPixelValue( 0 );  
  try
    {
    resample->Update();
    }
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    throw;
    } 
	
  typedef itk::ImageFileWriter<ImageType> FileWriterType;
  FileWriterType::Pointer ImageWriter =  FileWriterType::New();
  ImageWriter->SetFileName( outputImageFilename.c_str() );
  ImageWriter->SetInput( resample->GetOutput( ) );     
  try 
	  {			
	  ImageWriter->Update();
	  }
  catch (itk::ExceptionObject &ex)
	  {
	  std::cout << ex << std::endl;
	  throw;
	  }
	 
  return 0;
}
