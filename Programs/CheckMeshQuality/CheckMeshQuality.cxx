/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: CheckMeshQuality.cxx,v $
  Language:  C++
  Date:      $Date: 2007/04/16 18:26:48 $
  Version:   $Revision: 1.2 $

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
#include <vtkMeshQuality.h>
#include <vtkFieldData.h>

#include <metaCommand.h>
#include <itkImage.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkThinPlateSplineKernelTransform.h>
#include <itkPoint.h>
#include <itkPointSet.h>



#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

const unsigned int imageDimension = 3;
typedef itk::Image<float, imageDimension>  DistanceImageType;
typedef DistanceImageType::PointType       ImagePointType;

float EucledianDistance (double point[3], ImagePointType imageLoc);


//  Register two surface meshes using the Iterative Closest Point Algorithm


int main( int argc, char * argv[] )
{

  MetaCommand command;

  command.SetOption("InputMesh","m",false,"Input Mesh File");
  command.AddOptionField("InputMesh","filename",MetaCommand::STRING,true);
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string InputMeshFilename (command.GetValueAsString("InputMesh","filename"));
      
  std::cout << "Input Mesh: " <<  InputMeshFilename << std::endl; 
  
  /* Read in the Canonical Mesh, Apply Transform and Write out */ 
  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
    meshReader->SetFileName( InputMeshFilename.c_str() );
    meshReader->Update();  

  /* Compute Mesh Quality */
  vtkMeshQuality *meshQualityFilter = vtkMeshQuality::New();
    meshQualityFilter->SetInput( meshReader->GetOutput( ) );
    meshQualityFilter->SetHexQualityMeasureToEdgeRatio( );
    meshQualityFilter->Update( );
  
  std::cout << "Edge Ratio: " << std::endl;
  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Average: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 1 ); 
  std::cout << std::endl;
  std::cout << "Minimum: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 0 ); 
  std::cout << std::endl;
  std::cout << "Maximum: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 2 ); 
  std::cout << std::endl;
  std::cout << "Variance: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 3 ); 
  std::cout << std::endl;
  std::cout << std::endl;
  
  meshQualityFilter->SetHexQualityMeasureToMedAspectFrobenius( );
  meshQualityFilter->Update( );
  std::cout << "Med Frobenius Norm: " << std::endl;
  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Average: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 1 ); 
  std::cout << std::endl;
  std::cout << "Minimum: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 0 ); 
  std::cout << std::endl;
  std::cout << "Maximum: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 2 ); 
  std::cout << std::endl;
  std::cout << "Variance: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 3 ); 
  std::cout << std::endl;
  std::cout << std::endl;

  meshQualityFilter->SetHexQualityMeasureToMaxAspectFrobenius( );
  meshQualityFilter->Update( );
  std::cout << "Max Frobenius Norm: " << std::endl;
  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Average: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 1 ); 
  std::cout << std::endl;
  std::cout << "Minimum: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 0 ); 
  std::cout << std::endl;
  std::cout << "Maximum: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 2 ); 
  std::cout << std::endl;
  std::cout << "Variance: ";
  std::cout << meshQualityFilter->GetOutput()->GetFieldData()->GetArray( "Mesh Hexahedron Quality" )->GetComponent( 0, 3 ); 
  std::cout << std::endl;
  std::cout << std::endl;

 
  return 0;
}


