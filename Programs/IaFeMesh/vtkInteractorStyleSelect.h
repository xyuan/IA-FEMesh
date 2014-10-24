/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkInteractorStyleSelect.h,v $
Language:  C++
Date:      $Date: 2007/09/20 20:40:15 $
Version:   $Revision: 1.4 $

Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInteractorStyleSelect - manipulate camera in scene with natural view up (e.g., terrain)
// .SECTION Description

#ifndef __vtkInteractorStyleSelect_h
#define __vtkInteractorStyleSelect_h

#include "vtkInteractorStyle.h"

#include "vtkPolyData.h"
#include "vtkActor.h"

#define VTKIS_PICKPOINT   10
#define VTKIS_BOUNDINGBOXINTERACT 11

class vtkInteractorStyleSelect : public vtkInteractorStyle
{
public:
	// Description:
	// Instantiate the object.
	static vtkInteractorStyleSelect *New();

	vtkTypeRevisionMacro(vtkInteractorStyleSelect,vtkInteractorStyle);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Event bindings controlling the effects of pressing mouse buttons
	// or moving the mouse.
	virtual void OnMouseMove();
	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();
	virtual void Rotate();
	virtual void Dolly();
	virtual void Pan();
	void SetInteractorStyleSelectState(int);
	int GetInteractorStyleSelectState();
	void SetActor(vtkActor*);
	vtkActor* GetActor();
protected:
	vtkInteractorStyleSelect();
	~vtkInteractorStyleSelect();
	float MotionFactor;
	int InteractorStyleSelectState;
	void PickPoint(vtkActor*);
	vtkActor* Trace;
private:
	vtkInteractorStyleSelect(const vtkInteractorStyleSelect&);  // Not implemented.
	void operator=(const vtkInteractorStyleSelect&);  // Not implemented.

};

#endif

