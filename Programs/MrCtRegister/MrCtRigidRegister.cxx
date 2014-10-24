/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: MrCtRigidRegister.cxx,v $
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
#include <metaCommand.h>
#include <itkImageFileReader.h>


#include "iamTransformIO.h"
#include "iamRegisterVersorRigidFilter.h"


int main (int argc, char **argv)
{
  MetaCommand command;

  command.SetOption("FixedImageFilename","f",false,"Fixed Image Filename");
  command.AddOptionField("FixedImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("MovingImageFilename","m",false,"Moving Image Filename");
  command.AddOptionField("MovingImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputFilename","o",false,"Output Transform");
  command.AddOptionField("OutputFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("iterations","i",false,"# of iterations [1000]");
  command.AddOptionField("iterations","iterations",MetaCommand::INT,false,"1000");
  
  command.SetOption("samples","s",false,"# of samples for MI computation [100000]");
  command.AddOptionField("samples","samples",MetaCommand::INT,false,"100000");
  
  command.SetOption("relaxation","r",false,"Relaxation Factor [0.5]");
  command.AddOptionField("relaxation","factor",MetaCommand::FLOAT,false,"0.5");
    
  command.SetOption("max","max",false,"Maximum step size [0.2]");
  command.AddOptionField("max","max",MetaCommand::FLOAT,false,"0.2");
  
  command.SetOption("min","min",false,"Minimum step size [0.0001]");
  command.AddOptionField("min","min",MetaCommand::FLOAT,false,"0.0001");
  
  command.SetOption("translate","ts",false,"Translation scale [1000]");
  command.AddOptionField("translate","scale",MetaCommand::INT,false,"1000");
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  char FixedImageFilename[512];
  char OutputFilename[512];
  char MovingImageFilename[512];
  strcpy(FixedImageFilename,command.GetValueAsString("FixedImageFilename","filename").c_str());
  strcpy(MovingImageFilename,command.GetValueAsString("MovingImageFilename","filename").c_str());
  strcpy(OutputFilename,command.GetValueAsString("OutputFilename","filename").c_str());
    
      
  float TranslationScale = command.GetValueAsFloat("translate","scale");
  float MaximumStepLength = command.GetValueAsFloat("max","max");
  float MinimumStepLength = command.GetValueAsFloat("min","min");
  float RelaxationFactor = command.GetValueAsFloat("relaxation","factor");
  int NumberOfIterations = command.GetValueAsInt("iterations","iterations");
  int NumberOfSpatialSamples = command.GetValueAsInt("samples","samples");
  
  std::cout << "Fixed Image: " <<  FixedImageFilename << std::endl; 
  std::cout << "Moving Image: " <<  MovingImageFilename << std::endl; 
  std::cout << "Output File: " <<  OutputFilename << std::endl; 
  std::cout << "Translation Scale: " << TranslationScale <<std::endl;
  std::cout << "Maximum Step Length: " << MaximumStepLength <<std::endl;
  std::cout << "Minimum Step Length: " << MinimumStepLength <<std::endl;
  std::cout << "Relaxation Factor: " << RelaxationFactor <<std::endl;
  std::cout << "Iterations: " << NumberOfIterations <<std::endl;
  std::cout << "Samples: " << NumberOfSpatialSamples <<std::endl;  
  //exit(1);
  

  
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


  typedef itk::RegisterVersorRigidFilter RegisterFilterType;
  RegisterFilterType::Pointer registerImageFilter = RegisterFilterType::New();

  registerImageFilter->SetTranslationScale( TranslationScale );
  registerImageFilter->SetMaximumStepLength( MaximumStepLength );
  registerImageFilter->SetMinimumStepLength( MinimumStepLength );
  registerImageFilter->SetRelaxationFactor( RelaxationFactor );
  registerImageFilter->SetNumberOfIterations( NumberOfIterations );
  registerImageFilter->SetNumberOfSpatialSamples( NumberOfSpatialSamples );
  registerImageFilter->SetFixedImage( fixedImageReader->GetOutput() );
  registerImageFilter->SetMovingImage( movingImageReader->GetOutput() );
  registerImageFilter->Update( );
  
  
  
  itk::iamTransformIO::Pointer xfrmIO =  itk::iamTransformIO::New();
  xfrmIO->SetFileName( OutputFilename );
  xfrmIO->SetRigidTransform( registerImageFilter->GetOutput() );
  try 
	  {			
	  xfrmIO->SaveTransform( 0 );  
	  }
  catch (itk::ExceptionObject &ex)
	  {
	  std::cout << ex << std::endl;
	  throw;
	  }
	
    
       
}    
