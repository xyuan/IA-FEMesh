/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMirrorBBGroup.h,v $
Language:  C++
Date:      $Date: 2008/02/01 15:24:57 $
Version:   $Revision: 1.4 $

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
// .NAME vtkKWMimxMirrorBBGroup - a taFEMeshed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxMirrorBBGroup_h
#define __vtkKWMimxMirrorBBGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxEditBBGroup.h"
#include "vtkKWRadioButton.h"

class vtkKWEntryWithLabel;
class vtkKWRadioButton;
class vtkKWRadioButtonSet;
class vtkKWCheckButtonWithLabel;
class vtkKWComboBoxWithLabel;
class vtkPlaneWidget;

class vtkKWMimxMirrorBBGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxMirrorBBGroup* New();
vtkTypeRevisionMacro(vtkKWMimxMirrorBBGroup,vtkKWMimxGroupBase);
void PrintSelf(ostream& os, vtkIndent indent);
virtual void Update();
virtual void UpdateEnableState();
void MirrorBBDoneCallback();
void MirrorBBCancelCallback();
int MirrorBBApplyCallback();
vtkSetObjectMacro(EditBBGroup, vtkKWMimxEditBBGroup);
vtkSetObjectMacro(VTKRadioButton, vtkKWRadioButton);
void PlaceMirroringPlaneAboutX();
void PlaceMirroringPlaneAboutY();
void PlaceMirroringPlaneAboutZ();
void UpdateObjectLists();
void SelectionChangedCallback(const char*);

protected:
	vtkKWMimxMirrorBBGroup();
	~vtkKWMimxMirrorBBGroup();
	virtual void CreateWidget();
	vtkKWMimxEditBBGroup *EditBBGroup;
	vtkKWRadioButton *VTKRadioButton;
	vtkKWCheckButtonWithLabel *TypeOfMirroring;
	vtkKWRadioButtonSet *AxisSelection;
	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkPlaneWidget *MirrorPlaneWidget;
private:
  vtkKWMimxMirrorBBGroup(const vtkKWMimxMirrorBBGroup&); // Not implemented
void operator=(const vtkKWMimxMirrorBBGroup&); // Not implemented
 };

#endif

