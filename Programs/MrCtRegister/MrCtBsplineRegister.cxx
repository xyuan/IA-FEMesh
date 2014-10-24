/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: MrCtBsplineRegister.cxx,v $
Language:  C++
Date:      $Date: 2005/12/22 04:27:01 $
Version:   $Revision: 1.2 $

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
#include "iamRegisterBSplineFilter.h"




int main (int argc, char **argv)
{
  MetaCommand command;

  command.SetOption("FixedImageFilename","f",false,"Fixed Image");
  command.AddOptionField("FixedImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("MovingImageFilename","m",false,"Moving Image");
  command.AddOptionField("MovingImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("InputTransform","i",false,"Input Starting Rigid Transform");
  command.AddOptionField("InputTransform","filename",MetaCommand::STRING,false,"");
  
  command.SetOption("OutputFilename","o",false,"Output Transform");
  command.AddOptionField("OutputFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("iterations","n",false,"# of iterations [1000]");
  command.AddOptionField("iterations","iterations",MetaCommand::INT,false,"1000");
  
  command.SetOption("grid","g",false,"Grid Size [12]");
  command.AddOptionField("grid","size",MetaCommand::INT,false,"12");
  
  command.SetOption("border","b",false,"Border Size [3]");
  command.AddOptionField("border","size",MetaCommand::INT,false,"3");
  
  command.SetOption("corrections","c",false,"Number of Corrections [12]");
  command.AddOptionField("corrections","number",MetaCommand::INT,false,"12");
  
  command.SetOption("evaluations","e",false,"Number of Evaluations [500]");
  command.AddOptionField("evaluations","number",MetaCommand::INT,false,"500");
    
  command.SetOption("histogram","h",false,"Number of Histogram Bins [50]");
  command.AddOptionField("histogram","bins",MetaCommand::INT,false,"50");
  
  command.SetOption("scale","s",false,"Spatial Sample Scale [100]");
  command.AddOptionField("scale","scale",MetaCommand::INT,false,"100");
  
  command.SetOption("convergence","cf",false,"Convergence Factor [10000000]");
  command.AddOptionField("convergence","factor",MetaCommand::INT,false,"10000000");
  
  command.SetOption("tolerance","gt",false,"Gradient Tolerance [0.0001]");
  command.AddOptionField("tolerance","tolerance",MetaCommand::INT,false,"0.0001");
  
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  char FixedImageFilename[512];
  char OutputFilename[512];
  char BulkTransformFilename[512];
  char MovingImageFilename[512];
  strcpy(FixedImageFilename,command.GetValueAsString("FixedImageFilename","filename").c_str());
  strcpy(MovingImageFilename,command.GetValueAsString("MovingImageFilename","filename").c_str());
  strcpy(OutputFilename,command.GetValueAsString("OutputFilename","filename").c_str());
  strcpy(BulkTransformFilename,command.GetValueAsString("InputTransform","filename").c_str());  
      
  int Iterations = command.GetValueAsInt("iterations","iterations");
  int GridSize = command.GetValueAsInt("grid","size");
  int BorderSize = command.GetValueAsInt("border","size");
  int NumberOfCorrections = command.GetValueAsInt("corrections","number");
  int NumberOfEvaluations = command.GetValueAsInt("evaluations","number");
  int HistogramBins = command.GetValueAsInt("histogram","bins");  
  int SpatialScale = command.GetValueAsInt("scale","scale");  
  float Convergence = command.GetValueAsFloat("convergence","factor");
  float Tolerance = command.GetValueAsFloat("tolerance","tolerance");
  
  std::cout << "Fixed Image: " <<  FixedImageFilename << std::endl; 
  std::cout << "Moving Image: " <<  MovingImageFilename << std::endl; 
  std::cout << "Output File: " <<  OutputFilename << std::endl; 
  std::cout << "Bulk Transform: " <<  BulkTransformFilename << std::endl; 
  std::cout << "Grid Size: " << GridSize <<std::endl;
  std::cout << "Border Size: " << BorderSize <<std::endl;
  std::cout << "Corrections: " << NumberOfCorrections <<std::endl;
  std::cout << "Evaluations: " << NumberOfEvaluations <<std::endl;
  std::cout << "Histogram: " << HistogramBins <<std::endl;
  std::cout << "Scale: " << SpatialScale <<std::endl;  
  std::cout << "Convergence: " << Convergence <<std::endl;  
  std::cout << "Tolerance: " << Tolerance <<std::endl;  
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
    
  typedef itk::RegisterBSplineFilter RegisterFilterType;
  RegisterFilterType::Pointer registerImageFilter = RegisterFilterType::New();
  
  registerImageFilter->SetSpatialSampleScale( SpatialScale );
  registerImageFilter->SetMaximumNumberOfIterations( Iterations ); 
  registerImageFilter->SetMaximumNumberOfEvaluations( NumberOfEvaluations );
  registerImageFilter->SetMaximumNumberOfCorrections( NumberOfCorrections );
  registerImageFilter->SetBSplineHistogramBins( HistogramBins );
  registerImageFilter->SetGridSize( GridSize);
  registerImageFilter->SetGridBorderSize( BorderSize );
  registerImageFilter->SetCostFunctionConvergenceFactor( Convergence );
  registerImageFilter->SetProjectedGradientTolerance( Tolerance );
  
  if ( strlen(BulkTransformFilename) > 0)
	  {
	  itk::iamTransformIO::Pointer xfrmReader =  itk::iamTransformIO::New();
	  xfrmReader->SetFileName( BulkTransformFilename );
	  try 
	    {			
	    xfrmReader->LoadTransform( );
      }
	  catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      throw;
      }
	  registerImageFilter->SetBulkTransform(  xfrmReader->GetRigidTransform( ) );
	  }
  
  registerImageFilter->SetFixedImage( fixedImageReader->GetOutput() );
  registerImageFilter->SetMovingImage( movingImageReader->GetOutput() );
  registerImageFilter->Update( );
  	      
 
 
  itk::iamTransformIO::Pointer xfrmWriter =  itk::iamTransformIO::New();
  xfrmWriter->SetFileName( OutputFilename );
  xfrmWriter->SetBSplineTransform( registerImageFilter->GetOutput( ) );   
  try 
	  {
	  xfrmWriter->SaveTransform( 1 );
	  } 
  catch (itk::ExceptionObject &ex)
	  {
	  std::cout << ex << std::endl;
	  throw;
	  }
    
       
}    
