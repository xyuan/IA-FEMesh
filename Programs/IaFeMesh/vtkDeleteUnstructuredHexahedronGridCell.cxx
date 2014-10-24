/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDeleteUnstructuredHexahedronGridCell.cxx,v $

  Copyright (c) University of Iowa
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.


=========================================================================*/
#include "vtkDeleteUnstructuredHexahedronGridCell.h"

#include "vtkCell.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointLocator.h"
#include "vtkPoints.h"
#include "vtkTriangle.h"
#include "vtkUnstructuredGrid.h"

vtkCxxRevisionMacro(vtkDeleteUnstructuredHexahedronGridCell, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkDeleteUnstructuredHexahedronGridCell);

// Description:

vtkDeleteUnstructuredHexahedronGridCell::vtkDeleteUnstructuredHexahedronGridCell()
{
	this->CellNum = -1;
}

vtkDeleteUnstructuredHexahedronGridCell::~vtkDeleteUnstructuredHexahedronGridCell()
{
}

int vtkDeleteUnstructuredHexahedronGridCell::RequestData(
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
 
  if(this->CellNum < 0 || this->CellNum >= input->GetNumberOfCells())
  {
	  vtkErrorMacro("Cell number to be deleted incorrect");
	  return 0;
  }

  vtkDebugMacro(<< "Check if the face points input are valid ?");
  vtkIdType numCells = input->GetNumberOfCells();
  vtkIdType numPts = input->GetNumberOfPoints();
  if(numCells < 1 || numPts <8)
  {
	  vtkErrorMacro("Invalid input data");
	  return 0;
  }
  
  // storage of output
  output->Allocate(input->GetNumberOfCells()-1,input->GetNumberOfCells()-1);
  vtkPointLocator *locator = vtkPointLocator::New();
  vtkPoints *points = vtkPoints::New();
  points->Allocate(input->GetNumberOfPoints());
  locator->InitPointInsertion (points, input->GetPoints()->GetBounds());
  vtkIdList *idlist;
  int i;
  double x[3];
  vtkIdType tempid;
  for(i=0; i <input->GetNumberOfCells(); i++)
  {
	  if(i != this->CellNum)
	  {
		  idlist = input->GetCell(i)->GetPointIds();
		  for(int j=0; j <8; j++)
		  {
			  input->GetPoints()->GetPoint(idlist->GetId(j),x);		 
			  locator->InsertUniquePoint(x,tempid);
			  idlist->SetId(j,tempid);
		  }
		  output->InsertNextCell(12, idlist);
	  }
  }
  points->Squeeze();
  output->SetPoints(points);
  output->Squeeze();
  points->Delete();
  locator->Delete();
 return 1;
  
}

void vtkDeleteUnstructuredHexahedronGridCell::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
