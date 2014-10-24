/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateFEMeshElementSetGroup.h,v $
Language:  C++
Date:      $Date: 2008/04/28 19:55:54 $
Version:   $Revision: 1.5 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkKWMimxCreateFEMeshElementSetGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxCreateFEMeshElementSetGroup_h
#define __vtkKWMimxCreateFEMeshElementSetGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

class vtkKWComboBoxWithLabel;
class vtkKWRadioButtonSet;
class vtkMimxCreateElementSetWidgetFEMesh;
class vtkKWEntryWithLabel;
class vtkKWPushButtonSet;
class vtkMimxMeshActor;

class vtkKWMimxCreateFEMeshElementSetGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxCreateFEMeshElementSetGroup* New();
vtkTypeRevisionMacro(vtkKWMimxCreateFEMeshElementSetGroup,vtkKWMimxGroupBase);
void PrintSelf(ostream& os, vtkIndent indent);
virtual void Update();
virtual void UpdateEnableState();
int SelectElementsThroughCallback();
int SelectElementsSurfaceCallback();
int SelectVisibleElementsSurfaceCallback();
int SelectSingleElementCallback();
int SelectMultipleElementsCallback();
void VTKInteractionCallback();
int CreateElementSetApplyCallback();
void CreateElementSetCancelCallback();
void UpdateObjectLists();
void AcceptSelectionCallback();
void FullCallback();
void SelectionChangedCallback(const char *Selection);
protected:
	vtkKWMimxCreateFEMeshElementSetGroup();
	~vtkKWMimxCreateFEMeshElementSetGroup();
	virtual void CreateWidget();
vtkKWComboBoxWithLabel *ObjectListComboBox;
vtkKWRadioButtonSet *SelectSubsetRadiobuttonSet;
vtkMimxCreateElementSetWidgetFEMesh *SelectCellsWidget;
vtkKWEntryWithLabel *ElementSetNameEntry;
vtkKWPushButtonSet *AcceptSelectionButtonSet;
vtkMimxMeshActor* GetSelectedObject();
int SelectionState;
private:
  vtkKWMimxCreateFEMeshElementSetGroup(const vtkKWMimxCreateFEMeshElementSetGroup&); // Not implemented
void operator=(const vtkKWMimxCreateFEMeshElementSetGroup&); // Not implemented
 };

#endif

