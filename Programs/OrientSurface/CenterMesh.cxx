/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: CenterMesh.cxx,v $
  Language:  C++
  Date:      $Date: 2006/09/18 19:25:43 $
  Version:   $Revision: 1.4 $

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


#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>


#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif



//  Register two surface meshes using the Iterative Closest Point Algorithm


int main( int argc, char * argv[] )
{

  
  if( argc != 9 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  Original-STL-Surface  Output-STL-Mesh Xaxis Yaxis Zaxis flipX flipY flipZ" << std::endl;
    return EXIT_FAILURE;
    }

  int Xaxis = atoi(argv[3]);
  int Yaxis = atoi(argv[4]);
  int Zaxis = atoi(argv[5]);
  
  int flipX = atoi(argv[6]);
  int flipY = atoi(argv[7]);
  int flipZ = atoi(argv[8]);
  
  if (flipX != 0) flipX = -1;
  else flipX = 1;
  
  if (flipY != 0) flipY = -1;
  else flipY = 1;
  
  if (flipZ != 0) flipZ = -1;
  else flipZ = 1;
  
  /*** Read the Meshes to Register ***/
  vtkPolyData *originalPolyData;
  if ( (strstr(argv[1], ".stl") != NULL) || (strstr(argv[1], ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(argv[1]);
	fixedReader->Update( );
	originalPolyData = fixedReader->GetOutput();
	}
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName(argv[1]);
	polyReader->Update( );	
    originalPolyData = polyReader->GetOutput();
    }
  //vtkPolyData *originalPolyData = fixedReader->GetOutput();
  vtkPolyData *newPolyData = vtkPolyData::New();
  
  vtkFloatingPointType center[3];
  originalPolyData->GetCenter( &(center[0]) );
  
  vtkTransform *centerTransform = vtkTransform::New();
    centerTransform->Translate(-center[0],-center[1],-center[2]);
	
  vtkTransformPolyDataFilter *resampleMeshFilter = vtkTransformPolyDataFilter::New();
    resampleMeshFilter->SetInput( originalPolyData );
    resampleMeshFilter->SetTransform( centerTransform );
	resampleMeshFilter->Update();


  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->Identity( );
	matrix->SetElement(0, 0, 0);
	matrix->SetElement(0, Xaxis, 1);
	matrix->SetElement(1, 1, 0);
	matrix->SetElement(1, Yaxis, 1);
	matrix->SetElement(2, 2, 0);
	matrix->SetElement(2, Zaxis, 1);
	
  vtkTransform *swapAxisTransform = vtkTransform::New();
    swapAxisTransform->SetMatrix( matrix );
	
  vtkTransformPolyDataFilter *swapAxisMeshFilter = vtkTransformPolyDataFilter::New();
    swapAxisMeshFilter->SetInput( resampleMeshFilter->GetOutput( ) );
    swapAxisMeshFilter->SetTransform( swapAxisTransform );
	swapAxisMeshFilter->Update();

  vtkTransform *flipTransform = vtkTransform::New();
    matrix->Identity( );
	matrix->SetElement(0, 0, (double) flipX);
	matrix->SetElement(1, 1, (double) flipY);
	matrix->SetElement(2, 2, (double) flipZ);	
	flipTransform->SetMatrix( matrix );
		
  vtkTransformPolyDataFilter *flipMeshFilter = vtkTransformPolyDataFilter::New();
    flipMeshFilter->SetInput( swapAxisMeshFilter->GetOutput( ) );
    flipMeshFilter->SetTransform( flipTransform );
	flipMeshFilter->Update();
	
  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
    meshWriter->SetFileName(argv[2]);
	meshWriter->SetFileTypeToASCII();
	meshWriter->SetInput( flipMeshFilter->GetOutput( ) );
	meshWriter->Update( );


  return 0;
}

