/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSplitUnstructuredHexahedronGridCell.h,v $

  Copyright (c) University of Iowa
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSplitUnstructuredHexahedronGridCell - 
// .SECTION Caveats
// Filter takes in an unstructuredgrid as primary input. PointsList input
// should contain two points denoting an edge in the unstructured grid.
// the output contains unstructured grid with the cell containing the edge
// split in half.

#ifndef __vtkSplitUnstructuredHexahedronGridCell_h
#define __vtkSplitUnstructuredHexahedronGridCell_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkIdList;

class VTK_GRAPHICS_EXPORT vtkSplitUnstructuredHexahedronGridCell : public vtkUnstructuredGridAlgorithm
{
public:

  static vtkSplitUnstructuredHexahedronGridCell *New();
  vtkTypeRevisionMacro(vtkSplitUnstructuredHexahedronGridCell,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkSetMacro(IdList, vtkIdList*);
protected:
  vtkSplitUnstructuredHexahedronGridCell();
  ~vtkSplitUnstructuredHexahedronGridCell();
  vtkIdList *IdList;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  private:
  vtkSplitUnstructuredHexahedronGridCell(const vtkSplitUnstructuredHexahedronGridCell&);  // Not implemented.
  void operator=(const vtkSplitUnstructuredHexahedronGridCell&);  // Not implemented.
};

#endif
