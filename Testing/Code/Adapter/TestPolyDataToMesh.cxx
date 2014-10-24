/*=========================================================================
 *
 *   Program:   IaFeMesh
 *   Module:    $RCSfile: TestPolyDataToMesh.cxx,v $
 *   Language:  C++
 *   Date:      $Date: 2006/10/19 02:57:06 $
 *   Version:   $Revision: 1.2 $
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
#include "itkImageToHexahedronMeshSource.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "itkAutomaticTopologyMeshSource.h"
#include <vtkImageImport.h>

#include "vtkPolyDataToitkMesh.h"

int main( int argc, char * argv[] )
{  

  std::cout << "TestPolyDataToMesh:" << std::endl;
  
  vtkPoints *Points = vtkPoints::New();
  Points->SetNumberOfPoints(3);
  
  double vpoint[3];
  vpoint[0]= 0.0; vpoint[1]= 0.0; vpoint[2]= 0.0;
  Points->SetPoint(0,vpoint);
  vpoint[0]= 1.0; vpoint[1]= 0.0; vpoint[2]= 0.0;
  Points->SetPoint(1,vpoint);
  vpoint[0]= 1.0; vpoint[1]= 1.0; vpoint[2]= 0.0;
  Points->SetPoint(2,vpoint);
  std::cout << "\tSet Point Data: " << std::endl;  
  
  vtkPolyData *PolyData = vtkPolyData::New();  
  PolyData->SetPoints(Points);
  Points->Delete();
  
  vtkCellArray *Polys = vtkCellArray::New();
  vtkIdType pts[3];
  pts[0]=0; pts[1]=1; pts[2]=2;
  Polys->InsertNextCell(3,pts);
  std::cout << "\tInsert Cell: " << std::endl;  
  
  PolyData->SetPolys(Polys);
  std::cout << "\tSet Polys: " << std::endl;  
  Polys->Delete();
  
  std::cout << "\tInitialize VTK Poly Data: " << std::endl;
  


  // Transform to ITK
  typedef vtkPolyDataToitkMesh MeshToItkType;
  MeshToItkType *MeshToItk = new MeshToItkType;
  
  MeshToItk->SetInput( PolyData );
  
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> MeshTraits;
  typedef itk::Mesh<double,3, MeshTraits>                           MeshType;
  typedef MeshType::PointType                                       PointType;
  typedef MeshType::PointsContainer                                 InputPointsContainer;
  typedef InputPointsContainer::Pointer                             InputPointsContainerPointer;
  typedef InputPointsContainer::Iterator                            InputPointsContainerIterator;
  
  //MeshToItk->Update();
  
  MeshType::Pointer mesh = MeshToItk->GetOutput( );
  
  std::cout << "\tConvert to ITK Poly Data: " << std::endl;
  
  int numPoints =  mesh->GetNumberOfPoints();
  std::cout << "\tNumber of Points: " << numPoints << std::endl;
  
  if (numPoints != 3)
    {
    std::cout << "\tError: Invalid number of Points in ITK Triangle Mesh" << std::endl;
    return EXIT_FAILURE;
    }
    
  int numCells =  mesh->GetNumberOfCells();
  std::cout << "\tNumber of Cells: " << numCells << std::endl;
  
  if (numCells != 1)
    {
    std::cout << "\tError: Invalid number of Cells in ITK Triangle Mesh" << std::endl;
    return EXIT_FAILURE;
    }
     
  return EXIT_SUCCESS;

 }
