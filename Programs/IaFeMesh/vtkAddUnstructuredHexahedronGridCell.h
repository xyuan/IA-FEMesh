/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkAddUnstructuredHexahedronGridCell.h,v $

  Copyright (c) University of Iowa
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAddUnstructuredHexahedronGridCell - 
// .SECTION Caveats
// Filter takes in an unstructuredgrid as primary input. PointsList input
// should contain 4 points denoting an face in the unstructured grid.
// the output contains unstructured grid with the cell containing the
// face will have a newly extruded neighboring cell sharing an edge with the
// face originally picked.

#ifndef __vtkAddUnstructuredHexahedronGridCell_h
#define __vtkAddUnstructuredHexahedronGridCell_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkIdList;

class VTK_GRAPHICS_EXPORT vtkAddUnstructuredHexahedronGridCell : public vtkUnstructuredGridAlgorithm
{
public:

  static vtkAddUnstructuredHexahedronGridCell *New();
  vtkTypeRevisionMacro(vtkAddUnstructuredHexahedronGridCell,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkSetMacro(IdList, vtkIdList*);
  vtkSetMacro(ExtrusionLength, double);
protected:
  vtkAddUnstructuredHexahedronGridCell();
  ~vtkAddUnstructuredHexahedronGridCell();
  vtkIdList *IdList;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int ExtrusionLength;
  private:
  vtkAddUnstructuredHexahedronGridCell(const vtkAddUnstructuredHexahedronGridCell&);  // Not implemented.
  void operator=(const vtkAddUnstructuredHexahedronGridCell&);  // Not implemented.
};

#endif
