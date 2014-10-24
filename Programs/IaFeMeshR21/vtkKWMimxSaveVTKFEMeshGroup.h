/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveVTKFEMeshGroup.h,v $
Language:  C++
Date:      $Date: 2008/05/31 19:19:38 $
Version:   $Revision: 1.8 $

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
// .NAME vtkKWMimxSaveVTKFEMeshGroup - a taFEMeshed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxSaveVTKFEMeshGroup_h
#define __vtkKWMimxSaveVTKFEMeshGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;
class vtkKWFrameWithLabel;

class vtkKWMimxSaveVTKFEMeshGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxSaveVTKFEMeshGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveVTKFEMeshGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  void SaveVTKFEMeshCancelCallback();
  int SaveVTKFEMeshApplyCallback();
  void UpdateObjectLists();
  
protected:
	vtkKWMimxSaveVTKFEMeshGroup();
	~vtkKWMimxSaveVTKFEMeshGroup();
	virtual void CreateWidget();
	
	vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWLoadSaveDialog *FileBrowserDialog;
private:
  vtkKWMimxSaveVTKFEMeshGroup(const vtkKWMimxSaveVTKFEMeshGroup&); // Not implemented
void operator=(const vtkKWMimxSaveVTKFEMeshGroup&); // Not implemented
 };

#endif

