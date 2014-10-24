/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ICPInitMappedMesh.cxx,v $
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
#include <vtkUnstructuredGridWriter.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkIdList.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>


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

  command.SetOption("CanonicalMesh","m",false,"Input Canonical Mesh File");
  command.AddOptionField("CanonicalMesh","filename",MetaCommand::STRING,true);

  command.SetOption("OutputMesh","x",false,"Output Canonical Mesh File");
  command.AddOptionField("OutputMesh","filename",MetaCommand::STRING,true);
 
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
  const std::string CanonicalMeshFilename (command.GetValueAsString("CanonicalMesh","filename"));
  const std::string OutputMeshFilename (command.GetValueAsString("OutputMesh","filename"));
  const std::string TransformFilename (command.GetValueAsString("Transform","filename"));  
      
  int numberOfIterations = command.GetValueAsInt("NumberOfIterations","number");
  int numberOfLandmarks = command.GetValueAsInt("NumberOfLandmarks","landmarks");
  float maxMeanDistance = command.GetValueAsFloat("MaximumMeanDistance","distance");
  
  std::cout << "Canonical Surface: " <<  CanonicalSurfaceFilename << std::endl; 
  std::cout << "Subject Surface: " <<  SubjectSurfaceFilename << std::endl; 
  std::cout << "Output Surface: " <<  OutputSurfaceFilename << std::endl; 
  std::cout << "Canonical Mesh: " <<  CanonicalMeshFilename << std::endl; 
  std::cout << "Output Mesh: " <<  OutputMeshFilename << std::endl; 
  std::cout << "Iterations: " << numberOfIterations <<std::endl;
  std::cout << "Landmarks: " << numberOfLandmarks <<std::endl;
  std::cout << "Mean Distance: " << maxMeanDistance <<std::endl;
   

  
  /*** Read the Canonical Surface ***/
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
  
  /*** Read the Subject Surface ***/
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
    polyReader->SetFileName(SubjectSurfaceFilename.c_str());
    polyReader->Update( );	
    subjectPolyData = polyReader->GetOutput();
    }

     

  vtkIterativeClosestPointTransform *icpTransform = vtkIterativeClosestPointTransform::New();
    icpTransform->SetSource( canonicalPolyData );
    icpTransform->SetTarget( subjectPolyData );
    icpTransform->StartByMatchingCentroidsOn( );
    icpTransform->GetLandmarkTransform( )->SetModeToAffine( );
    icpTransform->SetMaximumNumberOfIterations( numberOfIterations );
    icpTransform->CheckMeanDistanceOn();
    icpTransform->SetMaximumMeanDistance( maxMeanDistance );
    icpTransform->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
    icpTransform->SetMaximumNumberOfLandmarks( numberOfLandmarks );
    icpTransform->Update( );

  /* Write out the Transformed Canonical Surface */
  vtkTransformPolyDataFilter *resampleMeshFilter = vtkTransformPolyDataFilter::New();
    resampleMeshFilter->SetInput( canonicalPolyData );
    resampleMeshFilter->SetTransform( icpTransform );
    resampleMeshFilter->Update( );

  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
    meshWriter->SetFileName( OutputSurfaceFilename.c_str() );
    meshWriter->SetFileTypeToASCII();
    meshWriter->SetInput( resampleMeshFilter->GetOutput( ) );
    meshWriter->Update( );
  
  /* Read in the Canonical Mesh, Apply Transform and Write out */ 
  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
    meshReader->SetFileName( CanonicalMeshFilename.c_str() );
    meshReader->Update();  

  /* Transform Mesh Using ICP Transform */
  const unsigned int numberOfPoints = meshReader->GetOutput( )->GetNumberOfPoints( );
  const unsigned int numberOfCells = meshReader->GetOutput( )->GetNumberOfCells( );
  vtkPoints *transformPoints = vtkPoints::New();
  
  for(int p=0; p<numberOfPoints; p++)
    {
    vtkFloatingPointType *opoint = meshReader->GetOutput( )->GetPoint( p );
    vtkFloatingPointType tpoint[3];
    icpTransform->TransformPoint(opoint, tpoint);
    std::cout << "Original Point: " << opoint[0] << " " << opoint[1] << " " << opoint[2];
    std::cout << " Transformed Point: " << tpoint[0] << " " << tpoint[1] << " " << tpoint[2] << std::endl;
    transformPoints->InsertPoint(p, tpoint);
    }

  vtkUnstructuredGrid *transformMesh = vtkUnstructuredGrid::New();
  transformMesh->SetPoints( transformPoints );
  vtkIdList *cellPoints = vtkIdList::New();

  for(int c=0; c<numberOfCells; c++)
    {
    switch ( meshReader->GetOutput( )->GetCellType( c ) )
      {	  
      case VTK_TETRA: 
	cellPoints->Reset();
	meshReader->GetOutput( )->GetCellPoints(c, cellPoints);
	transformMesh->InsertNextCell(VTK_TETRA, cellPoints);
        break;
      case VTK_HEXAHEDRON:
	cellPoints->Reset();
	meshReader->GetOutput( )->GetCellPoints(c, cellPoints);
	transformMesh->InsertNextCell(VTK_HEXAHEDRON, cellPoints);
	break;
      }			  
    }

  vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
    writer->SetInput( transformMesh );
    writer->SetFileName( OutputMeshFilename.c_str() );
    writer->SetFileTypeToASCII( );
    writer->Update();

  std::cout << "Wrote ICP Registered Mesh" <<std::endl;
 
  return 0;
}


