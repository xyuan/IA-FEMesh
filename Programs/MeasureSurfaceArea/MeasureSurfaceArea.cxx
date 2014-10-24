/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: MeasureSurfaceArea.cxx,v $
  Language:  C++
  Date:      $Date: 2007/07/02 01:11:31 $
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
#include <vtkCellData.h>
#include <vtkDataArray.h>

#include <metaCommand.h>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif


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
      
  vtkPolyData *polyData;
  if ( (strstr(InputMeshFilename.c_str(), ".stl") != NULL) || (strstr(InputMeshFilename.c_str(), ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(InputMeshFilename.c_str());
    fixedReader->Update( );
    polyData = fixedReader->GetOutput();
    }
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName(InputMeshFilename.c_str());
    polyReader->Update( );
    polyData = polyReader->GetOutput();
    }
  
  /* Compute Mesh Quality */
  vtkMeshQuality *meshQualityFilter = vtkMeshQuality::New();
  meshQualityFilter->SetInput( polyData );
  meshQualityFilter->SetTriangleQualityMeasureToArea( );
  meshQualityFilter->Update( );
 
  vtkDataSet *mesh = meshQualityFilter->GetOutput();
  vtkCellData *cellData = mesh->GetCellData();
  vtkDataArray *areaArray = cellData->GetArray("Quality");

  double area = 0.0;
  int numberOfTuples = areaArray->GetNumberOfTuples();
  int numberOfComponents =  areaArray->GetNumberOfComponents();
  int numPos = 0;
  int numNeg = 0;

  for (int i=0;i< numberOfTuples;i++)
   {
   double curArea = areaArray->GetComponent(i, 0);
   if (curArea > 0) numPos++;
   else numNeg++;
   area += curArea;
   } 
 
  std::cout << "Surface Area Measurement" << std::endl;
  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Filename: " << InputMeshFilename << std::endl;
  std::cout << "Surface Area: " << area << std::endl;
  std::cout << "Total Triangles: " << numberOfTuples << std::endl;
  std::cout << "Num Positive Area: " << numPos << std::endl;
  std::cout << "Number of zero area: " << numNeg << std::endl;
  std::cout << std::endl;
  
  return 0;
}

