/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMainNotebook.h,v $
Language:  C++
Date:      $Date: 2008/04/24 14:08:40 $
Version:   $Revision: 1.24 $

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
// .NAME vtkKWMimxMainNotebook - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxMainNotebook_h
#define __vtkKWMimxMainNotebook_h

#include "vtkKWCompositeWidget.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkKWMimxBBMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkLinkedListWrapperTree.h"

class vtkKWUserInterfacePanel;
class vtkKWFrameWithScrollbar;
class vtkKWMenuButtonWithLabel;
class vtkMimxErrorCallback;
class vtkKWMimxImageMenuGroup;
class vtkKWMimxMainWindow;
class vtkKWFrameWithLabel;
class vtkKWUserInterfacePanel;
class vtkKWMimxQualityMenuGroup;
class vtkKWMimxMaterialPropertyMenuGroup;
class vtkKWMimxBoundaryConditionsMenuGroup;


class vtkKWMimxMainNotebook : public vtkKWCompositeWidget
{
public:
  static vtkKWMimxMainNotebook* New();
  vtkTypeRevisionMacro(vtkKWMimxMainNotebook,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
 // const char* GetActivePage();	// return the name of the chosen page
 // int GetActiveOption();	// return the chosen operation
  virtual void Update();
  virtual void UpdateEnableState();
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(SurfaceMenuGroup, vtkKWMimxSurfaceMenuGroup);
  vtkGetObjectMacro(ImageMenuGroup, vtkKWMimxImageMenuGroup);
  vtkGetObjectMacro(FEMeshMenuGroup, vtkKWMimxFEMeshMenuGroup);
  vtkGetObjectMacro(QualityMenuGroup, vtkKWMimxQualityMenuGroup);
  vtkSetObjectMacro(DoUndoTree, vtkLinkedListWrapperTree);
protected:
	vtkKWMimxMainNotebook();
	~vtkKWMimxMainNotebook();
  void SetLists();
	virtual void CreateWidget();
	vtkKWUserInterfacePanel *Notebook;
	vtkKWFrameWithLabel *MainFrame;
	vtkKWMimxMainWindow *MimxMainWindow;
	vtkKWMimxSurfaceMenuGroup *SurfaceMenuGroup;
	vtkKWMimxBBMenuGroup *BBMenuGroup;
	vtkKWMimxFEMeshMenuGroup *FEMeshMenuGroup;
	vtkKWMimxImageMenuGroup *ImageMenuGroup;
  vtkKWMimxQualityMenuGroup *QualityMenuGroup;
  vtkKWMimxMaterialPropertyMenuGroup *MaterialPropertyMenuGroup;
  vtkKWMimxBoundaryConditionsMenuGroup *BoundaryConditionsMenuGroup;
	vtkLinkedListWrapperTree *DoUndoTree;
	
	virtual void ProcessCallbackCommandEvents(
    vtkObject *caller, unsigned long event, void *calldata);
    
private:
  vtkKWMimxMainNotebook(const vtkKWMimxMainNotebook&); // Not implemented
  void operator=(const vtkKWMimxMainNotebook&); // Not implemented
  
  char *tabLabels[9];
  int startNotebookPage;
  int showNumberOfPages;
 };

#endif

