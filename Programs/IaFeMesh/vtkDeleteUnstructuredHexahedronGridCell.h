/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDeleteUnstructuredHexahedronGridCell.h,v $

  Copyright (c) University of Iowa
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkDeleteUnstructuredHexahedronGridCell - 
// .SECTION Caveats
// Filter takes in an unstructuredgrid as primary input. PointsList input
// should contain 4 points denoting an face in the unstructured grid.
// the output contains unstructured grid with the cell containing the
// face will have a newly extruded neighboring cell sharing an edge with the
// face originally picked.

#ifndef __vtkDeleteUnstructuredHexahedronGridCell_h
#define __vtkDeleteUnstructuredHexahedronGridCell_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkIdList;

class VTK_GRAPHICS_EXPORT vtkDeleteUnstructuredHexahedronGridCell : public vtkUnstructuredGridAlgorithm
{
public:

  static vtkDeleteUnstructuredHexahedronGridCell *New();
  vtkTypeRevisionMacro(vtkDeleteUnstructuredHexahedronGridCell,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkSetMacro(CellNum, vtkIdType);
protected:
  vtkDeleteUnstructuredHexahedronGridCell();
  ~vtkDeleteUnstructuredHexahedronGridCell();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  vtkIdType CellNum;
  private:
  vtkDeleteUnstructuredHexahedronGridCell(const vtkDeleteUnstructuredHexahedronGridCell&);  // Not implemented.
  void operator=(const vtkDeleteUnstructuredHexahedronGridCell&);  // Not implemented.
};

#endif
