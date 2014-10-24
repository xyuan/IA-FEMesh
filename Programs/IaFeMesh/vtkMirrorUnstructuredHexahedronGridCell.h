/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMirrorUnstructuredHexahedronGridCell.h,v $

  Copyright (c) University of Iowa
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMirrorUnstructuredHexahedronGridCell - 
// .SECTION Caveats
// Filter takes in an unstructuredgrid as primary input. The axis perpendicular to
// and the point about which the grid has to be mirrored. 

#ifndef __vtkMirrorUnstructuredHexahedronGridCell_h
#define __vtkMirrorUnstructuredHexahedronGridCell_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkIdList;

class VTK_GRAPHICS_EXPORT vtkMirrorUnstructuredHexahedronGridCell : public vtkUnstructuredGridAlgorithm
{
public:

  static vtkMirrorUnstructuredHexahedronGridCell *New();
  vtkTypeRevisionMacro(vtkMirrorUnstructuredHexahedronGridCell,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkSetMacro(Axis, int);
  vtkSetMacro(MirrorPoint, double);
protected:
  vtkMirrorUnstructuredHexahedronGridCell();
  ~vtkMirrorUnstructuredHexahedronGridCell();
  vtkIdList *IdList;
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  // 0 for X, 1 for Y and 2 for Z
  int Axis;
  double MirrorPoint;
  private:
  vtkMirrorUnstructuredHexahedronGridCell(const vtkMirrorUnstructuredHexahedronGridCell&);  // Not implemented.
  void operator=(const vtkMirrorUnstructuredHexahedronGridCell&);  // Not implemented.
};

#endif
