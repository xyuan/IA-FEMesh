/*=========================================================================

  Program:   IaFeMesh
  Module:    $RCSfile: MainitkCurveToStructuredPlanarMesh.cxx,v $
  Language:  C++
  Date:      $Date: 2006/10/15 23:56:43 $
  Version:   $Revision: 1.2 $

  Copyright (c) Nicole Grosland. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "vtkSTLReader.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkLookupTable.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkCellLocator.h"
#include "vtkPointLocator.h"
#include "vtkCellArray.h"
#include "itkEuclideanDistancePointMetric.h"
#include "itkPointSet.h"
#include "itkArray.h"
#include "itkImageToMeshFilter.h"
#include "itkMeshToMeshFilter.h"
#include "itkPolygon3DSpatialObject.h"
#include "itkCurveToStructuredPlanarMesh.h"
#include "itkMesh.h"

int main(int argc, char * argv [])
{
  if( argc != 5 )
    {
    std::cerr << argv[0]
              << " curve-1 curve-2 curve-3 curve-4"
              << std::endl;
    return EXIT_FAILURE;
    }


	typedef itk::Polygon3DSpatialObject<3>	CurveType3D;	
	CurveType3D::Pointer  Curve1 = CurveType3D::New();

	std::ifstream FileInput1;
	try 
    {
    FileInput1.open(argv[1], std::ios::in);
    }
  catch (...)
    {
    std::cout << "Error: Failed to open Curve 1 file " << argv[1] << std::endl;
    return EXIT_FAILURE;
    }

	CurveType3D::PointType pointtype;
	do {
		if(FileInput1 >> pointtype[0]>>pointtype[1]>>pointtype[2])
		Curve1->AddPoint(pointtype);
	} while(!FileInput1.eof());
  FileInput1.close();

	CurveType3D::Pointer  Curve2 = CurveType3D::New();

	std::ifstream FileInput2;
	try 
    {
	  FileInput2.open(argv[2], std::ios::in);
    }
  catch (...)
    {
    std::cout << "Error: Failed to open Curve 2 file " << argv[2] << std::endl;
    return EXIT_FAILURE;
    }

	do {
		if(FileInput2 >> pointtype[0]>>pointtype[1]>>pointtype[2])
		Curve2->AddPoint(pointtype);
	} while(!FileInput2.eof());
  FileInput2.close();

	CurveType3D::Pointer  Curve3 = CurveType3D::New();

	std::ifstream FileInput3;
	try 
    {
	 	FileInput3.open(argv[3], std::ios::in);
    }
  catch (...)
    {
    std::cout << "Error: Failed to open Curve 3 file " << argv[3] << std::endl;
    return EXIT_FAILURE;
    }


	do {
		if(FileInput3 >> pointtype[0]>>pointtype[1]>>pointtype[2])
		Curve3->AddPoint(pointtype);
	} while(!FileInput3.eof());
  FileInput3.close();

	CurveType3D::Pointer  Curve4 = CurveType3D::New();

	std::ifstream FileInput4;
	try 
    {
	 	FileInput4.open(argv[4], std::ios::in);
    }
  catch (...)
    {
    std::cout << "Error: Failed to open Curve 4 file " << argv[4] << std::endl;
    return EXIT_FAILURE;
    }


	do {
		if(FileInput4 >> pointtype[0]>>pointtype[1]>>pointtype[2])
			Curve4->AddPoint(pointtype);
	} while(!FileInput4.eof());
  FileInput4.close();

	typedef itk::Mesh<float,3> MeshType;
	itk::CurveToStructuredPlanarMesh<CurveType3D,MeshType>::Pointer 
	CurveToStructuredPlanarMeshPointer = itk::CurveToStructuredPlanarMesh<CurveType3D,MeshType>::New();
	CurveToStructuredPlanarMeshPointer->SetCurve(1,Curve1);
	CurveToStructuredPlanarMeshPointer->SetCurve(2,Curve2);
	CurveToStructuredPlanarMeshPointer->SetCurve(3,Curve3);
	CurveToStructuredPlanarMeshPointer->SetCurve(4,Curve4);
	try 
    {
    CurveToStructuredPlanarMeshPointer->GenerateQuadrilateralMesh();
    }
  catch (...)
    {
    std::cout << "Error: CurveToStructuredPlanarMesh threw an exception" << std::endl;
    return EXIT_FAILURE;
    } 


  return EXIT_SUCCESS;

}






