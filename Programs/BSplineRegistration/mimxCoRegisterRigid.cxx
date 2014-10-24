/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: mimxCoRegisterRigid.cxx,v $
Language:  C++
Date:      $Date: 2007/05/22 19:33:18 $
Version:   $Revision: 1.3 $

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
#include <itkOrientImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkVersorRigid3DTransformOptimizer.h>
#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkVersorRigid3DTransform.h>
#include <itkCenteredTransformInitializer.h>
#include <itkTimeProbesCollectorBase.h>
#include <itkTransformFactory.h>
#include <itkTransformFileWriter.h>
#include <itkCommand.h>


//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::VersorRigid3DTransformOptimizer     OptimizerType;
  typedef const OptimizerType   *OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( ! itk::IterationEvent().CheckEvent( &event ) )
        {
        return;
        }
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};
  
/****************************************************************
 Program: mimxCoRegisterRigid
 
 Purpose: Co-register two image datasets using a Rigid Transform

 Testing for Neural Network and EM Segmentation Algorithms
*****************************************************************/


int main (int argc, char **argv)
{
  MetaCommand command;

  command.SetOption("MovingImageFilename","m",false,"Moving Image");
  command.AddOptionField("MovingImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("FixedImageFilename","f",false,"Fixed Image");
  command.AddOptionField("FixedImageFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputTransformFilename","o",false,"Output Transform");
  command.AddOptionField("OutputTransformFilename","filename",MetaCommand::STRING,true);
  
  command.SetOption("iterations","n",false,"# of iterations [1000]");
  command.AddOptionField("iterations","iterations",MetaCommand::INT,false,"1000");
  
  command.SetOption("samples","s",false,"# of samples for MI computation [100000]");
  command.AddOptionField("samples","samples",MetaCommand::INT,false,"100000");
  
  command.SetOption("relax","r",false,"Relaxation Factor [0.5]");
  command.AddOptionField("relax","relaxation-factor",MetaCommand::FLOAT,false,"0.5");
    
  command.SetOption("max","max",false,"Maximum step size [0.2]");
  command.AddOptionField("max","max",MetaCommand::FLOAT,false,"0.2");
  
  command.SetOption("min","min",false,"Minimum step size [0.0001]");
  command.AddOptionField("min","min",MetaCommand::FLOAT,false,"0.0001");
  
  command.SetOption("translate","ts",false,"Translation scale [1000]");
  command.AddOptionField("translate","translation-scale",MetaCommand::INT,false,"1000");

  command.SetOption("reorient","r",false,"Reorient images before registration [0=Off, 1=On]");
  command.AddOptionField("reorient","option",MetaCommand::INT,false,"0");
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  std::string MovingImageFilename = command.GetValueAsString("MovingImageFilename","filename");
  std::string FixedImageFilename = command.GetValueAsString("FixedImageFilename","filename");
  std::string TransformFilename = command.GetValueAsString("OutputTransformFilename","filename");

      
  float TranslationScale = command.GetValueAsFloat("translate","translation-scale");
  float MaximumStepLength = command.GetValueAsFloat("max","max");
  float MinimumStepLength = command.GetValueAsFloat("min","min");
  float RelaxationFactor = command.GetValueAsFloat("relax","relaxation-factor");
  int NumberOfIterations = command.GetValueAsInt("iterations","iterations");
  int NumberOfSpatialSamples = command.GetValueAsInt("samples","samples");
  int OrientAnatomicalImage = command.GetValueAsInt("reorient","option");
  
  std::cout << "Moving Image: " <<  MovingImageFilename << std::endl; 
  std::cout << "Fixed Image: " <<  FixedImageFilename << std::endl; 
  std::cout << "Output Transform: " <<  TransformFilename << std::endl; 
  std::cout << "Translation Scale: " << TranslationScale <<std::endl;
  std::cout << "Maximum Step Length: " << MaximumStepLength <<std::endl;
  std::cout << "Minimum Step Length: " << MinimumStepLength <<std::endl;
  std::cout << "Relaxation Factor: " << RelaxationFactor <<std::endl;
  std::cout << "Iterations: " << NumberOfIterations <<std::endl;
  std::cout << "Samples: " << NumberOfSpatialSamples <<std::endl; 
  std::cout << "Reorient Anatomical Image: " << OrientAnatomicalImage <<std::endl;    
  //exit(1);
  

  typedef itk::Image<signed short, 3> ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;

  /* Read the Moving Image */
  ImageReaderType::Pointer movingImageReader =  ImageReaderType::New();
  movingImageReader->SetFileName( MovingImageFilename.c_str() );
  try 
	  {
	  movingImageReader->Update();
    typedef itk::ImageFileWriter<ImageType> FileWriterType;
    FileWriterType::Pointer ImageWriter =  FileWriterType::New();
    ImageWriter->SetFileName( "MovingImage.nii" );
    ImageWriter->SetInput( movingImageReader->GetOutput( ) );     			
	  ImageWriter->Update();
	  }
  catch (itk::ExceptionObject &ex)
	  {
	  std::cout << ex << std::endl;
	  throw;
	  }

  typedef itk::OrientImageFilter< ImageType, ImageType>  OrientFilterType;
  ImageType::Pointer movingImage;
  if ( OrientAnatomicalImage )
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


  /* Read the Fixed Image */
  ImageReaderType::Pointer fixedImageReader =  ImageReaderType::New();
  fixedImageReader->SetFileName( FixedImageFilename.c_str() );
  try 
    {     
    fixedImageReader->Update(); 
    typedef itk::ImageFileWriter<ImageType> FileWriterType;
    FileWriterType::Pointer ImageWriter =  FileWriterType::New();
    ImageWriter->SetFileName( "FixedImage.nii" );
    ImageWriter->SetInput( fixedImageReader->GetOutput( ) );     			
	  ImageWriter->Update(); 
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    throw;
    }

  ImageType::Pointer fixedImage;
  if ( OrientAnatomicalImage )
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

  /* Now Setup the Registration */
  typedef itk::ImageRegistrationMethod<ImageType,ImageType > RegistrationType;
  RegistrationType::Pointer   registration  = RegistrationType::New();

  typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType > MetricType;
  MetricType::Pointer         metric        = MetricType::New();
  metric->SetNumberOfSpatialSamples( NumberOfSpatialSamples );
  registration->SetMetric(        metric        );

  typedef itk::VersorRigid3DTransformOptimizer      OptimizerType;
  OptimizerType::Pointer         optimizer   = OptimizerType::New();
  registration->SetOptimizer(     optimizer     );

  typedef itk:: LinearInterpolateImageFunction<ImageType,double> InterpolatorType;
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  registration->SetInterpolator(  interpolator  );

  typedef itk::VersorRigid3DTransform< double >     TransformType;
  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  registration->SetFixedImage(   fixedImage   );
  registration->SetMovingImage(   movingImage  );
  registration->SetFixedImageRegion( fixedImage->GetBufferedRegion() );

  typedef itk::CenteredTransformInitializer< TransformType,
            ImageType,
            ImageType
                >  TransformInitializerType;
  TransformInitializerType::Pointer initializer = TransformInitializerType::New();
  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImage );
  initializer->SetMovingImage( movingImage );
  initializer->MomentsOn();
  initializer->InitializeTransform();

  std::cout << "Initializer, center: " << transform->GetCenter()
            << ", offset: " << transform->GetOffset()
            << "." << std::endl;

  typedef TransformType::VersorType  VersorType;
  typedef VersorType::VectorType     VectorType;
  VersorType     rotation;
  VectorType     axis;

  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;

  const double angle = 0;

  rotation.Set(  axis, angle  );
  transform->SetRotation( rotation );
  registration->SetInitialTransformParameters( transform->GetParameters() );

  const double translationScale = 1.0 / TranslationScale;

  typedef OptimizerType::ScalesType  OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = translationScale;
  optimizerScales[4] = translationScale;
  optimizerScales[5] = translationScale;
  optimizer->SetScales( optimizerScales );
  optimizer->SetMaximumStepLength( MaximumStepLength );
  optimizer->SetMinimumStepLength( MinimumStepLength );
  optimizer->SetRelaxationFactor( RelaxationFactor );
  optimizer->SetNumberOfIterations( NumberOfIterations );

  std::cout << "Before Rigid Registration, center: " << transform->GetCenter()
            << ", offset: " << transform->GetOffset()
            << "." << std::endl;

  // Create the Command observer and register it with the registration filter.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );
  
  try
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    }

  OptimizerType::ParametersType finalParameters 
                        = registration->GetLastTransformParameters();


  const double versorX              = finalParameters[0];
  const double versorY              = finalParameters[1];
  const double versorZ              = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];
  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  const double bestValue = optimizer->GetValue();

  //
  // Print out results
  //
  std::cout << std::endl << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " versor X      = " << versorX  << std::endl;
  std::cout << " versor Y      = " << versorY  << std::endl;
  std::cout << " versor Z      = " << versorZ  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Translation Z = " << finalTranslationZ  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;


  transform->SetParameters( finalParameters );

  std::cout << "After Rigid Registration, center: " << transform->GetCenter()
            << ", offset: " << transform->GetOffset()
            << "." << std::endl;


  TransformType::MatrixType matrix = transform->GetRotationMatrix();
  TransformType::OffsetType offset = transform->GetOffset();

  std::cout << "Matrix = " << std::endl << matrix << std::endl;
  std::cout << "Offset = " << std::endl << offset << std::endl;

  
  itk::TransformFileWriter::Pointer transformWriter =  itk::TransformFileWriter::New();
  transformWriter->SetFileName( TransformFilename.c_str() );
  transformWriter->SetInput( transform );
  try 
	  {			
	  transformWriter->Update( );  
	  }
  catch (itk::ExceptionObject &ex)
	  {
	  std::cout << ex << std::endl;
	  throw;
	  }


  exit(0);

}
