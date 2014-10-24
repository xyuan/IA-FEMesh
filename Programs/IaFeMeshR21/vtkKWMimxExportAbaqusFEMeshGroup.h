/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxExportAbaqusFEMeshGroup.h,v $
Language:  C++
Date:      $Date: 2008/06/17 00:30:32 $
Version:   $Revision: 1.11 $

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
// .NAME vtkKWMimxExportAbaqusFEMeshGroup - a taFEMeshed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxExportAbaqusFEMeshGroup_h
#define __vtkKWMimxExportAbaqusFEMeshGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

class vtkKWComboBoxWithLabel;
class vtkKWTextWithScrollbarsWithLabel;
class vtkKWLoadSaveDialog;
class vtkKWFrameWithLabel;
class vtkKWEntryWithLabel;


class vtkKWMimxExportAbaqusFEMeshGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxExportAbaqusFEMeshGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxExportAbaqusFEMeshGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void ExportAbaqusFEMeshDoneCallback();
  void ExportAbaqusFEMeshCancelCallback();
  int ExportAbaqusFEMeshApplyCallback();
  void UpdateObjectLists();
protected:
	vtkKWMimxExportAbaqusFEMeshGroup();
	~vtkKWMimxExportAbaqusFEMeshGroup();
	virtual void CreateWidget();
	
	vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWLoadSaveDialog *FileBrowserDialog;
  vtkKWTextWithScrollbarsWithLabel *HeaderInformationText;
  vtkKWEntryWithLabel *UserNameEntry;
  
private:
  vtkKWMimxExportAbaqusFEMeshGroup(const vtkKWMimxExportAbaqusFEMeshGroup&); // Not implemented
  void operator=(const vtkKWMimxExportAbaqusFEMeshGroup&); // Not implemented
};

#endif

