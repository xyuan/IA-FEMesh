/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ProjectMappedMesh.cxx,v $
  Language:  C++
  Date:      $Date: 2006/10/17 13:53:44 $
  Version:   $Revision: 1.3 $

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
#include <vtkIdList.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>

#include <metaCommand.h>
#include <itkImage.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkThinPlateSplineKernelTransform.h>
#include <itkPoint.h>
#include <itkPointSet.h>

#include "TriLineIntersect.h" 


#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

const unsigned int imageDimension = 3;
typedef itk::Image<float, imageDimension>  DistanceImageType;
typedef DistanceImageType::PointType       ImagePointType;

void GetClosestSurfacePointAndNormal( vtkFloatingPointType *meshPoint, 
                                      vtkPolyData *surface, 
                                      vtkFloatingPointType *surfacePoint, 
                                      vtkFloatingPointType *surfaceNormal);
bool ProjectPointToSurface(vtkFloatingPointType *surfacePoint, 
                           vtkFloatingPointType *surfaceNormal, 
                           vtkPolyData *surface, 
                           vtkFloatingPointType *subjectPoint, 
                           bool isInside);



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
      
  std::cout << "Canonical Surface: " <<  CanonicalSurfaceFilename << std::endl; 
  std::cout << "Subject Surface: " <<  SubjectSurfaceFilename << std::endl; 
  std::cout << "Output Surface: " <<  OutputSurfaceFilename << std::endl; 
  std::cout << "Canonical Mesh: " <<  CanonicalMeshFilename << std::endl; 
  std::cout << "Output Mesh: " <<  OutputMeshFilename << std::endl; 
   

  
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
  
     vtkPolyDataNormals *meshNormals = vtkPolyDataNormals::New();
      meshNormals->SetInput( canonicalPolyData );
      meshNormals->ComputePointNormalsOn( );
//      meshNormals->SetFeatureAngle(60.0);
      meshNormals->Update( );
    
    vtkPolyData *meshWithNormals = meshNormals->GetOutput();
    
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

  /*** Read in the Canonical Mesh ***/ 
  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
    meshReader->SetFileName( CanonicalMeshFilename.c_str() );
    meshReader->Update();  

 //subjectPolyData->Print(std::cout);
    
  const unsigned int numberOfMeshPoints = meshReader->GetOutput()->GetNumberOfPoints(); 
  const unsigned int numberOfMeshCells = meshReader->GetOutput( )->GetNumberOfCells( );
  

  std::cout << "Mesh Points: " << numberOfMeshPoints << std::endl;
  vtkPoints *mappedPoints = vtkPoints::New();
  vtkIdList *cellPoints = vtkIdList::New();
  int numberOfEdgePoints = 0;
  
  vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet( subjectPolyData );
  obb->SetMaxLevel( 99999 );
  obb->SetTolerance(0.001);
  obb->BuildLocator();
  
  for(int p=0; p<numberOfMeshPoints; p++)
    {
    cellPoints->Initialize();
    meshReader->GetOutput( )->GetPointCells(p, cellPoints);
    unsigned int numberOfPointCells = cellPoints->GetNumberOfIds();
    vtkFloatingPointType subjectPoint[3];
    vtkFloatingPointType meshPoint[3];
    meshReader->GetOutput( )->GetPoint(p, meshPoint);
	    
    if ( numberOfPointCells != 8 )
      {
      vtkFloatingPointType surfacePoint[3];
      vtkFloatingPointType surfaceNormal[3];
      numberOfEdgePoints++;      
      meshReader->GetOutput( )->GetPoint(p, meshPoint);
      //std::cout << "Project Point: " << p << " " << numberOfPointCells << " Point: " << meshPoint[0] << " " << meshPoint[1]  << " " << meshPoint[2]  << std::endl;
	
      GetClosestSurfacePointAndNormal(meshPoint, meshWithNormals, surfacePoint, surfaceNormal);
      //std::cout << "GetClosestSurfacePointAndNormal()" << std::endl;
      //std::cout << "\tSurface Point: " << surfacePoint[0] << " " << surfacePoint[1] << " " << surfacePoint[2] << std::endl;
      //std::cout << "\tSurface Normal: " << surfaceNormal[0] << " " << surfaceNormal[1] << " " << surfaceNormal[2] << std::endl;
      
      bool isInside = obb->InsideOrOutside( meshPoint );

      bool foundProjectedPoint = ProjectPointToSurface(meshPoint, surfaceNormal, subjectPolyData, subjectPoint, isInside);
      if ( foundProjectedPoint )
        {
        std::cout << "ProjectPointToSurface(): " << p << std::endl;
        std::cout << "\tInside: " << isInside << std::endl;
        std::cout << "\tProjected Point: " << subjectPoint[0];
        std::cout << " " << subjectPoint[1] << " " << subjectPoint[2] << std::endl;
        }
      else
        {
        std::cout << "Failed to Project Point(): " << p << std::endl;
        std::cout << "\tInside: " << isInside << std::endl;
        std::cout << "\tOriginal Point: " << meshPoint[0] << " ";
        std::cout << meshPoint[1] << " " << meshPoint[2] << std::endl;
        subjectPoint[0] = meshPoint[0]; subjectPoint[1] = meshPoint[1]; subjectPoint[2] = meshPoint[2]; 
        }
      }
    else
      {
      std::cout << "Internal Point: " << p << " " << numberOfPointCells << std::endl;
      std::cout << "\tOriginal Point: " << meshPoint[0] << " ";
      std::cout  << meshPoint[1] << " " << meshPoint[2] << std::endl;
      subjectPoint[0] = meshPoint[0]; subjectPoint[1] = meshPoint[1]; subjectPoint[2] = meshPoint[2]; 
      }
    mappedPoints->InsertPoint(p, subjectPoint);
    }
    std::cout << "Number of Edge Points: " << numberOfEdgePoints << std::endl;
    
  vtkUnstructuredGrid *transformMesh = vtkUnstructuredGrid::New();
  transformMesh->SetPoints( mappedPoints );

    
  for(int c=0; c<numberOfMeshCells; c++)
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
    
  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New();
  meshWriter->SetInput( transformMesh );
  meshWriter->SetFileName( OutputMeshFilename.c_str() );
  meshWriter->SetFileTypeToASCII( );
  meshWriter->Update();
    
    
  
  return 0;
}


void GetClosestSurfacePointAndNormal( vtkFloatingPointType *meshPoint, vtkPolyData *surface, vtkFloatingPointType *surfacePoint, vtkFloatingPointType *surfaceNormal)
{
	const unsigned int numberOfPoints = surface->GetNumberOfPoints();
	vtkFloatingPointType minDistance;
	vtkFloatingPointType distance;
	unsigned int index;
	vtkFloatingPointType currentPoint[3];
	
	surface->GetPoint( 0, currentPoint);
	minDistance = (meshPoint[0] - currentPoint[0]) * (meshPoint[0] - currentPoint[0]) +
				(meshPoint[1] - currentPoint[1]) * (meshPoint[1] - currentPoint[1]) +
				(meshPoint[2] - currentPoint[2]) * (meshPoint[2] - currentPoint[2]);
	index = 0;
	
	for (int i=1; i<numberOfPoints;i++)
	{
		surface->GetPoint( i, currentPoint);
		distance = (meshPoint[0] - currentPoint[0]) * (meshPoint[0] - currentPoint[0]) +
				  (meshPoint[1] - currentPoint[1]) * (meshPoint[1] - currentPoint[1]) +
				  (meshPoint[2] - currentPoint[2]) * (meshPoint[2] - currentPoint[2]);
		if (distance < minDistance)
		{
			index = i;
			minDistance = distance;
		}
	}
	
	/*** Get the Closest Surface Point ***/
	surface->GetPoint( index, currentPoint);
	surfacePoint[0] = currentPoint[0];
	surfacePoint[1] = currentPoint[1];
	surfacePoint[2] = currentPoint[2];
	
	/*** Get Normal for the Surface Point***/
	vtkPointData* surfacePointData = surface->GetPointData();
	vtkDataArray* surfacePointNormal = surfacePointData->GetNormals ();
	surfaceNormal[0] = surfacePointNormal->GetComponent(index,0);
	surfaceNormal[1] = surfacePointNormal->GetComponent(index,1);
	surfaceNormal[2] = surfacePointNormal->GetComponent(index,2);
	
	std::cout << "Mesh Point: " << meshPoint[0] << " " << meshPoint[1] << " " << meshPoint[2] << std::endl;
        std::cout << "Surface Point: " << surfacePoint[0] << " " << surfacePoint[1] << " " << surfacePoint[2] << std::endl;
	std::cout << "Surface Normal: " << surfaceNormal[0] << " " << surfaceNormal[1] << " " << surfaceNormal[2] << std::endl;
	
}


bool ProjectPointToSurface(vtkFloatingPointType *surfacePoint, 
                           vtkFloatingPointType *surfaceNormal, 
                           vtkPolyData *surface, 
                           vtkFloatingPointType *subjectPoint,
                           bool isInside)
{
	const unsigned numberOfCells = surface->GetNumberOfCells();
	vtkIdList *ptIds = vtkIdList::New();
	vtkFloatingPointType minDistance = 10e20;
	vtkFloatingPointType minPt[3];
	vtkFloatingPointType currentPoint[3];
	Ray line;
	Triangle Tri;
	unsigned int triIndex;
	
	//std::cerr << "Number of  Cells : " << numberOfCells << std::endl;
	
	for (int i=1; i<numberOfCells;i++)
	{
		ptIds->Initialize();
		surface->GetCellPoints(i, ptIds);
		if ( ptIds->GetNumberOfIds() == 3 )
		{
			surface->GetPoint( ptIds->GetId(0), currentPoint );
			Tri.V0[0] = currentPoint[0]; Tri.V0[1] = currentPoint[1]; Tri.V0[2] = currentPoint[2]; 
			surface->GetPoint( ptIds->GetId(1), currentPoint );
			Tri.V1[0] = currentPoint[0]; Tri.V1[1] = currentPoint[1]; Tri.V1[2] = currentPoint[2]; 
			surface->GetPoint( ptIds->GetId(2), currentPoint );
			Tri.V2[0] = currentPoint[0]; Tri.V2[1] = currentPoint[1]; Tri.V2[2] = currentPoint[2]; 
		}
		else
		{
			std::cerr << "Number of Point Ids != 3 for Cell : " << i << " " << ptIds->GetNumberOfIds() << std::endl;
		}

		Point p1;
                bool intersectNorm;
		
		if ( isInside )
                  {
		
		  line.P0[0] = surfacePoint[0]; line.P0[1] = surfacePoint[1]; line.P0[2] = surfacePoint[2];
		  line.P1[0] = surfacePoint[0]+surfaceNormal[0]; 
		  line.P1[1] = surfacePoint[1]+surfaceNormal[1]; 
		  line.P1[2] = surfacePoint[2]+surfaceNormal[2];
		
		  //std::cerr << "Tri vert 1 : " << Tri.V0 << std::endl;
		  //std::cerr << "Tri vert 2 : " << Tri.V1 << std::endl;
		  //std::cerr << "Tri vert 3 : " << Tri.V2 << std::endl;
		  //std::cerr << "Ray Pt1 : " << line.P0 << std::endl;
		  //std::cerr << "Ray Pt2 : " << line.P1 << std::endl;
		
		  intersectNorm = LineFacet(line, Tri, &p1);
		  }
                else
                  {
		  line.P1[0] = surfacePoint[0] - surfaceNormal[0]; 
		  line.P1[1] = surfacePoint[1] - surfaceNormal[1]; 
		  line.P1[2] = surfacePoint[2] - surfaceNormal[2];
		
		  intersectNorm = LineFacet(line, Tri, &p1);
		  }
  
		//std::cerr << "Intersect : " << intersect << " Point: " << p << std::endl;
		vtkFloatingPointType distanceNorm = 10e20;
		
		if ( intersectNorm )
		{
                   
			distanceNorm = (surfacePoint[0] - p1[0]) * (surfacePoint[0] - p1[0]) + 
						  (surfacePoint[1] - p1[1]) * (surfacePoint[1] - p1[1]) +
						  (surfacePoint[2] - p1[2]) * (surfacePoint[2] - p1[2]) ;
		}
		
		
		if ( distanceNorm < minDistance) 
		{
			//std::cerr << "Update Min : " << minDistance << " Point: " << p << std::endl;
			minDistance = distanceNorm;
			minPt[0] = p1[0]; minPt[1] = p1[1]; minPt[2] = p1[2];
			triIndex = i;
		}
		
	}
	
	std::cerr << "Min Distance : " << minDistance;
	std::cerr << " Point: " << minPt[0] << " " << minPt[1] << " " << minPt[2] << std::endl;
	
	subjectPoint[0] = minPt[0];
	subjectPoint[1] = minPt[1];
	subjectPoint[2] = minPt[2];

      if (minDistance == 10e20)
      {
	      return false;
      }
      else
      {
	      return true;
      }
      
      
}
