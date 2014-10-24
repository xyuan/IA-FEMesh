/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: iaFemResampleImage.cxx,v $
Language:  C++
Date:      $Date: 2005/12/22 02:12:25 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <iostream>
#include <fstream>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <metaCommand.h>
#include <itkResampleImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkBSplineDeformableTransform.h>
#include <itkTransformFactory.h>
#include <itkVersorRigid3DTransform.h>

#include "iamTransformIO.h"


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

  command.SetOption("FixedImageFilename","f",false,"Fixed Image Filename");
  command.AddOptionField("FixedImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("MovingImageFilename","m",false,"Moving Image Filename");
  command.AddOptionField("MovingImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputImageFilename","o",false,"Output Image Filename");
  command.AddOptionField("OutputImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("InputTransform","x",false,"Input Transform");
  command.AddOptionField("InputTransform","filename",MetaCommand::STRING,true);
  
  command.SetOption("type","t",false,"Transform type: 1=Rigid, 2=B-Spline");
  command.AddOptionField("type","option",MetaCommand::INT,false,"1");
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
  char FixedImageFilename[512];
  char MovingImageFilename[512];
  char TransformFilename[512];
  char OutputImageFilename[512];
  strcpy(FixedImageFilename,command.GetValueAsString("FixedImageFilename","filename").c_str());
  strcpy(MovingImageFilename,command.GetValueAsString("MovingImageFilename","filename").c_str());
  strcpy(TransformFilename,command.GetValueAsString("InputTransform","filename").c_str());
  strcpy(OutputImageFilename,command.GetValueAsString("OutputImageFilename","filename").c_str());
  
  int type = command.GetValueAsInt("type","option");
  
  std::cout << "Fixed Image: " <<  FixedImageFilename << std::endl; 
  std::cout << "Moving Image: " <<  MovingImageFilename << std::endl; 
  std::cout << "Output Image: " <<  OutputImageFilename << std::endl; 
  std::cout << "Transform File: " << TransformFilename <<std::endl;
  std::cout << "Type: " << type <<std::endl;
   
   
  typedef signed short                      PixelType;
  typedef itk::Image<PixelType,3>	          ImageType;
  
    
  typedef itk::ImageFileReader<ImageType> FileReaderType;
	FileReaderType::Pointer fixedImageReader = FileReaderType::New();
	fixedImageReader->SetFileName( FixedImageFilename );
  try 
		{			
    fixedImageReader->Update();
		}
	catch (itk::ExceptionObject &ex)
		{
		std::cout << ex << std::endl;
		throw;
		}

	FileReaderType::Pointer movingImageReader = FileReaderType::New();
	movingImageReader->SetFileName( MovingImageFilename );
  try 
		{			
    movingImageReader->Update();
		}
	catch (itk::ExceptionObject &ex)
		{
		std::cout << ex << std::endl;
		throw;
		}
  
	
  itk::iamTransformIO::Pointer xfrmReader =  itk::iamTransformIO::New();	
  xfrmReader->SetFileName( TransformFilename );
  try 
    {			
    xfrmReader->LoadTransform( );
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    throw;
    }
	
	
  
  typedef itk::ResampleImageFilter< ImageType, ImageType >    ResampleFilterType;
  
  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  switch (type)
    {
    case 1:
      resample->SetTransform( xfrmReader->GetRigidTransform() );
      break;
    case 2:
      resample->SetTransform( xfrmReader->GetBSplineTransform() );
      break;
    }
  
  resample->SetInput( movingImageReader->GetOutput() );
  resample->SetSize( fixedImageReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImageReader->GetOutput()->GetOrigin() );
  resample->SetOutputSpacing( fixedImageReader->GetOutput()->GetSpacing() );
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
	
  ImageType::Pointer resampledImage = resample->GetOutput();
  resampledImage->SetMetaDataDictionary( fixedImageReader->GetOutput()->GetMetaDataDictionary() );
  
  
  typedef itk::ImageFileWriter<ImageType> FileWriterType;
	FileWriterType::Pointer outputImageWriter = FileWriterType::New();
	outputImageWriter->SetFileName( OutputImageFilename );
  outputImageWriter->SetInput( resampledImage );
  try 
		{			
    outputImageWriter->Update();
		}
	catch (itk::ExceptionObject &ex)
		{
		std::cout << ex << std::endl;
		throw;
		}

	
}
