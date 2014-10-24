/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ModelToImageRegistration2.cxx,v $
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







#include "itkSpatialObjectToImageFilter.h"
#include "itkBinaryMaskToNarrowBandPointSetFilter.h"
//  Software Guide : EndCodeSnippet 

#include "itkBinaryMaskToNarrowBandPointSetFilter.h"

#include "itkImage.h"
#include "itkPointSet.h"
#include "itkPointSetToImageRegistrationMethod.h"
#include "itkNormalizedCorrelationPointSetToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkRigid2DTransform.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include <itkLBFGSBOptimizer.h>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkPoints.h>

//
// Observer to the optimizer
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
  typedef   itk::RegularStepGradientDescentOptimizer  OptimizerType;
  typedef   const OptimizerType   *           OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( typeid( event ) != typeid( itk::IterationEvent ) )
        {
        return;
        }

      OptimizerType::DerivativeType gradient = optimizer->GetGradient();
      OptimizerType::ScalesType     scales   = optimizer->GetScales();

      double magnitude2 = 0.0;

      for(unsigned int i=0; i<gradient.size(); i++)
        {
        const double fc = gradient[i] / scales[i];
        magnitude2 += fc * fc;
        }  

      const double gradientMagnitude = sqrt( magnitude2 );

      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << gradientMagnitude << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};




int main( int argc, char * argv [] )
{

  MetaCommand command;

  command.SetOption("CanonicalSurface","c",false,"Input Canonical Surface File");
  command.AddOptionField("CanonicalSurface","filename",MetaCommand::STRING,true);

  command.SetOption("SubjectDistanceMap","s",false,"Input Subject Distance Map");
  command.AddOptionField("SubjectDistanceMap","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputSurface","o",false,"Output Surface File");
  command.AddOptionField("OutputSurface","filename",MetaCommand::STRING,true);

  command.SetOption("SpatialSampleScale","ss",false,"Spatial Sample Scale");
  command.AddOptionField("OutputSurface","value",MetaCommand::INT,false,"100");

  command.SetOption("MaxNumberOfIterations","ni",false,"Maximum Number of Iterations");
  command.AddOptionField("MaxNumberOfIterations","number",MetaCommand::INT,false, "500");

  command.SetOption("MaxNumberOfEvaluations","ne",false,"Maximum Number of Evaluations");
  command.AddOptionField("MaxNumberOfEvaluations","number",MetaCommand::INT,false, "500");

  command.SetOption("MaxNumberOfCorrections","nc",false,"Maximum Number of Corrections");
  command.AddOptionField("MaxNumberOfCorrections","number",MetaCommand::INT,false, "12");

  command.SetOption("grid","g",false,"Grid Size [12 12 12]");
  command.AddOptionField("grid","xsize",MetaCommand::INT,false,"12");
  command.AddOptionField("grid","ysize",MetaCommand::INT,false,"12");
  command.AddOptionField("grid","zsize",MetaCommand::INT,false,"12");
  
  command.SetOption("border","b",false,"Border Size [3 3 3]");
  command.AddOptionField("border","xsize",MetaCommand::INT,false,"3");
  command.AddOptionField("border","ysize",MetaCommand::INT,false,"3");
  command.AddOptionField("border","zsize",MetaCommand::INT,false,"3");
   
  command.SetOption("convergence","cf",false,"Convergence Factor [10000000]");
  command.AddOptionField("convergence","factor",MetaCommand::FLOAT,false,"10000000");
  
  command.SetOption("tolerance","gt",false,"Gradient Tolerance ");
  command.AddOptionField("tolerance","tolerance",MetaCommand::FLOAT,false,"0.0001");

  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string CanonicalSurfaceFilename (command.GetValueAsString("CanonicalSurface","filename"));
  const std::string SubjectDistanceMap (command.GetValueAsString("SubjectDistanceMap","filename"));
  const std::string OutputSurfaceFilename (command.GetValueAsString("OutputSurface","filename"));
  
  int Iterations = command.GetValueAsInt("MaxNumberOfIterations","number");
  int NumberOfCorrections = command.GetValueAsInt("corrections","number");
  int NumberOfEvaluations = command.GetValueAsInt("evaluations","number");
  //int HistogramBins = command.GetValueAsInt("histogram","bins");  
  int SpatialScale = command.GetValueAsInt("SpatialSampleScale","value");  
  float Convergence = command.GetValueAsFloat("convergence","factor");
  float Tolerance = command.GetValueAsFloat("tolerance","tolerance");
  int GridXSize = command.GetValueAsInt("grid","xsize");
  int GridYSize = command.GetValueAsInt("grid","ysize");
  int GridZSize = command.GetValueAsInt("grid","zsize");
  int BorderXSize = command.GetValueAsInt("border","xsize");
  int BorderYSize = command.GetValueAsInt("border","ysize");
  int BorderZSize = command.GetValueAsInt("border","zsize");

  
  std::cout << "Canonical Surface: " <<  CanonicalSurfaceFilename << std::endl; 
  std::cout << "Subject Distance Map: " <<  SubjectDistanceMap << std::endl; 
  std::cout << "Output Surface: " <<  OutputSurfaceFilename << std::endl; 
  std::cout << "Grid X Size: " << GridXSize <<std::endl;
  std::cout << "Grid Y Size: " << GridYSize <<std::endl;
  std::cout << "Grid Z Size: " << GridZSize <<std::endl;
  std::cout << "Border X Size: " << BorderXSize <<std::endl;
  std::cout << "Border Y Size: " << BorderYSize <<std::endl;
  std::cout << "Border Z Size: " << BorderZSize <<std::endl;
  std::cout << "Corrections: " << NumberOfCorrections <<std::endl;
  std::cout << "Evaluations: " << NumberOfEvaluations <<std::endl;
  //std::cout << "Histogram: " << HistogramBins <<std::endl;
  std::cout << "Scale: " << SpatialScale <<std::endl;  
  std::cout << "Convergence: " << Convergence <<std::endl;  
  std::cout << "Tolerance: " << Tolerance <<std::endl;  
  std::cout << "Iterations: " << Iterations <<std::endl;
  std::cout << "Index: " << MovingImageIndex <<std::endl;      

  const unsigned int Dimension = 3;

  typedef unsigned char MaskPixelType;
  typedef itk::Image< MaskPixelType, Dimension > MaskImageType;

  typedef  float   PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;


  typedef itk::MeshSpatialObject< Dimension >    SpatialObjectType;

  typedef itk::SpatialObjectToImageFilter< 
                              SpatialObjectType, 
                              MaskImageType 
                                >   SpatialObjectToImageFilterType;

  typedef itk::PointSet< float, Dimension >       FixedPointSetType;


  typedef itk::BinaryMaskToNarrowBandPointSetFilter<
                                    MaskImageType,
                                    FixedPointSetType 
                                            >  NarrowBandFilterType;

  
  

  //itk::BSplineDeformableTransform
  //itk::ElasticBodySplineKernelTransform - Landmark Based
  

  typedef itk::ImageFileReader< ImageType >      ImageReaderType;
  ImageReaderType::Pointer                       movingImageReader;
  FixedPointSetType::Pointer            fixedPointSet;
  ImageType::ConstPointer               movingImage;

 

  movingImageReader        = ImageReaderType::New();
  movingImageReader->SetFileName( SubjectDistanceMap.c_str() );
  try
    {
    movingImageReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem reading distance Map = " << SubjectDistanceMap.c_str() << std::endl;
    return EXIT_FAILURE;
    }

  movingImage = movingImageReader->GetOutput();
  
  
  /* Load the Fixed Surface File - VTK Format */

  vtkPolyData *canonicalPolyData;
  if ( (strstr(CanonicalSurfaceFilename.c_str(), ".stl") != NULL) || 
       (strstr(CanonicalSurfaceFilename.c_str(), ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(CanonicalSurfaceFilename.c_str());
    fixedReader->Update( );
    canonicalPolyData = fixedReader->GetOutput();
    }
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName(CanonicalSurfaceFilename.c_str());
    polyReader->Update( );	
    canonicalPolyData = polyReader->GetOutput();
    }


  /* Iterate through Surface Points and add to point set */
  const unsigned int numberOfPoints = canonicalPolyData->GetNumberOfPoints();
  vtkPoints * vtkpoints =  canonicalPolyData->GetPoints();
 
  for (unsigned int i=0;i<numberOfPoints;i++)
    {
    	fixedPointSet->SetPoint(i, point);
    }
  
  fixedPointSet->Print( std::cout );


  /* Now Set up the Registration */
  typedef itk::BSplineDeformableTransform< double, Dimension, 3  > TransformType;
  typedef TransformType::ParametersType          ParametersType;
  TransformType::Pointer                         transform;

  typedef itk::PointSetToImageRegistrationMethod< 
                                    FixedPointSetType,
                                    ImageType  > RegistrationType;
  RegistrationType::Pointer                      registrationMethod;

  
  typedef itk::LBFGSBOptimizer    OptimizerType;
  OptimizerType::Pointer          optimizer;
  typedef OptimizerType::BoundSelectionType        OptimizerBoundSelectionType;
  typedef OptimizerType::BoundValueType            OptimizerBoundValueType;

  typedef itk::LinearInterpolateImageFunction< 
                                    ImageType,
                                    double     > LinearInterpolatorType;
  LinearInterpolatorType::Pointer       linearInterpolator;
  typedef itk::NormalizedCorrelationPointSetToImageMetric< 
                                    FixedPointSetType, 
                                    ImageType  >   MetricType; 
  MetricType::Pointer                   metric;                                          
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  typedef CommandIterationUpdate          IterationObserverType;
  IterationObserverType::Pointer          iterationObserver;
  

  metric              = MetricType::New();
  transform           = TransformType::New();
  optimizer           = OptimizerType::New();
  linearInterpolator  = LinearInterpolatorType::New();
  iterationObserver   = IterationObserverType::New();
  

  registrationMethod  = RegistrationType::New();
  registrationMethod->SetOptimizer(     optimizer     );
  registrationMethod->SetInterpolator(  linearInterpolator  );
  registrationMethod->SetMetric(        metric        );
  registrationMethod->SetTransform(     transform     );

  registrationMethod->SetMovingImage(   movingImage  );
  registrationMethod->SetFixedPointSet( fixedPointSet );

  
  OptimizerBoundSelectionType boundSelect( m_Output->GetNumberOfParameters() );
  OptimizerBoundValueType     upperBound( m_Output->GetNumberOfParameters() );
  OptimizerBoundValueType     lowerBound( m_Output->GetNumberOfParameters() );

  boundSelect.Fill( 0 );
  upperBound.Fill( 0.0 );
  lowerBound.Fill( 0.0 );

  optimizer->SetBoundSelection( boundSelect );
  optimizer->SetUpperBound( upperBound );
  optimizer->SetLowerBound( lowerBound );

  optimizer->SetCostFunctionConvergenceFactor( m_CostFunctionConvergenceFactor );
  optimizer->SetProjectedGradientTolerance( m_ProjectedGradientTolerance );
  optimizer->SetMaximumNumberOfIterations( m_MaximumNumberOfIterations );
  optimizer->SetMaximumNumberOfEvaluations( m_MaximumNumberOfEvaluations );
  optimizer->SetMaximumNumberOfCorrections( m_MaximumNumberOfCorrections );
  optimizer->MinimizeOn();
  optimizer->AddObserver( itk::IterationEvent(), iterationObserver );


  
  
  /*** Make this a Parameter ***/
  const unsigned int numberOfSamples = fixedImageRegion.GetNumberOfPixels() / m_SpatialSampleScale;

  metric->SetNumberOfSpatialSamples( numberOfSamples );
  metric->ReinitializeSeed( 76926294 );
  metric->SetNumberOfHistogramBins( m_BSplineHistogramBins );


  TransformType::TranslationType  initialTranslation;
  const unsigned int numberOfParameters =
               initialTranslation->GetNumberOfParameters();
  
  TransformParametersType parameters( numberOfParameters );
  parameters.Fill( 0.0 );
  initialTranslation->SetIdentity();
  initialTranslation->SetParameters( parameters );
  

  registration->SetInitialTransformParameters( initialTranslation->GetParameters() );


  registrationMethod->SetInitialTransformParameters( 
                                  transform->GetParameters() ); 

 
  try
    {
    registrationMethod->StartRegistration(); 
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem found during the registration" << std::endl;
    std::cerr << argv[1] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }



  ParametersType transformParameters = 
         registrationMethod->GetLastTransformParameters();


  TransformType::OutputPointType center = transform->GetCenter();

  std::cout << "Registration parameter = " << std::endl;
  std::cout << "Rotation center = " << center << std::endl;
  std::cout << "Parameters = " << transformParameters << std::endl;


  return 0;
}


