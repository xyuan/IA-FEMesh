/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ItkIcpMorphMesh.cxx,v $
  Language:  C++
  Date:      $Date: 2006/09/18 19:25:43 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef _WIN32
#pragma warning ( disable : 4786 )
#endif

// Software Guide : BeginLatex
//
// This example illustrates how to perform Iterative Closest Point (ICP) 
// registration in ITK using a DistanceMap in order to increase the performance.
// There is of course a trade-off between the time needed for computing the
// DistanceMap and the time saving obtained by its repeated use during the
// iterative computation of the point to point distances. It is then necessary
// in practice to ponder both factors.
//
// \doxygen{EuclideanDistancePointMetric}.
//
// Software Guide : EndLatex 


#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkOBBTree.h>
#include <vtkPoints.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkQuadricDecimation.h>

// Software Guide : BeginCodeSnippet
#include <metaCommand.h>
#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"
#include "itkEuclideanDistancePointMetric.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSet.h"
#include "itkPointSetToPointSetRegistrationMethod.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkPointSetToImageFilter.h"
#include <iostream>
#include <fstream>


int main(int argc, char * argv[] )
{

  MetaCommand command;

  command.SetOption("CanonicalSurface","c",false,"Input Canonical Surface File");
  command.AddOptionField("CanonicalSurface","filename",MetaCommand::STRING,true);

  command.SetOption("SubjectSurface","s",false,"Input Subject Surface File");
  command.AddOptionField("SubjectSurface","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputSurface","o",false,"Output Surface File");
  command.AddOptionField("OutputSurface","filename",MetaCommand::STRING,true);

  command.SetOption("Transform","t",false,"Output Transform File");
  command.AddOptionField("Transform","filename",MetaCommand::STRING,true);
  
  command.SetOption("NumberOfIterations","i",false,"Number of Iterations [100]");
  command.AddOptionField("NumberOfIterations","number",MetaCommand::INT,false,"100");
  
  command.SetOption("EpsilonFunction","f",false,"Episilon Function");
  command.AddOptionField("EpsilonFunction","distance",MetaCommand::FLOAT,false,"1.0e-6");
  
  command.SetOption("GradientTolerance","g",false,"Gradient Tolerance");
  command.AddOptionField("GradientTolerance","tolerance",MetaCommand::FLOAT,false,"1.0e-5");

  command.SetOption("ValueTolerance","d",false,"Value Tolerance");
  command.AddOptionField("ValueTolerance","tolerance",MetaCommand::FLOAT,false,"1.0e-5");
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string CanonicalSurfaceFilename (command.GetValueAsString("CanonicalSurface","filename"));
  const std::string SubjectSurfaceFilename (command.GetValueAsString("SubjectSurface","filename"));
  const std::string OutputSurfaceFilename (command.GetValueAsString("OutputSurface","filename"));
  const std::string TransformFilename (command.GetValueAsString("Transform","filename"));  
      
  int numberOfIterations = command.GetValueAsInt("NumberOfIterations","number");
  float epsilonFunction = command.GetValueAsFloat("EpsilonFunction","distance");
  float gradientTolerance = command.GetValueAsFloat("GradientTolerance","tolerance");
  float valueTolerance = command.GetValueAsFloat("ValueTolerance","tolerance");
  
  std::cout << "Canonical Surface: " <<  CanonicalSurfaceFilename << std::endl; 
  std::cout << "Subject Surface: " <<  SubjectSurfaceFilename << std::endl; 
  std::cout << "Output Surface: " <<  OutputSurfaceFilename << std::endl; 
  std::cout << "Transform: " <<  TransformFilename << std::endl; 
  std::cout << "Iterations: " << numberOfIterations <<std::endl;
  std::cout << "Epsilon Function: " << epsilonFunction <<std::endl;
  std::cout << "Gradient Tolerance: " << gradientTolerance <<std::endl;
  std::cout << "Value Tolerance: " << valueTolerance <<std::endl;
   

  
  /*** Read the Mesh For Distance Map ***/
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
  vtkQuadricDecimation *triangleDecimateFilter = vtkQuadricDecimation::New();
    triangleDecimateFilter->SetInput( canonicalPolyData );
    triangleDecimateFilter->SetTargetReduction( 0.90 );
    triangleDecimateFilter->Update( );
  
  vtkPolyData *subjectPolyData;
  if ( (strstr(SubjectSurfaceFilename.c_str(), ".stl") != NULL) || 
       (strstr(SubjectSurfaceFilename.c_str(), ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(SubjectSurfaceFilename.c_str());
    fixedReader->Update( );
    subjectPolyData = fixedReader->GetOutput();
    }
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName(CanonicalSurfaceFilename.c_str());
    polyReader->Update( );	
    subjectPolyData = polyReader->GetOutput();
    }
 
  vtkQuadricDecimation *triangleDecimateFilter1 = vtkQuadricDecimation::New();
    triangleDecimateFilter1->SetInput( subjectPolyData );
    triangleDecimateFilter1->SetTargetReduction( 0.90 );
    triangleDecimateFilter1->Update( );

  const unsigned int Dimension = 3;

  typedef itk::PointSet< float, Dimension >   PointSetType;

  PointSetType::Pointer fixedPointSet  = PointSetType::New();
  PointSetType::Pointer movingPointSet = PointSetType::New();

  typedef PointSetType::PointType     PointType;

  typedef PointSetType::PointsContainer  PointsContainer;

  PointsContainer::Pointer fixedPointContainer  = PointsContainer::New();
  PointsContainer::Pointer movingPointContainer = PointsContainer::New();

  PointType fixedPoint;
  PointType movingPoint;


  // Convert VTK to ITK for fixed points.
  vtkPoints *fixedVerts = triangleDecimateFilter1->GetOutput()->GetPoints( );
  
  for (unsigned int pointId=0;pointId<fixedVerts->GetNumberOfPoints( );pointId++)
    {
    double meshVert[3];
    fixedVerts->GetPoint(pointId, meshVert);
    fixedPoint[0] = meshVert[0]; fixedPoint[1] = meshVert[1]; fixedPoint[2] = meshVert[2]; 
    fixedPointContainer->InsertElement( pointId, fixedPoint );
    }
  fixedPointSet->SetPoints( fixedPointContainer );
  std::cout << "Number of fixed Points = " 
        << fixedPointSet->GetNumberOfPoints() << std::endl;



  // Convert VTK to ITK for Moving points.
  vtkPoints *movingVerts = triangleDecimateFilter->GetOutput()->GetPoints( );
  
  for (unsigned int pointId=0;pointId<movingVerts->GetNumberOfPoints( );pointId++)
    {
    double meshVert[3];
    movingVerts->GetPoint(pointId, meshVert);
    movingPoint[0] = meshVert[0]; movingPoint[1] = meshVert[1]; movingPoint[2] = meshVert[2]; 
    movingPointContainer->InsertElement( pointId, movingPoint );
    }

  movingPointSet->SetPoints( movingPointContainer );
  std::cout << "Number of moving Points = " 
      << movingPointSet->GetNumberOfPoints() << std::endl;


//-----------------------------------------------------------
// Set up  the Metric
//-----------------------------------------------------------
  typedef itk::EuclideanDistancePointMetric<  
                                    PointSetType, 
                                    PointSetType>
                                                    MetricType;

  typedef MetricType::TransformType                 TransformBaseType;
  typedef TransformBaseType::ParametersType         ParametersType;
  typedef TransformBaseType::JacobianType           JacobianType;

  MetricType::Pointer  metric = MetricType::New();


//-----------------------------------------------------------
// Set up a Transform
//-----------------------------------------------------------

  typedef itk::AffineTransform< double, Dimension >      TransformType;

  TransformType::Pointer transform = TransformType::New();


  // Optimizer Type
  typedef itk::LevenbergMarquardtOptimizer OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  //optimizer->SetUseCostFunctionGradient(false);

  // Registration Method
  typedef itk::PointSetToPointSetRegistrationMethod< 
                                            PointSetType, 
                                            PointSetType >
                                                    RegistrationType;


  RegistrationType::Pointer   registration  = RegistrationType::New();

  // Scale the translation components of the Transform in the Optimizer
  OptimizerType::ScalesType scales( transform->GetNumberOfParameters() );
  scales.Fill( 0.01 );


  optimizer->SetScales( scales );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetValueTolerance( valueTolerance );
  optimizer->SetGradientTolerance( gradientTolerance );
  optimizer->SetEpsilonFunction( epsilonFunction );

  // Start from an Identity transform (in a normal case, the user 
  // can probably provide a better guess than the identity...
  transform->SetIdentity();

  registration->SetInitialTransformParameters( transform->GetParameters() );

  //------------------------------------------------------
  // Connect all the components required for Registration
  //------------------------------------------------------
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetFixedPointSet( fixedPointSet );
  registration->SetMovingPointSet(   movingPointSet   );


  //------------------------------------------------------
  // Prepare the Distance Map in order to accelerate 
  // distance computations.
  //------------------------------------------------------
  //
  //  First map the Fixed Points into a binary image.
  //  This is needed because the DanielssonDistance 
  //  filter expects an image as input.
  //
  //-------------------------------------------------
  typedef itk::Image< unsigned char,  Dimension >  BinaryImageType;

  typedef itk::PointSetToImageFilter< 
                            PointSetType,
                            BinaryImageType> PointsToImageFilterType;

  PointsToImageFilterType::Pointer 
                  pointsToImageFilter = PointsToImageFilterType::New();
  
  pointsToImageFilter->SetInput( fixedPointSet );

  BinaryImageType::SpacingType spacing;
  spacing.Fill( 1.0 );

  BinaryImageType::PointType origin;
  origin.Fill( 0.0 );

  pointsToImageFilter->SetSpacing( spacing );
  pointsToImageFilter->SetOrigin( origin   );
  
  pointsToImageFilter->Update();

  BinaryImageType::Pointer binaryImage = pointsToImageFilter->GetOutput();


  typedef itk::Image< unsigned short, Dimension >  DistanceImageType;

  typedef itk::DanielssonDistanceMapImageFilter< 
                                          BinaryImageType,
                                          DistanceImageType> DistanceFilterType;

  DistanceFilterType::Pointer distanceFilter = DistanceFilterType::New();
  
  distanceFilter->SetInput( binaryImage );

  distanceFilter->Update();
std::cout << "Distance Map" << std::endl;

  //metric->SetDistanceMap( distanceFilter->GetOutput() );


  try 
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << e << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Solution = " << transform->GetParameters() << std::endl;

  vtkPoints *surfaceVerts = canonicalPolyData->GetPoints( );
  vtkPoints *warpVerts = vtkPoints::New();
  warpVerts->SetNumberOfPoints( surfaceVerts->GetNumberOfPoints( ) );
  for (int i=0;i<surfaceVerts->GetNumberOfPoints( );i++)
    {
    double meshVert[3];
    surfaceVerts->GetPoint( i, meshVert );
    TransformType::InputPointType origPoint;
    origPoint[0] = meshVert[0]; origPoint[1] = meshVert[1]; origPoint[2] = meshVert[2]; 
    TransformType::OutputPointType transPoint;
    transPoint = transform->TransformPoint( origPoint );   
    //std::cout << "Trans: " << i << " " << origPoint << " " << transPoint << std::endl;
    meshVert[0] = transPoint[0]; meshVert[1] = transPoint[1]; meshVert[2] = transPoint[2]; 
    warpVerts->SetPoint(i, meshVert);
    }

  vtkPolyData *resamplePolyData = vtkPolyData::New();
  resamplePolyData->SetPoints( warpVerts );
  resamplePolyData->SetLines( canonicalPolyData->GetLines( ) );
  resamplePolyData->SetPolys( canonicalPolyData->GetPolys( ) );

  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
    meshWriter->SetFileName( OutputSurfaceFilename.c_str() );
    meshWriter->SetFileTypeToASCII();
    meshWriter->SetInput( resamplePolyData );
    meshWriter->Update( );

  


// Software Guide : EndCodeSnippet


  return EXIT_SUCCESS;

}

