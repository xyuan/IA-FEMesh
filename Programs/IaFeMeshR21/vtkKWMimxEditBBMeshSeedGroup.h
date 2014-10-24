/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxEditBBMeshSeedGroup.h,v $
Language:  C++
Date:      $Date: 2008/06/28 01:24:44 $
Version:   $Revision: 1.17 $

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
// .NAME vtkKWMimxEditBBMeshSeedGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxEditBBMeshSeedGroup_h
#define __vtkKWMimxEditBBMeshSeedGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"

class vtkKWMenuButtonWithLabel;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWPushButton;
class vtkMimxColorCodeMeshSeedActor;

class vtkLinkedListWrapper;
class vtkKWCheckButtonWithLabel;
class vtkIntArray;
class vtkMimxPlaceLocalAxesWidget;
class vtkKWRadioButton;
class vtkKWComboBoxWithLabel;
class vtkKWRenderWidget;
class vtkKWFrameWithLabel;
class vtkKWRadioButton;
class vtkKWRadioButtonSet;
class vtkKWFrame;


class vtkKWMimxEditBBMeshSeedGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxEditBBMeshSeedGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxEditBBMeshSeedGroup,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);

  void EditBBMeshSeedDoneCallback();
  void EditBBMeshSeedCancelCallback();
  int EditBBMeshSeedApplyCallback();
  void EditBBMeshSeedVtkInteractionCallback();
  void EditBBMeshSeedPickCellCallback();
  void SelectionChangedCallback(const char*);
  vtkGetObjectMacro(XMeshSeed, vtkKWEntryWithLabel);
  vtkGetObjectMacro(YMeshSeed, vtkKWEntryWithLabel);
  vtkGetObjectMacro(ZMeshSeed, vtkKWEntryWithLabel);
  void UpdateObjectLists();
  void UserDefinedElementLengthCheckButtonCallback();
  void NumberOfDivisionsCheckButtonCallback();
  void MeshSeedTypeCreateCallback();
  void MeshSeedTypeEditCallback();
  void ShowMeshSeedCallback(int State);
  void InitializeState( );
  void ReturnUnstructuredGridActor(const char *name);
  
protected:
	vtkKWMimxEditBBMeshSeedGroup();
	~vtkKWMimxEditBBMeshSeedGroup();
	virtual void CreateWidget();
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkMimxPlaceLocalAxesWidget *LocalAxesWidget;
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWFrameWithLabel *CellSelectionFrame;
  vtkKWFrameWithLabel *MeshSeedTypeFrame;
  vtkKWRadioButton *EditButton;
  vtkKWRadioButton *VtkInteractionButton;
  vtkKWRadioButtonSet *MeshSeedTypeButton;
  vtkKWEntryWithLabel *XMeshSeed;
  vtkKWEntryWithLabel *YMeshSeed;
  vtkKWEntryWithLabel *ZMeshSeed;
  vtkKWEntryWithLabel *AvElementLength;
  vtkKWFrame *ButtonFrame;
  vtkIntArray *OriginalPosition;
  vtkKWFrame *EditTypeChoiceFrame;
 // vtkKWRadioButton *ElementLengthCheckButton;
 // vtkKWRadioButton *NumDivCheckButton;
  vtkKWRadioButtonSet *AxisSelection;
  vtkKWMenuButtonWithLabel *EditTypeSelection;
  vtkKWEntryWithLabel *UserDefinedElementLength;
  vtkKWCheckButtonWithLabel *ShowMeshSeedCheckButton;
  vtkMimxColorCodeMeshSeedActor *ColorCodeMeshSeedActor;
private:
  vtkKWMimxEditBBMeshSeedGroup(const vtkKWMimxEditBBMeshSeedGroup&); // Not implemented
  void operator=(const vtkKWMimxEditBBMeshSeedGroup&); // Not implemented
  
  char PreviousSelection[64];
  double AverageEdgeLength[3];
};

#endif

