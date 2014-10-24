/*=========================================================================
 *
 *   Program:   IaFeMesh
 *   Module:    $RCSfile: TestMeshToGrid.cxx,v $
 *   Language:  C++
 *   Date:      $Date: 2006/10/19 02:57:06 $
 *   Version:   $Revision: 1.6 $
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

#include "vtkPoints.h"
#include "vtkHexahedron.h"
#include "vtkTetra.h"
#include "itkImage.h"
#include "itkMesh.h"
#include "itkVertexCell.h"
#include "itkImageToHexahedronMeshSource.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "itkAutomaticTopologyMeshSource.h"

#include "itkMeshTovtkUnstructuredGrid.h"


int TestHexMeshToGrid( char *defaultDir );
int TestTetMeshToGrid();



int main( int argc, char * argv[] )
{  

  std::cout << "TestMeshToGrid:" << std::endl;
  
  TestHexMeshToGrid( argv[1] );
  TestTetMeshToGrid( );
   
  
  return EXIT_SUCCESS;

 }


/**********************************************************
 *
 * Test Hexahedral to vtkUnstructuredGrid Conversion
 *
 **********************************************************/
 
 
int TestHexMeshToGrid( char *defaultDir )
{

  typedef    signed short    PixelType;
  
  typedef itk::Image< PixelType,  3 >   ImageType;

  ImageType::Pointer image = ImageType::New();
  
  const ImageType::SizeType size = {{10,10,10}};
  const ImageType::IndexType index = {{0,0,0}};
  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  image->SetLargestPossibleRegion( region );
  image->SetBufferedRegion( region );
  image->SetRequestedRegion( region );
  image->Allocate();
  
  for (int x=0;x<1;x++)
  {
	  for (int y=0;y<2;y++)
	  {
		  for (int z=0;z<3;z++)
		  {
			  ImageType::IndexType tmpIndex;
			  tmpIndex[0] = x; tmpIndex[1] = y; tmpIndex[2] = z;
			  image->SetPixel(tmpIndex, 1);
		  }
	  }
  }
  
   
  typedef itk::Mesh< double > MeshType;  
  typedef itk::ImageToHexahedronMeshSource<ImageType, 
											MeshType> ImageToMeshSourceType;
  
  ImageToMeshSourceType::Pointer meshSource = ImageToMeshSourceType::New();

  meshSource->SetInput( image );
  meshSource->Update( );
  std::cout << "\t Create Mesh" << std::endl;
  MeshType::Pointer myMesh = meshSource->GetOutput( );

  // Transform to VTK and write out

  typedef itkMeshTovtkUnstructuredGrid<MeshType> MeshToVtkType;
  MeshToVtkType *toVtkFilter = new MeshToVtkType;
  
  toVtkFilter->SetInput(myMesh);
  
  std::string OutputFile = defaultDir;
  OutputFile += "/testItkMeshToVtkUgrid.vtk";
  
  vtkUnstructuredGrid *tetGrid = toVtkFilter->GetOutput();
  
  vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
  writer->SetInput( tetGrid );
  writer->SetFileName( OutputFile.c_str() );
  writer->SetFileTypeToASCII( );
  writer->Update();
  
  int numCells = tetGrid->GetNumberOfCells();  
  if ( numCells != 6 )
    {
    std::cout << "Error: Invalid number of Cells in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    }
  
  int numPoints = tetGrid->GetNumberOfPoints();
  if ( numPoints != 24 )
    {
    std::cout << "Error: Invalid number of Points in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    } 
  
  
  /* Test Cells and Points */
  
  
}



/**********************************************************
 *
 * Test Tetrahedral to vtkUnstructuredGrid Conversion
 *
 **********************************************************/
 
int TestTetMeshToGrid()
{

  typedef itk::Mesh< double > MeshType;  
  typedef itk::AutomaticTopologyMeshSource<MeshType> AutoMeshType;
  AutoMeshType::Pointer tetSource = AutoMeshType::New();
		  
  tetSource->AddTetrahedron (   	
		  tetSource->AddPoint(0.0, 0.0, 0.0),
		  tetSource->AddPoint(1.0, 0.0, 0.0),
		  tetSource->AddPoint(1.0, 1.0, 0.0),
		  tetSource->AddPoint(1.0, 1.0, 1.0));
  
  std::cout << "\t Create Tet Mesh" << std::endl;
  
  typedef itkMeshTovtkUnstructuredGrid<MeshType> MeshToVtkType;
  MeshToVtkType *tetToVtkFilter = new MeshToVtkType;
  tetToVtkFilter->SetInput(tetSource->GetOutput());
  
  std::cout << "\t Convert to Tetraharal Mesh VTK Unstructured Grid" << std::endl;
  
  vtkUnstructuredGrid *tetGrid = tetToVtkFilter->GetOutput();
  
  
  int numCells = tetGrid->GetNumberOfCells();
  if ( numCells != 1 )
    {
    std::cout << "Error: Invalid number of Cells in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    }
  
  int numPoints = tetGrid->GetNumberOfPoints();
  if ( numPoints != 4 )
    {
    std::cout << "Error: Invalid number of Points in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    }  
  
  double point[3];  
  tetGrid->GetPoint(0, point);
  if ((point[0] != 1.0) || (point[1] != 1.0) || (point[2] != 1.0))
    {
    std::cout << "Error: Invalid point 0 in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    }
  tetGrid->GetPoint(1, point);
  if ((point[0] != 1.0) || (point[1] != 1.0) || (point[2] != 0.0))
    {
    std::cout << "Error: Invalid point 1 in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    }
  tetGrid->GetPoint(2, point);
  if ((point[0] != 1.0) || (point[1] != 0.0) || (point[2] != 0.0))
    {
    std::cout << "Error: Invalid point 2 in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    }
  tetGrid->GetPoint(3, point);
  if ((point[0] != 0.0) || (point[1] != 0.0) || (point[2] != 0.0))
    {
    std::cout << "Error: Invalid point 3 in vtkUnstructuredGrid" << std::endl;
    return EXIT_FAILURE;
    }
  
  /* Probably should Test Cell as well */
 
  return EXIT_SUCCESS;
}

