/*=========================================================================
 *
 *   Program:   IaFeMesh
 *   Module:    $RCSfile: TestGridToMesh.cxx,v $
 *   Language:  C++
 *   Date:      $Date: 2007/05/17 17:09:46 $
 *   Version:   $Revision: 1.8 $
 *
 *   Copyright (c) Nicole Grosland. All rights reserved.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notices for more information.
 *
 *=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif


#include "itkImage.h"
#include "itkMesh.h"
#include "itkVertexCell.h"
#include "itkMimxImageToHexahedronMeshSource.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "itkAutomaticTopologyMeshSource.h"


#include "vtkUnstructuredGridToitkMesh.h"

int TestTetGridToMesh( char *defaultDir );



int main( int argc, char * argv[] )
{  

  TestTetGridToMesh( argv[0] );
  
  
  return EXIT_SUCCESS;

 }


int TestTetGridToMesh( char *defaultDir )
{

  typedef itk::DefaultStaticMeshTraits<double, 3, 3,float,float> MeshTraits;
  typedef itk::Mesh<double,3, MeshTraits>                           MeshType;
  typedef MeshType::PointType                                       PointType;
  typedef MeshType::PointsContainer                                 InputPointsContainer;
  typedef InputPointsContainer::Pointer                             InputPointsContainerPointer;
  typedef InputPointsContainer::Iterator                            InputPointsContainerIterator;
  
  
  vtkPoints *Points = vtkPoints::New();
  vtkUnstructuredGrid *Grid = vtkUnstructuredGrid::New();
  Points->SetNumberOfPoints( 4 );
  
  
  double vpoint[3];
  vpoint[0]= 0.0;
  vpoint[1]= 0.0;
  vpoint[2]= 0.0;
  Points->SetPoint(0,vpoint);
  
  vpoint[0]= 1.0;
  vpoint[1]= 0.0;
  vpoint[2]= 0.0;
  Points->SetPoint(1,vpoint);
  
  vpoint[0]= 1.0;
  vpoint[1]= 1.0;
  vpoint[2]= 0.0;
  Points->SetPoint(2,vpoint);
  
  vpoint[0]= 1.0;
  vpoint[1]= 1.0;
  vpoint[2]= 1.0;
  Points->SetPoint(3,vpoint);
  
  vtkIdType pts[4];
  pts[0] = 0; pts[1] = 1; pts[2] = 2; pts[3] = 3; 
  Grid->InsertNextCell(VTK_TETRA, 4, pts);
  
  typedef vtkUnstructuredGridToitkMesh GridToMeshType;
  GridToMeshType *toItkFilter = new GridToMeshType;
  
  toItkFilter->SetInput( Grid );
  MeshType::Pointer mesh = toItkFilter->GetOutput();
  
  int numPoints =  mesh->GetNumberOfPoints();
  if (numPoints != 4)
    {
    std::cout << "\tError: Invalid number of Points in ITK Tet Mesh" << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef MeshType::CellsContainerPointer                           CellsContainerPointer;
  typedef MeshType::CellsContainerIterator                          CellsContainerIterator;   
  InputPointsContainerPointer      myPoints = mesh->GetPoints();
  InputPointsContainerIterator     points = myPoints->Begin();
  PointType point;
  int i=0;
  while( points != myPoints->End() ) 
    {   
    point = points.Value();
    if (i==0)
      {
      if ((point[0] != 0.0) || (point[1] != 0.0) || (point[2] != 0.0))
        {
        std::cout << "\tError: Invalid Tet Point Index 0" << std::endl;
        return EXIT_FAILURE;
        }
      }
    if (i==1)
      {
      if ((point[0] != 1.0) || (point[1] != 0.0) || (point[2] != 0.0))
        {
        std::cout << "\tError: Invalid Tet Point Index 1" << std::endl;
        return EXIT_FAILURE;
        }
      }
    if (i==2)
      {
      if ((point[0] != 1.0) || (point[1] != 1.0) || (point[2] != 0.0))
        {
        std::cout << "\tError: Invalid Tet Point Index 2" << std::endl;
        return EXIT_FAILURE;
        }
      }
    if (i==3)
      {
      if ((point[0] != 1.0) || (point[1] != 1.0) || (point[2] != 1.0))
        {
        std::cout << "\tError: Invalid Tet Point Index 3" << std::endl;
        return EXIT_FAILURE;
        }
      }
      i++;
      points++;
    }
  std::cout << "\tChecked VTK Tet Grid To ITK Mesh" << std::endl;
 
  return EXIT_SUCCESS;
}
