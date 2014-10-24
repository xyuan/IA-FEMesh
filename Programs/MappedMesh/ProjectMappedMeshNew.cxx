/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ProjectMappedMeshNew.cxx,v $
  Language:  C++
  Date:      $Date: 2006/10/19 15:19:47 $
  Version:   $Revision: 1.1 $

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
#include <vtkCell.h>

#include <metaCommand.h>
#include <itkImage.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkThinPlateSplineKernelTransform.h>
#include <itkPoint.h>
#include <itkPointSet.h>

#include "TriLineIntersect.h" 
#include "CheckHexahedronTopology.h"

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
  mappedPoints->SetNumberOfPoints( numberOfMeshPoints );
  vtkIdList *cellPoints = vtkIdList::New();
  vtkIdList *modifiedList = vtkIdList::New();
  modifiedList->SetNumberOfIds( numberOfMeshPoints );
  for (int i=0; i<numberOfMeshPoints;i++)
    {
    modifiedList->SetId( i, 0);
    }
  int numberOfEdgePoints = 0;
  
  vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet( subjectPolyData );
  obb->SetMaxLevel( 99999 );
  obb->SetTolerance(0.001);
  obb->BuildLocator();

/*********************  Adjust Exterior Nodes  *************************/  
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
      modifiedList->SetId(p, 1);
      mappedPoints->SetPoint(p, subjectPoint);
      }
    /*
    else
      {
      std::cout << "Internal Point: " << p << " " << numberOfPointCells << std::endl;
      std::cout << "\tOriginal Point: " << meshPoint[0] << " ";
      std::cout  << meshPoint[1] << " " << meshPoint[2] << std::endl;
      subjectPoint[0] = meshPoint[0]; subjectPoint[1] = meshPoint[1]; subjectPoint[2] = meshPoint[2]; 
      }
    mappedPoints->InsertPoint(p, subjectPoint);
    */
    }
    std::cout << "Number of Edge Points: " << numberOfEdgePoints << std::endl;
/********************* End Adjust Exterior Nodes *************************/


/*********************** Adjust Interior Nodes ***************************/
  int numberOfInteriorPoints = 0;
  int iterations = 0;
  bool done = false;
  while ( ! done )
    { 
    done = true;
    iterations++;
    for(int p=0; p<numberOfMeshPoints; p++)
      {
      cellPoints->Initialize();
      meshReader->GetOutput( )->GetPointCells(p, cellPoints);
      unsigned int numberOfPointCells = cellPoints->GetNumberOfIds();
      vtkFloatingPointType subjectPoint[3];
      vtkFloatingPointType meshPoint[3];
      meshReader->GetOutput( )->GetPoint(p, meshPoint);

      /* Check if this point needs to be adjusted */   
      if (( numberOfPointCells == 8 ) && (modifiedList->GetId( p ) == 0))
        {
        int numberOfModifiedNeighbors = 0;
        vtkFloatingPointType modifiedLocation[3];
        modifiedLocation[0] = 0.0;
        modifiedLocation[1] = 0.0;
        modifiedLocation[2] = 0.0;

        /* Adjust current point if edge connected nodes have been adjusted */
        for (int i=0;i<numberOfPointCells;i++)
          {
          vtkCell *currentCell = meshReader->GetOutput( )->GetCell( cellPoints->GetId(i) );
          unsigned int numCellEdges = currentCell->GetNumberOfEdges();
          for (int j=0;j<numCellEdges;j++)
            {
            vtkCell *currentEdge = currentCell->GetEdge( j );
            currentEdge->GetNumberOfPoints( );
            unsigned int edgePointId1 = currentEdge->GetPointId( 0 );
            unsigned int edgePointId2 = currentEdge->GetPointId( 1 );
            std::cout << "Edge Point 1 ID: " << edgePointId1 << std::endl;
            std::cout << "Edge Point 2 ID: " << edgePointId2 << std::endl;
            if ( edgePointId1 == p )
              {
              if ( (modifiedList->GetId( edgePointId2 ) != 0 ) &&
                   (modifiedList->GetId( edgePointId2 ) < iterations ) )
                {
                numberOfModifiedNeighbors++;
                vtkFloatingPointType newPoint[3];
                mappedPoints->GetPoint(edgePointId2, newPoint);
                vtkFloatingPointType origPoint[3];
                meshReader->GetOutput( )->GetPoint(edgePointId2, origPoint);
                modifiedLocation[0] += newPoint[0] - origPoint[0];
                modifiedLocation[1] += newPoint[1] - origPoint[1];
                modifiedLocation[2] += newPoint[2] - origPoint[2];
                }
              }
            else if ( edgePointId2 == p )
              {
              if ( (modifiedList->GetId( edgePointId1 ) != 0 ) && 
                   (modifiedList->GetId( edgePointId1 ) < iterations ) )
                {
                numberOfModifiedNeighbors++;
                vtkFloatingPointType newPoint[3];
                mappedPoints->GetPoint(edgePointId1, newPoint);
                vtkFloatingPointType origPoint[3];
                meshReader->GetOutput( )->GetPoint(edgePointId1, origPoint);
                modifiedLocation[0] += newPoint[0] - origPoint[0];
                modifiedLocation[1] += newPoint[1] - origPoint[1];
                modifiedLocation[2] += newPoint[2] - origPoint[2];
                }
              }
            }
          }
        if ( numberOfModifiedNeighbors == 0 )
          {
          done = false;
          }
        else
          {
          modifiedLocation[0] /= static_cast<double> (numberOfModifiedNeighbors * 2); 
          modifiedLocation[1] /= static_cast<double> (numberOfModifiedNeighbors * 2); 
          modifiedLocation[2] /= static_cast<double> (numberOfModifiedNeighbors * 2); 
          vtkFloatingPointType origPoint[3];
          meshReader->GetOutput( )->GetPoint(p, origPoint);
          vtkFloatingPointType newPoint[3];
          newPoint[0] = origPoint[0] + modifiedLocation[0];
          newPoint[1] = origPoint[1] + modifiedLocation[1];
          newPoint[2] = origPoint[2] + modifiedLocation[2];
          mappedPoints->SetPoint(p, newPoint);
          modifiedList->SetId( p, iterations );
          numberOfInteriorPoints++;
          std::cout << "Point: " << p << " Neighbors: " << numberOfModifiedNeighbors << std::endl;
          std::cout << "Orig: " << origPoint[0] << " " << origPoint[1] << " " << origPoint[2] << std::endl;
          std::cout << "New: " << newPoint[0] << " " << newPoint[1] << " " << newPoint[2] << std::endl;
          std::cout << "Mod: " << modifiedLocation[0] << " " << modifiedLocation[1] << " " << modifiedLocation[2] << std::endl;
          }
        }
      }
    } 
  std::cout << "Number of Edge Points: " << numberOfEdgePoints << std::endl;
  std::cout << "Number of Interior Points: " << numberOfInteriorPoints << std::endl;
  std::cout << "Number of Mesh Points: " << numberOfMeshPoints << std::endl;

  int numCorrect= 0;
  int numInCorrect = 0;
 
  for (int c=0; c<numberOfMeshCells; c++)
    {
      cellPoints->Initialize();
      meshReader->GetOutput( )->GetCellPoints(c, cellPoints);
      double x[8][3];
      for (int p=0;p<8;p++)
        {
        double pt[3];
        meshReader->GetOutput( )->GetPoint(cellPoints->GetId(p), pt);
        x[p][0] = pt[0]; x[p][1] = pt[1]; x[p][2] = pt[2];
        }
      if ( CheckHexahedronTopology( x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]) )
        {
        numCorrect++;
        }
      else
        {
        numInCorrect++;
        }
    }
  std::cout << "Number of Cells: " << numberOfMeshCells << std::endl;
  std::cout << "Number of Topology Correct Cells: " << numCorrect << std::endl;
  std::cout << "Number of Topology Incorrect Cells: " << numInCorrect << std::endl; 

  
/********************* End Adjust Interior Nodes *************************/    
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
