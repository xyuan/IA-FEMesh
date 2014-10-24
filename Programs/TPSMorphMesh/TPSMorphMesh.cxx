/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: TPSMorphMesh.cxx,v $
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


#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <string>

#include <vtkPolyData.h>
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

#include <metaCommand.h>
#include <itkImage.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkThinPlateSplineKernelTransform.h>
#include <itkPoint.h>
#include <itkPointSet.h>



#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

const unsigned int imageDimension = 3;
typedef itk::Image<float, imageDimension>  DistanceImageType;
typedef DistanceImageType::PointType       ImagePointType;

float EucledianDistance (double point[3], ImagePointType imageLoc);


//  Register two surface meshes using the Iterative Closest Point Algorithm


int main( int argc, char * argv[] )
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
  
  command.SetOption("NumberOfLandmarks","n",false,"Number of Landmarks [100]");
  command.AddOptionField("NumberOfLandmarks","landmarks",MetaCommand::INT,false,"100");

  command.SetOption("MaximumMeanDistance","d",false,"Maximum Mean Distance");
  command.AddOptionField("MaximumMeanDistance","distance",MetaCommand::FLOAT,false,"1.0");
  
  
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string CanonicalSurfaceFilename (command.GetValueAsString("CanonicalSurface","filename"));
  const std::string SubjectSurfaceFilename (command.GetValueAsString("SubjectSurface","filename"));
  const std::string OutputSurfaceFilename (command.GetValueAsString("OutputSurface","filename"));
  const std::string TransformFilename (command.GetValueAsString("Transform","filename"));  
      
  int numberOfIterations = command.GetValueAsInt("NumberOfIterations","number");
  int numberOfLandmarks = command.GetValueAsInt("NumberOfLandmarks","landmarks");
  float maxMeanDistance = command.GetValueAsFloat("MaximumMeanDistance","distance");
  
  std::cout << "Canonical Surface: " <<  CanonicalSurfaceFilename << std::endl; 
  std::cout << "Subject Surface: " <<  SubjectSurfaceFilename << std::endl; 
  std::cout << "Output Surface: " <<  OutputSurfaceFilename << std::endl; 
  std::cout << "TPS Transform: " <<  TransformFilename << std::endl; 
  std::cout << "Iterations: " << numberOfIterations <<std::endl;
  std::cout << "Landmarks: " << numberOfLandmarks <<std::endl;
  std::cout << "Mean Distance: " << maxMeanDistance <<std::endl;
   

  
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

  vtkQuadricDecimation *triangleDecimateFilter = vtkQuadricDecimation::New();
    triangleDecimateFilter->SetInput( subjectPolyData );
    triangleDecimateFilter->SetTargetReduction( 0.99 );
    triangleDecimateFilter->Update( );
  

  vtkQuadricDecimation *triangleDecimateFilter1 = vtkQuadricDecimation::New();
    triangleDecimateFilter1->SetInput( canonicalPolyData );
    triangleDecimateFilter1->SetTargetReduction( 0.995 );
    triangleDecimateFilter1->Update( );
    

  vtkIterativeClosestPointTransform *icpTransform = vtkIterativeClosestPointTransform::New();
    icpTransform->SetSource( triangleDecimateFilter1->GetOutput( ) );
    icpTransform->SetTarget( triangleDecimateFilter->GetOutput( ) );
    icpTransform->StartByMatchingCentroidsOn( );
    icpTransform->GetLandmarkTransform( )->SetModeToAffine( );
    icpTransform->SetMaximumNumberOfIterations( numberOfIterations );
    icpTransform->CheckMeanDistanceOn();
    icpTransform->SetMaximumMeanDistance( maxMeanDistance );
    icpTransform->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
    icpTransform->SetMaximumNumberOfLandmarks( numberOfLandmarks );
    icpTransform->Update( );

  /* Write the Transformed Surface */
  vtkTransformPolyDataFilter *resampleMeshFilter = vtkTransformPolyDataFilter::New();
    resampleMeshFilter->SetInput( triangleDecimateFilter1->GetOutput( ) );
    resampleMeshFilter->SetTransform( icpTransform );
    resampleMeshFilter->Update( );
  


  vtkPoints *sourcePoints = icpTransform->GetLandmarkTransform( )->GetSourceLandmarks( );
  vtkPoints *targetPoints = icpTransform->GetLandmarkTransform( )->GetTargetLandmarks( );
std::cout << "ICP Register Complete" <<std::endl;

  /* Now Create ITK TPS Transform */
  typedef  double CoordinateRepType;
  typedef  itk::ThinPlateSplineKernelTransform< CoordinateRepType, imageDimension> TransformType;
  typedef  itk::Point< CoordinateRepType, imageDimension >  PointType;
  typedef  std::vector< PointType >                   PointArrayType;
  typedef  TransformType::PointSetType      PointSetType;
  typedef  PointSetType::Pointer            PointSetPointer;
  typedef  PointSetType::PointIdentifier  PointIdType;

  int numberOfPoints = sourcePoints->GetNumberOfPoints( );
  
  PointSetType::Pointer sourceLandMarks = PointSetType::New();
  PointSetType::Pointer targetLandMarks = PointSetType::New();
  PointType p1;     
  PointType p2;
  PointSetType::PointsContainer::Pointer sourceLandMarkContainer = 
                                   sourceLandMarks->GetPoints();
  PointSetType::PointsContainer::Pointer targetLandMarkContainer = 
                                   targetLandMarks->GetPoints();
  
  PointIdType id = itk::NumericTraits< PointIdType >::Zero;
  for (int i=0;i<numberOfPoints;i++)
    {
    double meshVert[3];
    sourcePoints->GetPoint(i, meshVert);
    p1[0] = meshVert[0]; p1[1] = meshVert[1]; p1[2] = meshVert[2]; 
    targetPoints->GetPoint(i, meshVert);
    p2[0] = meshVert[0]; p2[1] = meshVert[1]; p2[2] = meshVert[2]; 
    sourceLandMarkContainer->InsertElement( id, p1 );
    targetLandMarkContainer->InsertElement( id, p2 );
    std::cout << "Landmark:" << i << " " << p1 << " " << p2 << std::endl;
    id++;
    }
 std::cout << "Transform Points" <<std::endl;
 
  TransformType::Pointer tps = TransformType::New();
  tps->SetSourceLandmarks( sourceLandMarks );
  tps->SetTargetLandmarks( targetLandMarks );
  tps->ComputeWMatrix();

 std::cout << "Created TPS" <<std::endl;
 
  vtkPoints *surfaceVerts = resampleMeshFilter->GetOutput()->GetPoints( );
  vtkPoints *tpsSurfaceVerts = vtkPoints::New();
    tpsSurfaceVerts->SetNumberOfPoints( surfaceVerts->GetNumberOfPoints( ) );
  for (int i=0;i<surfaceVerts->GetNumberOfPoints( );i++)
    {
    double meshVert[3];
    surfaceVerts->GetPoint( i, meshVert );
    TransformType::InputPointType origPoint;
    origPoint[0] = meshVert[0]; origPoint[1] = meshVert[1]; origPoint[2] = meshVert[2]; 
    TransformType::OutputPointType transPoint;
    transPoint = tps->TransformPoint( origPoint );   
std::cout << "Trans: " << i << " " << origPoint << " " << transPoint << std::endl;
    meshVert[0] = transPoint[0]; meshVert[1] = transPoint[1]; meshVert[2] = transPoint[2]; 
    tpsSurfaceVerts->SetPoint(i, meshVert);
    }

  vtkPolyData *resamplePolyData = vtkPolyData::New();
  resamplePolyData->SetPoints( tpsSurfaceVerts );
  resamplePolyData->SetLines( resampleMeshFilter->GetOutput()->GetLines( ) );
  resamplePolyData->SetPolys( resampleMeshFilter->GetOutput()->GetPolys( ) );
 std::cout << "Update PolyData" <<std::endl;
 
  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
    meshWriter->SetFileName( OutputSurfaceFilename.c_str() );
    meshWriter->SetFileTypeToASCII();
    meshWriter->SetInput( resamplePolyData );
    meshWriter->Update( );

std::cout << "Wrote Surface" <<std::endl;
 
  return 0;
}


