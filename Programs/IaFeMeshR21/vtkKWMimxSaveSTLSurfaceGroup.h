/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveSTLSurfaceGroup.h,v $
Language:  C++
Date:      $Date: 2008/05/31 19:19:38 $
Version:   $Revision: 1.10 $

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
// .NAME vtkKWMimxSaveSTLSurfaceGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxSaveSTLSurfaceGroup_h
#define __vtkKWMimxSaveSTLSurfaceGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;
class vtkEntryChangedEventCallback;
class vtkKWFrameWithLabel;

class vtkKWMimxSaveSTLSurfaceGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxSaveSTLSurfaceGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveSTLSurfaceGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void SaveSTLSurfaceDoneCallback();
  int SaveSTLSurfaceApplyCallback();
  void SaveSTLSurfaceCancelCallback();
  void UpdateObjectLists();
protected:
	vtkKWMimxSaveSTLSurfaceGroup();
	~vtkKWMimxSaveSTLSurfaceGroup();
	virtual void CreateWidget();
	
	vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWLoadSaveDialog *FileBrowserDialog;
  vtkEntryChangedEventCallback *EntryChangedCallbackEvent;
private:
  vtkKWMimxSaveSTLSurfaceGroup(const vtkKWMimxSaveSTLSurfaceGroup&); // Not implemented
  void operator=(const vtkKWMimxSaveSTLSurfaceGroup&); // Not implemented
 };

#endif

