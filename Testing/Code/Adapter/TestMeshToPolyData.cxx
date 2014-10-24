/*=========================================================================
 *
 *   Program:   IaFeMesh
 *   Module:    $RCSfile: TestMeshToPolyData.cxx,v $
 *   Language:  C++
 *   Date:      $Date: 2006/10/19 02:57:06 $
 *   Version:   $Revision: 1.3 $
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
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "itkAutomaticTopologyMeshSource.h"

#include "itkMeshTovtkPolyData.h"

int main( int argc, char * argv[] )
{  

  std::cout << "TestMeshToPolyData:" << std::endl;
  
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> MeshTraits;
  typedef itk::Mesh<double,3, MeshTraits>                           MeshType;
  typedef MeshType::PointType                                       PointType;
  typedef itk::AutomaticTopologyMeshSource<MeshType> AutoMeshType;
  AutoMeshType::Pointer triSource = AutoMeshType::New();
	
  	  
  triSource->AddTriangle (   	
		  triSource->AddPoint(0.0, 0.0, 0.0),
		  triSource->AddPoint(1.0, 0.0, 0.0),
		  triSource->AddPoint(1.0, 1.0, 0.0));
  
  std::cout << "\t Create Triangle Mesh" << std::endl;
  
  try
    {
    triSource->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown during Update() " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }
    
  
  // Transform to VTK and write out
  typedef itkMeshTovtkPolyData MeshToVtkType;
  MeshToVtkType *toVtkFilter = new MeshToVtkType;
  
  MeshType::Pointer myMesh = triSource->GetOutput();
  toVtkFilter->SetInput( myMesh );
  
  vtkPolyData *polyData = toVtkFilter->GetOutput();
  
  int numberOfPoints = polyData->GetNumberOfPoints();
  
  std::cout << "\t Number of Points: " << numberOfPoints << std::endl;
  
  if ( numberOfPoints != 3 )
    {
    std::cerr << "Mesh shows "
              << numberOfPoints
              << " points, but 3 were added." << std::endl;
    return EXIT_FAILURE;
    
    }
  
  int numberOfCells = polyData->GetNumberOfCells();
  
  std::cout << "\t Number of Cells: " << numberOfCells << std::endl;
  
  if ( numberOfCells != 1 )
    {
    std::cerr << "Mesh shows "
              << numberOfCells
              << " points, but 1 was added." << std::endl;
    return EXIT_FAILURE;
    
    }
     
  return EXIT_SUCCESS;

 }
