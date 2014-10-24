#include "vtkPolyData.h"
#include "vtkSTLReader.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkComputeNormalsFromPolydataFilter.h"
#include "vtkAbaqusFileWriter.h"
#include "vtkGeometryFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkPointLocator.h"
#include "vtkOBBTree.h"
#include "vtkMath.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkSTLWriter.h"

int main( )
{

  vtkUnstructuredGridReader *UGridReader = vtkUnstructuredGridReader::New();
  UGridReader->SetFileName( "UnstructuredGrid.vtk" );
  UGridReader->Update();
  vtkUnstructuredGrid *UGrid = UGridReader->GetOutput( );

  int numOfPoints = UGrid->GetNumberOfPoints();

  vtkSTLReader *PDReader = vtkSTLReader::New();
  PDReader->SetFileName( "Subject.stl" );
  PDReader->Update();
  vtkPolyData *subPolyData = PDReader->GetOutput();

  vtkPointLocator *PLocator = vtkPointLocator::New();
  PLocator->SetDataSet( subPolyData );
  PLocator->Update();
  
  /* Geometry Filter to convert Unstructured Grid to Surface Mesh ONLY for COMPUTING NORMALS */
  vtkGeometryFilter *GeometryFilter = vtkGeometryFilter::New();
  GeometryFilter->SetInput( UGrid );
  GeometryFilter->Update( );
  vtkPolyData *PolyData = GeometryFilter->GetOutput( );

  /*vtkIdList *cellPoints = vtkIdList::New();
  vtkIdType ptId;
  vtkIdType cellId;
  PolyData->GetCellPoints(cellId, cellPoints);*/

  /* Computing Normals */  
  vtkComputeNormalsFromPolydataFilter *computeNormals = vtkComputeNormalsFromPolydataFilter::New();
  computeNormals->SetInput(PolyData);
  computeNormals->Update();
  vtkPointSet *normalsPointSet = computeNormals->GetOutput( );
  vtkPoints *finalNormals = normalsPointSet->GetPoints();

  /* Projecting ONLY the surface points onto the subject surface */

  double uGridPt[3];
  double pDatapt[3];
  vtkIdType ptId;
  double pointNormal[3];
  for ( int i = 0; i < numOfPoints; i++ )
  {
    finalNormals->GetPoint( i , pointNormal);
    if ( pointNormal[0] != -2 )
    {
      UGrid->GetPoint( i, uGridPt );
      ptId = PLocator->FindClosestPoint( uGridPt );
      subPolyData->GetPoint( ptId, pDatapt );
      UGrid->GetPoints()->SetPoint( i, pDatapt );
    }
  }

/* Writing out the final Unstructured Grid in Abaqus File Format */ 
  vtkAbaqusFileWriter *AbqWriter = vtkAbaqusFileWriter::New();
  AbqWriter->SetFileName( "Abaqus.inp" );
  AbqWriter->SetModulus( 300000 );
  AbqWriter->SetPoissonsRatio( 0.4 );
  AbqWriter->SetInput( UGrid );
  AbqWriter->Update( );

  /* Writing out the Unstructured Grid for final Visualization */
  vtkUnstructuredGridWriter *UnstGridWriter = vtkUnstructuredGridWriter::New();
  UnstGridWriter->SetInput(UGrid);
  UnstGridWriter->SetFileName("DefUnstructuredGrid.vtk");
  UnstGridWriter->Update();

  /* Converting the PolyData to STL format & writing it out for final analysis */
  vtkTriangleFilter *triangle = vtkTriangleFilter::New();
  triangle->SetInput( PolyData );
  triangle->Update();

  vtkSTLWriter *stlwriter = vtkSTLWriter::New();
  stlwriter->SetInput( triangle->GetOutput( ) );
  stlwriter->SetFileName( "DefSTL.stl");
  stlwriter->Update();

  return EXIT_SUCCESS;
}