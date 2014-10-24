/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkComputeNormalsFromPolydataFilter.h,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkComputeNormalsFromPolydataFilter_h
#define __vtkComputeNormalsFromPolydataFilter_h

#include "vtkPointSetAlgorithm.h"

class VTK_GRAPHICS_EXPORT vtkComputeNormalsFromPolydataFilter : public vtkPointSetAlgorithm
{
public:
  static vtkComputeNormalsFromPolydataFilter *New();
  vtkTypeRevisionMacro(vtkComputeNormalsFromPolydataFilter,vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  
protected:
  vtkComputeNormalsFromPolydataFilter();
  ~vtkComputeNormalsFromPolydataFilter();

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
private:
  vtkComputeNormalsFromPolydataFilter(const vtkComputeNormalsFromPolydataFilter&);  // Not implemented.
  void operator=(const vtkComputeNormalsFromPolydataFilter&);  // Not implemented.

};

#endif
