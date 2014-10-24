/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxDefineNodeSetGroup.h,v $
Language:  C++
Date:      $Date: 2008/06/28 01:24:43 $
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
// .NAME vtkKWMimxDefineNodeSetGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxDefineNodeSetGroup_h
#define __vtkKWMimxDefineNodeSetGroup_h

#include "vtkKWTopLevel.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWComboBox.h"

class vtkKWCheckButtonSet; 
class vtkKWPushButton;
class vtkKWMenuButtonWithLabel;
class vtkKWFrame;
class vtkKWPushButtonSet;
class vtkKWEntryWithLabel;
class vtkLinkedListWrapper;
class vtkMimxSelectPointsWidget;
class vtkMimxMeshActor;
class vtkKWScaleWithLabel;
class vtkKWRadioButtonSet;
class vtkKWCheckButton;
class vtkKWComboBox;


class vtkKWMimxDefineNodeSetGroup : public vtkKWTopLevel
{
public:
  static vtkKWMimxDefineNodeSetGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxDefineNodeSetGroup,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  vtkSetObjectMacro(NodeSetCombobox, vtkKWComboBox);
  virtual void Update();
  virtual void UpdateEnableState();
  void SetMimxMainWindow(vtkKWMimxMainWindow *window);
  vtkKWMimxMainWindow *GetMimxMainWindow( );
  
  void SelectNodesThroughCallback(int mode);
  void SelectNodesSurfaceCallback(int mode);
  void SelectVisibleNodesSurfaceCallback(int mode);
  void SelectFaceNodeCallback(int mode);
  void SelectSingleNodeCallback(int mode);
  void DisableNodeSelection(int mode);
  int  EnableNodeSelection(int mode);
  
  int  ApplyButtonCallback();
  void SetMeshActor( vtkMimxMeshActor *actor );
  void OpacityCallback(double Val);
  void AddNodesCallback();
  void SubtractNodesCallback();
  virtual void Withdraw();
  void RemoveSelectionsCallback();
  void ResetState();
protected:
	vtkKWMimxDefineNodeSetGroup();
	~vtkKWMimxDefineNodeSetGroup();
	virtual void CreateWidget();
	vtkMimxMeshActor* GetSelectedObject();
	
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkLinkedListWrapper *ObjectList;
  
  vtkKWCheckButtonSet *SelectButtonSet;
  vtkKWEntryWithLabel *SetLabelEntry;
  vtkKWPushButton *CancelButton;
  vtkKWPushButton *ApplyButton;
  vtkKWFrame *ButtonFrame;
  vtkKWFrame *ModeFrame;
  vtkKWFrame *OperationFrame;
  vtkMimxSelectPointsWidget *SelectPointsWidget;
  vtkKWScaleWithLabel *OpacityScale;
  vtkKWRadioButtonSet *AddSubtractButton;
  vtkKWPushButton *ClearSelectionButton;
  vtkKWComboBox *NodeSetCombobox;
private:
  vtkKWMimxDefineNodeSetGroup(const vtkKWMimxDefineNodeSetGroup&); // Not implemented
  void operator=(const vtkKWMimxDefineNodeSetGroup&); // Not implemented
  
  int SelectionState;
  vtkMimxMeshActor *MeshActor;  
};
 
#endif

