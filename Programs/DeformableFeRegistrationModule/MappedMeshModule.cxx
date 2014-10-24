/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: MappedMeshModule.cxx,v $
Language:  C++
Date:      $Date: 2008/04/11 14:41:14 $
Version:   $Revision: 1.2 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <vtkMath.h>
#include <vtkCellArray.h>
#include <vtkLandmarkTransform.h>
#include "vtkPolyData.h"
#include "vtkOBBTree.h"
#include <vtkPointLocator.h>
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkPolyDataWriter.h"
#include "vtkPoints.h"

#include "vtkSTLReader.h"
#include <vtkTransform.h>
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkGeneralTransform.h"
#include "vtkGeometryFilter.h"
#include <vtkTransformFilter.h>
#include "vtkUnstructuredGrid.h"
#include "vtkCleanPolyData.h"
#include "vtkIterativeClosestPointTransform.h"
#include <vtkThinPlateSplineTransform.h>
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkMaskPoints.h"
#include "vtkPointLocator.h"

#include "vtkMimxComputeNormalsFromPolydataFilter.h"

#include "MappedMeshModuleCLP.h"

vtkGeneralTransform* PerformICP ( vtkPolyData *subjectPolyData, vtkUnstructuredGrid *OrigGrid );
vtkUnstructuredGrid* ApplyInitializeTransform(vtkGeneralTransform *initialTransform, vtkUnstructuredGrid * originalGrid);
void WriteIntermediateMesh( vtkUnstructuredGrid* IntermediateMesh, std::string name );
bool IntersectPointNormalWithSurface(double *pointLocation, double *pointNormal, double *distance,
                                     vtkOBBTree *obb, double *projectedPoint);
int main( int argc, char **argv)
{
	PARSE_ARGS;

  if ( debug )
  {
    std::cout << "Mapped Mesh Parameters" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "\tTemplate Mesh filename: " << TemplateMesh << std::endl;
    std::cout << "\tSubject Surface filename: " << SubjectSurface << std::endl;
    std::cout << "\tOutput VTK filename: " << OutputMesh << std::endl;
    std::cout << "\tNumber Of Iterations: " << numberOfIterations << std::endl;
    std::cout << "\tDistance Tolerance: " << tolerance << std::endl;
	  std::cout << "\tLaplacian Smoothing: " << useLaplacianSmoothing << std::endl;
	  std::cout << "\t\tBoundary Smoothing: " << BoundarySmoothingFlag << std::endl;
    std::cout << "\t\tFeature Edge Smoothing: " << FeatureEdgeSmoothingFlag << std::endl;
    std::cout << "\t\tNumber of Smoothing Iterations: " << SmoothingIterations << std::endl;
    std::cout << "\t\tRelaxation Factor: " << RelaxationFactor << std::endl;
    std::cout << "\t\tEdge Angle: " << EdgeAngle << std::endl;
    std::cout << "\t\tFeature Angle: " << FeatureAngle << std::endl;
    std::cout << "\tTPS Point Ratio: " << pointRatio << std::endl;
    std::cout << "\tTPS Random Points: " << randomPointSelection << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
  }

	/* Reading in Source Surface */
	//sphere.stl
  vtkPolyData *subjectPolyData;
	vtkSTLReader *PDReader = vtkSTLReader::New();
	PDReader->SetFileName( SubjectSurface.c_str() );
	PDReader->Update();
	subjectPolyData = PDReader->GetOutput();

	vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet( subjectPolyData );
  obb->SetMaxLevel( 999999 );
  obb->SetNumberOfCellsPerBucket( 999999 );
  obb->SetTolerance( 0.000001 );
  obb->BuildLocator( );

	// Load the template mesh
	vtkUnstructuredGridReader *TemplateMeshReader = vtkUnstructuredGridReader::New();
	TemplateMeshReader->SetFileName( TemplateMesh.c_str() );
	TemplateMeshReader->Update();
	vtkUnstructuredGrid *CurrentMesh = TemplateMeshReader->GetOutput( );

	// Perform ICP to get the common center for template mesh and subject surface
  vtkGeneralTransform *initializeTransform = vtkGeneralTransform::New( );
	initializeTransform = PerformICP( subjectPolyData, CurrentMesh);
	CurrentMesh = ApplyInitializeTransform( initializeTransform, CurrentMesh );

  if ( debug )
  {
  	std::string meshFileName = intermediateMeshFileName + "_ICP.vtk";
    WriteIntermediateMesh( CurrentMesh, meshFileName );
  }

	/* To find closest point from the STL surface*/
  vtkPointLocator *PLocator = vtkPointLocator::New();
  PLocator->SetDataSet( subjectPolyData );
  PLocator->Update();

	vtkMath *math = vtkMath::New();

	double pointOnMesh[3];
	double pointOnSurface[3];
	double targetpoint[3];
	double pointNormal[3];
	vtkPoints *lastNormals = vtkPoints::New();

  /* Extract the surface points for Mapped Mesh registration */
  vtkGeometryFilter *extractSurface = vtkGeometryFilter::New( );
  extractSurface->SetInput( CurrentMesh );
  extractSurface->Update( );

  vtkCleanPolyData *cleanPolyDataFilter = vtkCleanPolyData::New();
  cleanPolyDataFilter->SetInput( extractSurface->GetOutput( ) );
  cleanPolyDataFilter->SetTolerance( 0.0 ); 
  cleanPolyDataFilter->Update( );

  //save the origianl points before starting registration
  vtkPoints *OriginalPoints = vtkPoints::New();
  OriginalPoints->DeepCopy(cleanPolyDataFilter->GetOutput()->GetPoints());
  vtkPoints *meshPoints = vtkPoints::New();

  vtkPolyData *meshSurface = cleanPolyDataFilter->GetOutput();
  
	for ( int j = 0; j <  numberOfIterations; j++ )
	{
    if ( debug )
    {
      std::cout << "Iteration : " << j << std::endl;
    }

    vtkPolyData *deformedMeshSurface;
    if ( useLaplacianSmoothing )
    {
      vtkSmoothPolyDataFilter *smoother = vtkSmoothPolyDataFilter::New();
      smoother->SetInput( meshSurface );
      if ( BoundarySmoothingFlag )
      {
        smoother->BoundarySmoothingOn();
      }
      if ( FeatureEdgeSmoothingFlag )
      {
        smoother->FeatureEdgeSmoothingOn();
        smoother->SetFeatureAngle( FeatureAngle );
      }
      smoother->SetNumberOfIterations( SmoothingIterations );
      smoother->SetRelaxationFactor( RelaxationFactor );
      smoother->SetEdgeAngle( EdgeAngle );
      smoother->Update();
      
      if ( debug )
      {
        char tmpName[32];
        sprintf(tmpName,"_SmoothedSurface%d.vtk",j+1);
        std::string meshFileName = intermediateMeshFileName + tmpName;

        vtkPolyDataWriter *polywriter = vtkPolyDataWriter::New();
        polywriter->SetInput( smoother->GetOutput() );
        polywriter->SetFileName( meshFileName.c_str() );
        polywriter->Update();
      }
      
      deformedMeshSurface = smoother->GetOutput();
    }
    else
    {
      deformedMeshSurface = meshSurface;
    }

    //compute normals of mesh surface
    vtkMimxComputeNormalsFromPolydataFilter *computeNormals = vtkMimxComputeNormalsFromPolydataFilter::New();
    computeNormals->SetInput( deformedMeshSurface );
    computeNormals->Update();
    vtkPointSet *normalsPointSet = computeNormals->GetOutput( );
    vtkPoints *currentNormals = normalsPointSet->GetPoints();

    vtkPoints *movingPoints = deformedMeshSurface->GetPoints( );
    unsigned int Npts = movingPoints->GetNumberOfPoints();
    
    for( unsigned int ptCount = 0; ptCount < Npts; ptCount++ )
    {
      movingPoints->GetPoint(ptCount, pointOnMesh);
      //find the closest point of pointOnMesh on sphere target
      vtkIdType ptId = PLocator->FindClosestPoint( pointOnMesh );
      //find the closest point's location on sphere target
      subjectPolyData->GetPoint( ptId, pointOnSurface );
      //get the distance between pointOnMesh and the closest point
      double distance = pow ( math->Distance2BetweenPoints( pointOnMesh, pointOnSurface ), 0.5 );
      currentNormals->GetPoint(ptCount , pointNormal);


      // Check whether point is outside the surface.
      int isOutside = obb->InsideOrOutside( pointOnMesh ); 
      // If point is outside, make the distance negative so that the normals are flipped.
      if ( isOutside == 1) 
      {
        distance *= -1.0;
      }

      distance = distance * static_cast<double>((j+1))/static_cast<double>(numberOfIterations);

      if ( fabs( distance ) > tolerance )
      {
        //insert the targetpoint into TargetPoints
        //targetpoint is the pointOnMesh moving distance along normal
        targetpoint[0] = pointOnMesh[0] + distance * pointNormal[0];
        targetpoint[1] = pointOnMesh[1] + distance * pointNormal[1];
        targetpoint[2] = pointOnMesh[2] + distance * pointNormal[2];
      }
      else
      {
        targetpoint[0] = pointOnMesh[0];
        targetpoint[1] = pointOnMesh[1];
        targetpoint[2] = pointOnMesh[2];
      }
      movingPoints->SetPoint( ptCount, targetpoint );
    }

    meshSurface->SetPoints( movingPoints );

    //write out the current mesh of each iteration
    if ( debug )
    {
      char tmpName[32];
      sprintf(tmpName,"_Surface%d.vtk",j+1);
      std::string meshFileName = intermediateMeshFileName + tmpName;

      vtkPolyDataWriter *tmpWriter = vtkPolyDataWriter::New();
      tmpWriter->SetFileName( meshFileName.c_str() );
      tmpWriter->SetInput( meshSurface );
      tmpWriter->Update( );
    }
    
	}  //end of iterations

  /* Keep only a portion of the points for TPS Algorithm */
  vtkPoints *maskedTargetPoints;
  vtkPoints *maskedSourcePoints;
  
  if ( pointRatio > 1 )
  {
    vtkMaskPoints *maskDeformedPoints = vtkMaskPoints::New();
    maskDeformedPoints->SetInput( meshSurface );
    maskDeformedPoints->SetOnRatio( pointRatio );
    if ( randomPointSelection )
    {
      maskDeformedPoints->RandomModeOn( );
    }
    maskDeformedPoints->Update(  );
    
    maskedTargetPoints = maskDeformedPoints->GetOutput( )->GetPoints( );


    vtkPointLocator *DefLocator = vtkPointLocator::New( );
    DefLocator->InitPointInsertion( maskedTargetPoints, maskDeformedPoints->GetOutput( )->GetBounds( ) );
    DefLocator->SetDataSet( maskDeformedPoints->GetOutput( ) );
    DefLocator->Update( );
        
    vtkIdList *IdList = vtkIdList::New( );
    IdList->Allocate( maskedTargetPoints->GetNumberOfPoints( ) );
    
    for ( int i = 0; i < meshSurface->GetNumberOfPoints( ); i++ )
    {
      if( DefLocator->IsInsertedPoint( meshSurface->GetPoint( i ) ) != -1 )
      {
        IdList->InsertNextId( i );
      }
    }

    maskedSourcePoints = vtkPoints::New( );
    maskedSourcePoints->SetNumberOfPoints( maskedTargetPoints->GetNumberOfPoints( ) );
    vtkIdType pointId;
    for ( int i = 0; i < IdList->GetNumberOfIds( ); i++ )
    {
      pointId = IdList->GetId( i );
      maskedSourcePoints->SetPoint( i, OriginalPoints->GetPoint( pointId ) );
    }
    
  }
  else
  {
    maskedSourcePoints = OriginalPoints;
    maskedTargetPoints = meshPoints;
  }


	//after the iterations using the target points in the end and the original source point  
	//to generate the thinplate spline transform
	vtkThinPlateSplineTransform *tpsTransform = vtkThinPlateSplineTransform::New();
  tpsTransform->SetBasisToR( );
  tpsTransform->SetSourceLandmarks( maskedSourcePoints );
  tpsTransform->SetTargetLandmarks( maskedTargetPoints );
  tpsTransform->Update( );

	//apply the transform in the mesh
	vtkTransformFilter *tpsTransformUGrid = vtkTransformFilter::New( );
	tpsTransformUGrid->SetInput( CurrentMesh );
  tpsTransformUGrid->SetTransform( tpsTransform );
  tpsTransformUGrid->Update( );

	vtkUnstructuredGrid* afterTPSGrid = vtkUnstructuredGrid::New();
  afterTPSGrid->Allocate( CurrentMesh->GetNumberOfCells( ), CurrentMesh->GetNumberOfCells( ) );
  for ( int i=0; i < CurrentMesh->GetNumberOfCells( ); i++ )
  {
    afterTPSGrid->InsertNextCell( CurrentMesh->GetCellType( i ), CurrentMesh->GetCell( i )->GetPointIds( ) );
  }
  vtkPoints *copyPoints = vtkPoints::New();
  copyPoints->Allocate(  tpsTransformUGrid->GetOutput( )->GetPoints( )->GetNumberOfPoints(),  
	tpsTransformUGrid->GetOutput( )->GetPoints( )->GetNumberOfPoints() );
  copyPoints->DeepCopy( tpsTransformUGrid->GetOutput( )->GetPoints() );
  afterTPSGrid->SetPoints( copyPoints );

  WriteIntermediateMesh( afterTPSGrid, OutputMesh );

	return EXIT_SUCCESS;
 
}

vtkGeneralTransform* PerformICP( vtkPolyData *subjectPolyData, vtkUnstructuredGrid *OrigGrid)
{

  vtkUnstructuredGrid* icpGrid = vtkUnstructuredGrid::New();

  icpGrid->Allocate( OrigGrid->GetNumberOfCells( ), OrigGrid->GetNumberOfCells( ) );

  for ( int i=0; i < OrigGrid->GetNumberOfCells( ); i++ )
  {
    icpGrid->InsertNextCell( OrigGrid->GetCellType( i ), OrigGrid->GetCell( i )->GetPointIds( ) );
  }

  icpGrid->SetPoints( OrigGrid->GetPoints() );

  /* Extract the surface points for ICP registration */
  vtkGeometryFilter *extractSurface = vtkGeometryFilter::New( );
  extractSurface->SetInput( OrigGrid );
  extractSurface->Update( );
  
  vtkCleanPolyData *cleanPolyDataFilter = vtkCleanPolyData::New();
  cleanPolyDataFilter->SetInput( extractSurface->GetOutput( ) );
  cleanPolyDataFilter->SetTolerance( 0.0 ); 
  cleanPolyDataFilter->Update( );
  

  vtkIterativeClosestPointTransform *icpTransform = vtkIterativeClosestPointTransform::New();
  icpTransform->SetSource( cleanPolyDataFilter->GetOutput( ) );
  icpTransform->SetTarget( subjectPolyData );
  icpTransform->StartByMatchingCentroidsOn( );
  icpTransform->GetLandmarkTransform( )->SetModeToRigidBody( );
  icpTransform->SetMaximumNumberOfIterations( 1000 );
  icpTransform->CheckMeanDistanceOn();
  icpTransform->SetMaximumMeanDistance( 0.001 );
  icpTransform->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
  icpTransform->SetMaximumNumberOfLandmarks( 10000 );
  icpTransform->Update( );

  /*Added*/
  vtkTransformFilter *initialTransformUgrid = vtkTransformFilter::New( );
  initialTransformUgrid->SetInput( icpGrid );
  initialTransformUgrid->SetTransform( icpTransform );
  initialTransformUgrid->Update( );

  vtkUnstructuredGrid* qFinalGrid = vtkUnstructuredGrid::New();
  qFinalGrid->Allocate( icpGrid->GetNumberOfCells( ), icpGrid->GetNumberOfCells( ) );
  for ( int i = 0; i < icpGrid->GetNumberOfCells( ); i++ )
  {
    qFinalGrid->InsertNextCell( icpGrid->GetCellType( i ), icpGrid->GetCell( i )->GetPointIds( ) );
  }
  qFinalGrid->SetPoints( initialTransformUgrid->GetOutput( )->GetPoints( ) );
  /* End Added */

  /* Scale Based on Bounding Box Size */
  double templateBounds[6];
  qFinalGrid->GetBounds(templateBounds);

  double subjectBounds[6];
  subjectPolyData->GetBounds(subjectBounds);

  vtkMatrix4x4 *sMatrix = vtkMatrix4x4::New();
  sMatrix->Identity( );
  sMatrix->SetElement(0, 0, (subjectBounds[1]-subjectBounds[0])/(templateBounds[1]-templateBounds[0]));
  sMatrix->SetElement(1, 1, (subjectBounds[3]-subjectBounds[2])/(templateBounds[3]-templateBounds[2]));
  sMatrix->SetElement(2, 2, (subjectBounds[5]-subjectBounds[4])/(templateBounds[5]-templateBounds[4]));
  
  vtkMatrix4x4 *t1Matrix = vtkMatrix4x4::New();
  t1Matrix->Identity( );
  t1Matrix->SetElement(0, 3, -((templateBounds[1]+templateBounds[0])/2) );
  t1Matrix->SetElement(1, 3, -((templateBounds[2]+templateBounds[3])/2) );
  t1Matrix->SetElement(2, 3, -((templateBounds[4]+templateBounds[5])/2) );

  vtkMatrix4x4 *t2Matrix = vtkMatrix4x4::New();
  t2Matrix->Identity( );
  t2Matrix->SetElement(0, 3, ((templateBounds[1]+templateBounds[0])/2) );
  t2Matrix->SetElement(1, 3, ((templateBounds[2]+templateBounds[3])/2) );
  t2Matrix->SetElement(2, 3, ((templateBounds[4]+templateBounds[5])/2) );
  
  vtkTransform *scaleTransform = vtkTransform::New();
  scaleTransform->SetMatrix( sMatrix );
    
  vtkTransformFilter *scaleTransformUgrid = vtkTransformFilter::New( );
  scaleTransformUgrid->SetInput( qFinalGrid );
  scaleTransformUgrid->SetTransform( scaleTransform );
  scaleTransformUgrid->Update( );
  
  vtkUnstructuredGrid *sFinalGrid = vtkUnstructuredGrid::New( );
  sFinalGrid->Allocate( qFinalGrid->GetNumberOfCells( ), qFinalGrid->GetNumberOfCells( ) );
  for ( int i=0; i < qFinalGrid->GetNumberOfCells( ); i++ )
  {
    sFinalGrid->InsertNextCell( qFinalGrid->GetCellType( i ), qFinalGrid->GetCell( i )->GetPointIds( ) );
  }
  sFinalGrid->SetPoints( qFinalGrid->GetPoints() );
  
  
  vtkGeometryFilter *extract = vtkGeometryFilter::New( );
  extract->SetInput( sFinalGrid );
  extract->Update( );

  vtkCleanPolyData *cleanFinalPolyDataFilter = vtkCleanPolyData::New();
  cleanFinalPolyDataFilter->SetInput( extract->GetOutput( ) );
  cleanFinalPolyDataFilter->SetTolerance( 0.0 ); 
  cleanFinalPolyDataFilter->Update( );

  vtkIterativeClosestPointTransform *icpTransform2 = vtkIterativeClosestPointTransform::New();
  icpTransform2->SetSource( cleanFinalPolyDataFilter->GetOutput( ) );
  icpTransform2->SetTarget( subjectPolyData );
  icpTransform2->StartByMatchingCentroidsOn( );
  icpTransform2->GetLandmarkTransform( )->SetModeToRigidBody( );
  icpTransform2->SetMaximumNumberOfIterations( 1000 );
  icpTransform2->CheckMeanDistanceOn();
  icpTransform2->SetMaximumMeanDistance( 0.001 );
  icpTransform2->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
  icpTransform2->SetMaximumNumberOfLandmarks( 10000 );
  icpTransform2->Update( );
  
  
  vtkGeneralTransform *finalTransform = vtkGeneralTransform::New();
  finalTransform->PreMultiply( );
  finalTransform->Concatenate( icpTransform2);
  finalTransform->Concatenate( t2Matrix );
  finalTransform->Concatenate( sMatrix );
  finalTransform->Concatenate( t1Matrix );
  finalTransform->Concatenate( icpTransform );
    
  return ( finalTransform );

}


vtkUnstructuredGrid* ApplyInitializeTransform(vtkGeneralTransform *initialTransform, vtkUnstructuredGrid * originalGrid)
{
  vtkTransformFilter *initialTransformUgrid = vtkTransformFilter::New( );
  initialTransformUgrid->SetInput( originalGrid );
  initialTransformUgrid->SetTransform( initialTransform );
  initialTransformUgrid->Update( );

  vtkUnstructuredGrid* finalGrid = vtkUnstructuredGrid::New();
  finalGrid->Allocate( originalGrid->GetNumberOfCells( ), originalGrid->GetNumberOfCells( ) );
  for ( int i=0; i < originalGrid->GetNumberOfCells( ); i++ )
  {
    finalGrid->InsertNextCell( originalGrid->GetCellType( i ), originalGrid->GetCell( i )->GetPointIds( ) );
  }
  finalGrid->SetPoints( initialTransformUgrid->GetOutput( )->GetPoints() );

  return ( finalGrid );
}


void WriteIntermediateMesh( vtkUnstructuredGrid* IntermediateMesh, std::string name )
{
  vtkUnstructuredGridWriter *Writer = vtkUnstructuredGridWriter::New( );
  Writer->SetInput( IntermediateMesh );
  Writer->SetFileName( name.c_str( ) );
  Writer->Update( );
  Writer->Delete( );
}

bool IntersectPointNormalWithSurface(double *pointLocation, double *pointNormal, double *distance,
                                     vtkOBBTree *obb, double *projectedPoint)
{
  double a0[3];
  double a1[3];


  a0[0] = pointLocation[0] - 2 * (*distance) * pointNormal[0];
  a0[1] = pointLocation[1] - 2 * (*distance) * pointNormal[1];
  a0[2] = pointLocation[2] - 2 * (*distance) * pointNormal[2];

  a1[0] = pointLocation[0] + 2 * (*distance) * pointNormal[0];
  a1[1] = pointLocation[1] + 2 * (*distance) * pointNormal[1];
  a1[2] = pointLocation[2] + 2 * (*distance) * pointNormal[2];


  vtkIdList *intersectionIds = vtkIdList::New();
  vtkPoints *intersectionPoints = vtkPoints::New();
  vtkMath *math = vtkMath::New(); 

  int lineIntersects = obb->IntersectWithLine(a0, a1, intersectionPoints, intersectionIds);
  if ( lineIntersects != 0 )
  {
    double minDistance = 10e20;
    vtkIdType minPointId = 0;
    for (int i=0;i<intersectionPoints->GetNumberOfPoints();i++)
    {
      double currentDistance = pow ( math->Distance2BetweenPoints( pointLocation, intersectionPoints->GetPoint(i) ), 0.5 ); 
      if ( currentDistance < minDistance)
      {
        minDistance = currentDistance;
        minPointId = i;
      }
    }
    
    projectedPoint[0] = intersectionPoints->GetPoint(minPointId)[0];
    projectedPoint[1] = intersectionPoints->GetPoint(minPointId)[1];
    projectedPoint[2] = intersectionPoints->GetPoint(minPointId)[2];

    *distance = minDistance;
    return true;
  }
  else
  {
    return false;
  }
  
  
}
