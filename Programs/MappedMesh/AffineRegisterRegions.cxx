/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: AffineRegisterRegions.cxx,v $
Language:  C++
Date:      $Date: 2006/09/18 19:25:43 $
Version:   $Revision: 1.2 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
//  Software Guide : BeginCommandLineArgs
#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <math.h>
#include <time.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkIdentityTransform.h>
#include <itkResampleImageFilter.h>
#include <itkAffineTransform.h>
#include <itkMatchCardinalityImageToImageMetric.h>
#include <itkImageRegistrationMethod.h>
#include <itkCenteredTransformInitializer.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkGradientDifferenceImageToImageMetric.h>
#include <itkSingleValuedNonLinearOptimizer.h>
#include <itkCommand.h>
#include <metaCommand.h>


class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::SingleValuedNonLinearOptimizer     OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

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
      //std::cout << optimizer->GetCurrentIteration() << "   ";
      //std::cout << optimizer->GetValue() << "   ";
      //std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};




int main( int argc, char * argv[] )
{
//  itk::Brains2MaskImageIOFactory::RegisterOneFactory( );

  /** Handle the Command line arguments */
  MetaCommand command;

  command.SetOption("CanonicalFileName","c",false,"Input Canonical File Name");
  command.AddOptionField("CanonicalFileName","filename",MetaCommand::STRING,true);

  command.SetOption("SubjectFileName","s",false,"Input Subject File Name");
  command.AddOptionField("SubjectFileName","filename",MetaCommand::STRING,true);

  command.SetOption("OutputImageFileName","o",false,"Output Affine Registered Canonical Image");
  command.AddOptionField("OutputImageFileName","filename",MetaCommand::STRING,false, "");

  command.SetOption("OutputTransformFileName","x",false,"Output Transform Filename");
  command.AddOptionField("OutputTransformFileName","filename",MetaCommand::STRING,false, "");

  command.SetOption("TranslationScale","ts",false,"Translation Scale");
  command.AddOptionField("TranslationScale","value",MetaCommand::FLOAT,false,"0.001");
  
  command.SetOption("StepLength","sl",false,"Step Length");
  command.AddOptionField("StepLength","value",MetaCommand::FLOAT,false,"0.1");

  command.SetOption("MaxIterations","mi",false,"Maximum Number of Iterations");
  command.AddOptionField("MaxIterations","number",MetaCommand::FLOAT,false,"300");


  if ( !command.Parse(argc,argv) )
    {
    return 1;
    }
	
  const std::string CanonicalFileName(command.GetValueAsString("CanonicalFileName",
                                                            "filename"));
  const std::string SubjectFileName(command.GetValueAsString("SubjectFileName",
                                                            "filename"));
  const std::string OutputImageFileName(command.GetValueAsString("OutputImageFileName",
                                                            "filename"));
  const std::string OutputTransformFileName(command.GetValueAsString("OutputTransformFileName",
                                                            "filename"));
  
  float stepLength = command.GetValueAsFloat("TranslationScale","value");
  float translationScale = command.GetValueAsFloat("StepLength","value");
  int maxNumberOfIterations = command.GetValueAsInt("MaxIterations","number");

  const    unsigned int    Dimension = 3;
  typedef    unsigned char   InputPixelType;
  typedef itk::Image<InputPixelType, Dimension>  InputImageType;
  typedef itk::Image<InputPixelType, Dimension>  FixedImageType;
  typedef itk::Image<InputPixelType, Dimension>  MovingImageType;

  /** Read the Input Images */
  typedef itk::ImageFileReader< InputImageType >  ReaderType; 
  ReaderType::Pointer fixedImageReader = ReaderType::New( );
  fixedImageReader->SetFileName( SubjectFileName.c_str( ) );
  fixedImageReader->Update( );

  ReaderType::Pointer movingImageReader = ReaderType::New( );
  movingImageReader->SetFileName( CanonicalFileName.c_str( ) );
  movingImageReader->Update( );

  /* Set up an Affine Registration */
  typedef itk::AffineTransform< 
                                  double, 
                                  Dimension  >     TransformType;


  typedef itk::SingleValuedNonLinearOptimizer       OptimizerType;
  typedef itk::MatchCardinalityImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;
  typedef itk:: NearestNeighborInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;
  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  registration->SetFixedImage(    fixedImageReader->GetOutput()    );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );
  registration->SetFixedImageRegion( 
     fixedImageReader->GetOutput()->GetBufferedRegion() );

  typedef itk::CenteredTransformInitializer< 
                                    TransformType, 
                                    FixedImageType, 
                                    MovingImageType >  TransformInitializerType;
  TransformInitializerType::Pointer initializer = TransformInitializerType::New();
  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImageReader->GetOutput() );
  initializer->SetMovingImage( movingImageReader->GetOutput() );
  initializer->MomentsOn();
  initializer->InitializeTransform();
  
  registration->SetInitialTransformParameters( 
                                 transform->GetParameters() );


  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );

  optimizerScales[0] =  1.0;
  optimizerScales[1] =  1.0;
  optimizerScales[2] =  1.0;
  optimizerScales[3] =  1.0;
  optimizerScales[4] =  1.0;
  optimizerScales[5] =  1.0;
  optimizerScales[6] =  translationScale;
  optimizerScales[7] =  translationScale;
  optimizerScales[8] =  translationScale;

  optimizer->SetScales( optimizerScales );
//  optimizer->SetMaximumStepLength( stepLength ); 
//  optimizer->SetMinimumStepLength( 0.0001 );
//  optimizer->SetNumberOfIterations( maxNumberOfIterations );
//  optimizer->MinimizeOn();

//  metric->SetFixedImageMask( fixedImageReader->GetOutput() );

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
    return -1;
    } 


  InterpolatorType::Pointer imageInterpolator = InterpolatorType::New( );
  imageInterpolator->SetInputImage( movingImageReader->GetOutput() );


  typedef itk::ResampleImageFilter< InputImageType, InputImageType> 
                                    ResampleImageFilterType;
  ResampleImageFilterType::Pointer resampleImageFilter = 
                                    ResampleImageFilterType::New( );
  resampleImageFilter->SetInput( movingImageReader->GetOutput() );
  resampleImageFilter->SetTransform( transform );
  resampleImageFilter->SetInterpolator( imageInterpolator );
  resampleImageFilter->SetOutputSpacing( fixedImageReader->GetOutput()->GetSpacing() );
  resampleImageFilter->SetOutputOrigin( fixedImageReader->GetOutput()->GetOrigin() );
  resampleImageFilter->SetSize( fixedImageReader->GetOutput()->GetLargestPossibleRegion( ).GetSize( ) );
  resampleImageFilter->Update( );

  /** Write the Extracted Image */
  typedef itk::ImageFileWriter< InputImageType >  WriterType; 
  WriterType::Pointer imageWriter = WriterType::New( );
  imageWriter->SetInput( resampleImageFilter->GetOutput( ) );
  imageWriter->SetFileName( OutputImageFileName.c_str( ) );
  imageWriter->Update( );


  exit(1);

/*
  InputImageType::SpacingType spacing = 
      imageReader->GetOutput( )->GetSpacing( );
  InputImageType::SizeType size =
      imageReader->GetOutput( )->GetLargestPossibleRegion( ).GetSize( );
  InputImageType::PointType upperBound = 
      imageReader->GetOutput()->GetOrigin();
  InputImageType::PointType lowerBound = 
      imageReader->GetOutput()->GetOrigin();
  
  for (int i=0;i<Dimension;i++)
    {
    lowerBound[i] += size[i] * spacing[i];
    }


  typedef itk::ImageRegionIteratorWithIndex<InputImageType>  IteratorType;  
  IteratorType it( imageReader->GetOutput(), imageReader->GetOutput()->GetBufferedRegion() );

  while( !it.IsAtEnd() ) 
    {
    if ( it.Get() > 0 )
      {
      InputImageType::PointType indexPointLoc;
      imageReader->GetOutput()->TransformIndexToPhysicalPoint (it.GetIndex(), indexPointLoc);
      for (int i=0;i<Dimension;i++)
        {
        if (lowerBound[i] > indexPointLoc[i]) lowerBound[i] = indexPointLoc[i];
        if (upperBound[i] < indexPointLoc[i]) upperBound[i] = indexPointLoc[i];
        }
      }
    ++it;
    }

  InputImageType::SizeType resampleSize;
  for (int i=0;i<Dimension;i++)
    {
    lowerBound[i] -= pad[i] * spacing[i];
    upperBound[i] += pad[i] * spacing[i];
    resampleSize[i] = static_cast<unsigned int>((upperBound[i] - lowerBound[i]) / spacing[i]);
    }
 */ 
  /*** Extract the Region of Interest ***/
/*
  typedef double  CoordRepType;
  typedef itk::NearestNeighborInterpolateImageFunction< InputImageType,CoordRepType >
                                                          InterpolatorType;

  InterpolatorType::Pointer imageInterpolator = InterpolatorType::New( );
  imageInterpolator->SetInputImage( imageReader->GetOutput() );

  typedef itk::IdentityTransform< double, 3 >  IdentityTransformType;
  IdentityTransformType::Pointer imageIdentityTransform = 
                                    IdentityTransformType::New( );

  typedef itk::ResampleImageFilter< InputImageType, InputImageType> 
                                    ResampleImageFilterType;
  ResampleImageFilterType::Pointer resampleImageFilter = 
                                    ResampleImageFilterType::New( );
  resampleImageFilter->SetInput( imageReader->GetOutput() );
  resampleImageFilter->SetTransform( imageIdentityTransform );
  resampleImageFilter->SetInterpolator( imageInterpolator );
  resampleImageFilter->SetOutputSpacing( spacing );
  resampleImageFilter->SetOutputOrigin( lowerBound );
  resampleImageFilter->SetSize( resampleSize );
  resampleImageFilter->Update( );
*/

  /** Write the Extracted Image */
/*  typedef itk::ImageFileWriter< InputImageType >  WriterType; 
  WriterType::Pointer imageWriter = WriterType::New( );
  imageWriter->SetInput( resampleImageFilter->GetOutput( ) );
  imageWriter->SetFileName( OutputFileName.c_str( ) );
  imageWriter->Update( );
*/

  return EXIT_SUCCESS;
}
