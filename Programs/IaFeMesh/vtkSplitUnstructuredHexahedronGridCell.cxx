/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSplitUnstructuredHexahedronGridCell.cxx,v $

  Copyright (c) University of Iowa
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.


=========================================================================*/
#include "vtkSplitUnstructuredHexahedronGridCell.h"

#include "vtkCell.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkUnstructuredGrid.h"

vtkCxxRevisionMacro(vtkSplitUnstructuredHexahedronGridCell, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkSplitUnstructuredHexahedronGridCell);

// Description:

vtkSplitUnstructuredHexahedronGridCell::vtkSplitUnstructuredHexahedronGridCell()
{
	this->IdList = NULL;
}

vtkSplitUnstructuredHexahedronGridCell::~vtkSplitUnstructuredHexahedronGridCell()
{
}

int vtkSplitUnstructuredHexahedronGridCell::RequestData(
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
 
  if(!this->IdList)
  {
	  vtkErrorMacro("Enter the point numbers of the edge to be divided,  edge should contain only two points");
	  return 0;
  }
  if(IdList->GetNumberOfIds() !=2)
  {
	  vtkErrorMacro("Edge should contain only two points");
	  return 0;
  }

  vtkDebugMacro(<< "Check if the edge points input are valid ?");
  vtkIdType numCells = input->GetNumberOfCells();
  vtkIdType numPts = input->GetNumberOfPoints();

// Initialize and check input
//
  int i, edge_num;
  bool status = false;
	int count_ele = 0;
  do {
	for(i=0; i < 12; i++)
	{
		vtkCell *cell = input->GetCell(count_ele)->GetEdge(i);
		//vtkIdType pt0 = cell->GetPointId(0);
		//vtkIdType pt1 = cell->GetPointId(1);
		if(cell->GetPointId(0) == IdList->GetId(0) || cell->GetPointId(1) == IdList->GetId(0))
		{
			if(cell->GetPointId(0) == IdList->GetId(1) || cell->GetPointId(1) == IdList->GetId(1))
			{
				status = true;
				break;
			}
		}
	}
	count_ele++;
  } while(count_ele < input->GetNumberOfCells() && !status);

  if(!status)
  {
	  vtkErrorMacro("Invalid edge points entered, check for correct edge points input");
	  return 0;
  }
  // if the input is valid
   edge_num = i;
  double x1[3], x2[3];
  //	get points based on the plane being divided
  //	lines accessed are in CW
  // generate the connectivity to replace the original cell with
  //	the cell closest to the origin
  vtkIdType* pts_replace;
  vtkIdType npts;
  input->GetCellPoints(count_ele-1, npts, pts_replace);

  // storage of output
  output->Allocate(input->GetNumberOfCells()+1,input->GetNumberOfCells()+1);
  output->SetPoints(input->GetPoints());
  for(i=0; i <input->GetNumberOfCells(); i++)
  {
		output->InsertNextCell(input->GetCellType(i), input->GetCell(i)->GetPointIds());
  }
  //	list for the new cell being inserted into the grid
  vtkIdList* idlistnew = vtkIdList::New();
  idlistnew->SetNumberOfIds(8);
  for(i=0; i <8; i++)	{idlistnew->SetId(i,pts_replace[i]);}

  if(edge_num ==1 || edge_num == 3 || edge_num == 5 || edge_num == 7)
  {
	  // calculate the coordinates of the points
	  // point is located at the center of the line chosen
	  output->GetPoint(pts_replace[0],x1);	output->GetPoint(pts_replace[3],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[4],x1);	output->GetPoint(pts_replace[7],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[5],x1);	output->GetPoint(pts_replace[6],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[1],x1);	output->GetPoint(pts_replace[2],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  pts_replace[2] = output->GetPoints()->GetNumberOfPoints()-1;
	  pts_replace[3] = output->GetPoints()->GetNumberOfPoints()-4;
	  pts_replace[6] = output->GetPoints()->GetNumberOfPoints()-2;
	  pts_replace[7] = output->GetPoints()->GetNumberOfPoints()-3;
	  output->ReplaceCell(count_ele-1,8,pts_replace);
	  // generate the connectivity to insert cell farther from the origin
	  //	into grid 
	  idlistnew->SetId(0,output->GetPoints()->GetNumberOfPoints()-4);
	  idlistnew->SetId(1,output->GetPoints()->GetNumberOfPoints()-1);
	  idlistnew->SetId(4,output->GetPoints()->GetNumberOfPoints()-3);
	  idlistnew->SetId(5,output->GetPoints()->GetNumberOfPoints()-2);
	  output->InsertNextCell(12, idlistnew);

	  idlistnew->Delete();
	  output->Squeeze();

		return 1;
  }

  if(edge_num > 7 && edge_num < 12)
  {
	  // calculate the coordinates of the points
	  // point is located at the center of the line chosen
	  output->GetPoint(pts_replace[0],x1);	output->GetPoint(pts_replace[4],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[1],x1);	output->GetPoint(pts_replace[5],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[2],x1);	output->GetPoint(pts_replace[6],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[3],x1);	output->GetPoint(pts_replace[7],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  pts_replace[4] = output->GetPoints()->GetNumberOfPoints()-4;
	  pts_replace[5] = output->GetPoints()->GetNumberOfPoints()-3;
	  pts_replace[6] = output->GetPoints()->GetNumberOfPoints()-2;
	  pts_replace[7] = output->GetPoints()->GetNumberOfPoints()-1;
	  output->ReplaceCell(count_ele-1,8,pts_replace);
	  // generate the connectivity to insert cell farther from the origin
	  //	into grid 
	  idlistnew->SetId(0,output->GetPoints()->GetNumberOfPoints()-4);
	  idlistnew->SetId(1,output->GetPoints()->GetNumberOfPoints()-3);
	  idlistnew->SetId(2,output->GetPoints()->GetNumberOfPoints()-2);
	  idlistnew->SetId(3,output->GetPoints()->GetNumberOfPoints()-1);
	  output->InsertNextCell(12, idlistnew);
	  idlistnew->Delete();
	  output->Squeeze();

	  return 1;
  }

    if(edge_num == 0 || edge_num == 2 || edge_num == 4 || edge_num == 6)
  {
	  // calculate the coordinates of the points
	  // point is located at the center of the line chosen
	  output->GetPoint(pts_replace[0],x1);	output->GetPoint(pts_replace[1],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[3],x1);	output->GetPoint(pts_replace[2],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[7],x1);	output->GetPoint(pts_replace[6],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  output->GetPoint(pts_replace[4],x1);	output->GetPoint(pts_replace[5],x2);
	  output->GetPoints()->InsertNextPoint(
		  (x1[0]+x2[0])/2.0, (x1[1]+x2[1])/2.0, (x1[2]+x2[2])/2.0);
	  pts_replace[1] = output->GetPoints()->GetNumberOfPoints()-4;
	  pts_replace[2] = output->GetPoints()->GetNumberOfPoints()-3;
	  pts_replace[5] = output->GetPoints()->GetNumberOfPoints()-1;
	  pts_replace[6] = output->GetPoints()->GetNumberOfPoints()-2;
	  output->ReplaceCell(count_ele-1,8,pts_replace);
	  // generate the connectivity to insert cell farther from the origin
	  //	into grid 
	  idlistnew->SetId(0,output->GetPoints()->GetNumberOfPoints()-4);
	  idlistnew->SetId(3,output->GetPoints()->GetNumberOfPoints()-3);
	  idlistnew->SetId(4,output->GetPoints()->GetNumberOfPoints()-1);
	  idlistnew->SetId(7,output->GetPoints()->GetNumberOfPoints()-2);
	  output->InsertNextCell(12, idlistnew);

	  idlistnew->Delete();
	  output->Squeeze();

	  return 1;
  }
  return 0;
}

void vtkSplitUnstructuredHexahedronGridCell::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
