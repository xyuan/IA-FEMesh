/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMainMenuGroup.h,v $
Language:  C++
Date:      $Date: 2008/05/27 02:33:14 $
Version:   $Revision: 1.14 $

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
// .NAME vtkKWMimxMainMenuGroup - It is the base class for all operations menu options.
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It is the base class
// for all operations menu options.

#ifndef __vtkKWMimxMainMenuGroup_h
#define __vtkKWMimxMainMenuGroup_h

#include "vtkKWCompositeWidget.h"
#include "vtkKWMimxMainWindow.h"
//#include "vtkKWMimxViewProperties.h"
//#include "vtkKWMenuButtonWithLabel.h"

#include "vtkMimxActorBase.h"

#include "vtkLinkedListWrapper.h"
#include "vtkLinkedListWrapperTree.h"

class vtkKWMenuButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMimxViewProperties;
class vtkKWMessageDialog;
class vtkKWRenderWidget;
//class vtkLinkedListWrapperTree;
//class vtkKWMimxMainWindow;

//class vtkLinkedListWrapper;

class vtkKWMimxMainMenuGroup : public vtkKWCompositeWidget
{
public:
  static vtkKWMimxMainMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxMainMenuGroup,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Update();
  virtual void UpdateEnableState();
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
//  vtkGetObjectMacro(ObjectList, vtkLinkedListWrapper);
  vtkGetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkGetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkGetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkGetObjectMacro(ImageList, vtkLinkedListWrapper);
  vtkGetObjectMacro(ImageMaskList, vtkLinkedListWrapper);
//  vtkSetObjectMacro(ViewProperties, vtkKWMimxViewProperties);
  vtkSetObjectMacro(DoUndoTree, vtkLinkedListWrapperTree);
  vtkGetObjectMacro(MainFrame, vtkKWFrame);
  //
  virtual void SetMenuButtonsEnabled(int);
 protected:
	vtkKWMimxMainMenuGroup();
	virtual ~vtkKWMimxMainMenuGroup();

	virtual void CreateWidget();
	vtkKWMenuButtonWithLabel *ObjectMenuButton;
	vtkKWMenuButtonWithLabel *OperationMenuButton;
	vtkKWMenuButtonWithLabel *TypeMenuButton;
  vtkKWFrame *MainFrame;
//	vtkLinkedListWrapper *ObjectList;
  vtkLinkedListWrapper *SurfaceList;
  vtkLinkedListWrapper *BBoxList;
  vtkLinkedListWrapper *FEMeshList;
  vtkLinkedListWrapper *ImageList;
  vtkLinkedListWrapper *ImageMaskList;
	const char* ExtractFileName(const char*);
	vtkKWMimxMainWindow *MimxMainWindow;
	vtkKWMimxViewProperties *ViewProperties;
	vtkLinkedListWrapperTree *DoUndoTree;

	//static void ErrorMessage(vtkObject* object, 
	//	unsigned long event,
	//	void* clientdata, 
	//	void* calldata);

	//static void ErrorMessage(const char *);
	int ErrorState;
	//vtkKWMessageDialog *MessageDialog;

private:
  vtkKWMimxMainMenuGroup(const vtkKWMimxMainMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxMainMenuGroup&); // Not implemented
 };

#endif


