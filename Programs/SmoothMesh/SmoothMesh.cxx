/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SmoothMesh.cxx,v $
  Language:  C++
  Date:      $Date: 2007/12/01 02:44:59 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef _WIN32
#pragma warning ( disable : 4786 )
#endif

// Software Guide : BeginLatex
//
// This example illustrates how to perform Iterative Closest Point (ICP) 
// registration in ITK using a DistanceMap in order to increase the performance.
// There is of course a trade-off between the time needed for computing the
// DistanceMap and the time saving obtained by its repeated use during the
// iterative computation of the point to point distances. It is then necessary
// in practice to ponder both factors.
//
// \doxygen{EuclideanDistancePointMetric}.
//
// Software Guide : EndLatex 


#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>
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
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataWriter.h>


#include <metaCommand.h>
#include <iostream>
#include <fstream>
#include <list>

class l3Point {
  public:
    float loc[3];
    int n;
    int ptId;
  };

  class l3Element {
  public:
    int id;
    int n[4];
  };


int translatePointNumber(int, std::list<l3Point> *);
int reversetranslatePointNumber(int vtkId, std::list<l3Point> *ptList);


int main(int argc, char * argv[] )
{

  MetaCommand command;

  command.SetOption("ElementFile","e",false,"Element File");
  command.AddOptionField("ElementFile","filename",MetaCommand::STRING,true);

  command.SetOption("NodeFile","n",false,"Node File");
  command.AddOptionField("NodeFile","filename",MetaCommand::STRING,true);

  command.SetOption("SmoothedNodeFile","x",false,"Output Node File");
  command.AddOptionField("SmoothedNodeFile","filename",MetaCommand::STRING,true);
  
  command.SetOption("OriginalVtkFile","o",false,"Original VTK File");
  command.AddOptionField("OriginalVtkFile","filename", MetaCommand::STRING, true);

  command.SetOption("SmoothedVtkFile","s",false,"Smoothed VTK File");
  command.AddOptionField("SmoothedVtkFile","filename",MetaCommand::STRING,true);
  
  command.SetOption("NumberOfIterations","i",false,"Number of Iterations [100]");
  command.AddOptionField("NumberOfIterations","number",MetaCommand::INT,false,"100");
  
  command.SetOption("RelaxationFactor","f",false,"Relaxation Factor");
  command.AddOptionField("RelaxationFactor","value",MetaCommand::FLOAT,false,"0.3");
  
  command.SetOption("BoundarySmoothing","b",false,"Boundary Smoothing [0=off, 1=on]");
  command.AddOptionField("BoundarySmoothing","option",MetaCommand::INT,false,"0");

  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string ElementFilename (command.GetValueAsString("ElementFile","filename"));
  const std::string NodeFilename (command.GetValueAsString("NodeFile","filename"));
  const std::string OriginalVtkFile (command.GetValueAsString("OriginalVtkFile","filename"));
  const std::string SmoothedVtkFile (command.GetValueAsString("SmoothedVtkFile","filename")); 
  const std::string SmoothedNodeFile (command.GetValueAsString("SmoothedNodeFile","filename"));  
      
  int numberOfIterations = command.GetValueAsInt("NumberOfIterations","number");
  float relaxationFactor = command.GetValueAsFloat("RelaxationFactor","value");
  int boundarySmoothing = command.GetValueAsInt("BoundarySmoothing","option");
  
  
  std::cout << "Node Filename: " <<  NodeFilename << std::endl; 
  std::cout << "Element Filename: " <<  ElementFilename << std::endl; 
  std::cout << "Smoothed Node Filename: " <<  SmoothedNodeFile << std::endl; 
  std::cout << "Original Vtk File: " <<  OriginalVtkFile << std::endl; 
  std::cout << "Smoothed Vtk File: " <<  SmoothedVtkFile << std::endl; 
  std::cout << "Iterations: " << numberOfIterations <<std::endl;
  std::cout << "Relaxation Factor: " << relaxationFactor <<std::endl;
  std::cout << "Boundary Smoothing: " << boundarySmoothing <<std::endl;
   
  

  std::list<l3Point> *pointList = new std::list<l3Point>;
  std::list<l3Element> *elementList = new std::list<l3Element>;

  ifstream pointFile;
  pointFile.open (NodeFilename.c_str(), ios::in);
  if (! pointFile.is_open())
    {
    std::cerr << "Failed to open file: " << NodeFilename << std::endl;
    exit(1);
    }

  char fbuf[512];
  char *ptr;

  pointFile.getline(fbuf, 512); 
 
  int ptIndex=0;
  while ( ! pointFile.eof() )
    {
    l3Point point;
    ptr=strtok(fbuf, ","); point.n = atoi(ptr);
    ptr=strtok(NULL, ","); point.loc[0] = atof(ptr);
    ptr=strtok(NULL, ","); point.loc[1] = atof(ptr);
    ptr=strtok(NULL, ","); point.loc[2] = atof(ptr);
    point.ptId = ptIndex;
    ptIndex++;
    pointList->push_back( point );
    pointFile.getline(fbuf, 512); 
    std:cerr << point.n << " " << point.loc[0] << " " << point.loc[1] << " " << point.loc[2] << std::endl;
    }
  pointFile.close();
  std::cerr << "Read Nodes" << std::endl;

  ifstream elementFile;
  elementFile.open (ElementFilename.c_str());
  if (! elementFile.is_open())
    {
    std::cerr << "Failed to Element file: " << ElementFilename << std::endl;
    exit(1);
    }

  elementFile.getline(fbuf, 512); 
  while ( ! elementFile.eof() )
    {
    l3Element element;
    ptr=strtok(fbuf, ","); element.id = atoi(ptr);
    ptr=strtok(NULL, ","); element.n[0] = atoi(ptr);
    ptr=strtok(NULL, ","); element.n[1] = atoi(ptr);
    ptr=strtok(NULL, ","); element.n[2] = atoi(ptr);
    ptr=strtok(NULL, ","); element.n[3] = atoi(ptr);

    elementList->push_back( element );
    elementFile.getline(fbuf, 512); 

    std::cerr << element.id << " " << element.n[0] << " " << element.n[1] 
             << " " << element.n[2] << " " << element.n[3] << std::endl;
    }
  elementFile.close();
  std::cerr << "Read Elements" << std::endl;

  vtkPoints *vtkpts = vtkPoints::New( );
    vtkpts->SetNumberOfPoints( pointList->size( ) );
  
  std::list<l3Point>::const_iterator iter;
  for (iter=pointList->begin(); iter != pointList->end(); iter++)
    {
    vtkpts->SetPoint (iter->ptId, iter->loc);
    }
  std::cerr << "Set Points" << std::endl;  
  vtkPolyData *originalMesh = vtkPolyData::New( );
    originalMesh->SetPoints( vtkpts );
    originalMesh->Allocate(elementList->size(), elementList->size());
  std::list<l3Element>::const_iterator itern;
  for (itern=elementList->begin(); itern != elementList->end(); itern++)
    {
    vtkIdType ptdId[4];
    ptdId[0] = translatePointNumber(itern->n[0], pointList);
    ptdId[1] = translatePointNumber(itern->n[1], pointList);
    ptdId[2] = translatePointNumber(itern->n[2], pointList);
    ptdId[3] = translatePointNumber(itern->n[3], pointList);
    originalMesh->InsertNextCell(VTK_QUAD, 4, ptdId);
    }
  std::cerr << "Set Mesh" << std::endl;  
  vtkPolyDataWriter *origMeshWriter = vtkPolyDataWriter::New();
    origMeshWriter->SetFileName( OriginalVtkFile.c_str() );
    origMeshWriter->SetFileTypeToASCII();
    origMeshWriter->SetInput( originalMesh );
    origMeshWriter->Update( );

  vtkSmoothPolyDataFilter *smoothMeshFilter = vtkSmoothPolyDataFilter::New( );
    smoothMeshFilter->SetInput( originalMesh );
    smoothMeshFilter->SetNumberOfIterations( numberOfIterations );
    smoothMeshFilter->SetBoundarySmoothing( boundarySmoothing );
    smoothMeshFilter->SetRelaxationFactor( relaxationFactor );
    smoothMeshFilter->Update( );

  std::cerr << "Smoothed" << std::endl;  
  vtkPolyDataWriter *smoothMeshWriter = vtkPolyDataWriter::New();
    smoothMeshWriter->SetFileName( SmoothedVtkFile.c_str() );
    smoothMeshWriter->SetFileTypeToASCII();
    smoothMeshWriter->SetInput( smoothMeshFilter->GetOutput( ) );
    smoothMeshWriter->Update( );

  ofstream outFile;
  outFile.open (SmoothedNodeFile.c_str());
  if (! outFile.is_open())
    {
    std::cerr << "Failed to Node file: " << SmoothedNodeFile << std::endl;
    exit(1);
    }

  for (int i=0;i<smoothMeshFilter->GetOutput( )->GetNumberOfPoints();i++)
    {
    double pt[3];
    smoothMeshFilter->GetOutput( )->GetPoint(i, pt);
    int id = reversetranslatePointNumber(i, pointList);
    outFile << id << "," << pt[0] << "," << pt[1] << "," << pt[2] << std::endl;
    }
  outFile.close();

// Software Guide : EndCodeSnippet


  return EXIT_SUCCESS;

}



int translatePointNumber(int origId, std::list<l3Point> *ptList)
{
  std::list<l3Point>::const_iterator iter;
  for (iter=ptList->begin(); iter != ptList->end(); iter++)
    {
    if (iter->n == origId) return iter->ptId;
    }

  return 0;
}

int reversetranslatePointNumber(int vtkId, std::list<l3Point> *ptList)
{
  std::list<l3Point>::const_iterator iter;
  for (iter=ptList->begin(); iter != ptList->end(); iter++)
    {
    if (iter->ptId == vtkId) return iter->n;
    }

  return 0;
}


