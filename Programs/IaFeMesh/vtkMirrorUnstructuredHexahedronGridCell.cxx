/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMirrorUnstructuredHexahedronGridCell.cxx,v $

  Copyright (c) University of Iowa
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.


=========================================================================*/
#include "vtkMirrorUnstructuredHexahedronGridCell.h"

#include "vtkCell.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkTriangle.h"
#include "vtkUnstructuredGrid.h"

vtkCxxRevisionMacro(vtkMirrorUnstructuredHexahedronGridCell, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMirrorUnstructuredHexahedronGridCell);

// Description:

vtkMirrorUnstructuredHexahedronGridCell::vtkMirrorUnstructuredHexahedronGridCell()
{
	// default set to x
	this->Axis = 0;
	this->MirrorPoint = 0.0; 
}

vtkMirrorUnstructuredHexahedronGridCell::~vtkMirrorUnstructuredHexahedronGridCell()
{
}

int vtkMirrorUnstructuredHexahedronGridCell::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
	
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
 
  vtkIdType numCells = input->GetNumberOfCells();
  vtkIdType numPts = input->GetNumberOfPoints();
  if(numCells < 1 || numPts <8)
  {
	  vtkErrorMacro("Invalid input data");
	  return 0;
  }
// Initialize and check input
  // if the input is valid
  
  // storage of output
  output->Allocate(2*input->GetNumberOfCells(), 2*input->GetNumberOfCells());
  output->SetPoints(input->GetPoints());
  int i;

  for(i=0; i <input->GetNumberOfCells(); i++)
  {
		output->InsertNextCell(input->GetCellType(i), input->GetCell(i)->GetPointIds());
  }
  // add new cells based on mirroring point and the axis
 
  vtkIdList *idlist = vtkIdList::New();
  double x[3];
  for(i = 0; i < numPts; i++)
  {
	  input->GetPoint(i,x);
	  x[this->Axis] = 2.0*this->MirrorPoint - x[this->Axis];
	  output->GetPoints()->InsertNextPoint(x);
  }
  for(i=0; i <numCells; i++)
  {
	  idlist->Initialize();
	  idlist->SetNumberOfIds(8);
	  for(int j=0; j<8; j++)
	  {
		  idlist->SetId(j,input->GetCell(i)->GetPointId(j)+numPts);
	  }
	  output->InsertNextCell(12, idlist);
  }
  idlist->Delete();
  output->Squeeze();
 return 1;
  
}

void vtkMirrorUnstructuredHexahedronGridCell::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
