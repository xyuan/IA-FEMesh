/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxDefineElSetGroup.h,v $
Language:  C++
Date:      $Date: 2008/08/14 05:01:52 $
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
// .NAME vtkKWMimxDefineElSetGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxDefineElSetGroup_h
#define __vtkKWMimxDefineElSetGroup_h

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
class vtkMimxCreateElementSetWidgetFEMesh;
class vtkMimxMeshActor;
class vtkKWCheckButton;
class vtkWindowCloseEventCallback;
class vtkKWRadioButtonSet;
class vtkKWRadioButton;
class vtkKWComboBox;
class vtkKWScaleWithLabel;
class vtkUnstructuredGrid;
class vtkKWMimxCreateFEMeshFromBBGroup;

class vtkKWMimxDefineElSetGroup : public vtkKWTopLevel
{
public:
  static vtkKWMimxDefineElSetGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxDefineElSetGroup,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  virtual void Update();
  virtual void UpdateEnableState();
  /*
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  */
  vtkSetObjectMacro(ElementSetCombobox, vtkKWComboBox);
  void SetCreateFEMeshFromBBGroup(vtkKWMimxCreateFEMeshFromBBGroup *Group);
  vtkGetObjectMacro(SetLabelEntry, vtkKWEntryWithLabel);
  vtkGetObjectMacro(ApplyButton, vtkKWPushButton);
  vtkGetObjectMacro(ExtrudeFrame, vtkKWFrame);
  vtkGetObjectMacro(ModeFrame, vtkKWFrame);
  vtkSetMacro(DimensionState, int);
  vtkSetMacro(WithdrawOnApply, int);
  vtkGetMacro(WithdrawOnApply, int);
  void SetMimxMainWindow(vtkKWMimxMainWindow *window);
  vtkKWMimxMainWindow *GetMimxMainWindow( );
  vtkUnstructuredGrid* GetSelectedMesh();
  void SelectElementsThroughCallback(int mode);
  void SelectElementsSurfaceCallback(int mode);
  void SelectVisibleElementsSurfaceCallback(int mode);
  void SelectSingleElementCallback(int mode);
  void SelectMultipleElementsCallback(int mode);
  void RemoveSelectionsCallback( );
  int EnableElementSelection(int mode);
  void DisableElementSelection(int mode);
  vtkMimxMeshActor* GetSelectedObject();
  void SetMeshActor( vtkMimxMeshActor *actor );
  void FullCallback();
  void AcceptSelectionCallback();
  void AddElementsCallback();
  void SubtractElementsCallback();
  void CancelButtonCallback();
  void CloseWindowCallback();
  void FullPartialSelectedMeshCallback(int mode);
  int ApplyButtonCallback();
  virtual void Withdraw();
  void OpacityCallback(double Val);
  void ResetState();
  void SetDialogTitle(const char *title);
  void SetModeSurfaceOnly();
  void SetModeAll();
  int GetNumberOfDivisions();
  double GetExtrusionLength();
protected:
	vtkKWMimxDefineElSetGroup();
	~vtkKWMimxDefineElSetGroup();
	virtual void CreateWidget();
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkLinkedListWrapper *ObjectList;
  
  vtkKWCheckButtonSet *SelectButtonSet;
  vtkKWEntryWithLabel *SetLabelEntry;
  vtkKWPushButton *CancelButton;
  vtkKWPushButton *ApplyButton;
  vtkKWFrame *ButtonFrame;
  vtkKWFrame *ModeFrame;
  vtkKWFrame *OperationFrame;
  vtkKWPushButtonSet *MeshModeButtonSet;
  vtkKWPushButton *ClearSelectionButton;
  vtkMimxCreateElementSetWidgetFEMesh *SelectCellsWidget;
  //vtkKWCheckButton *AddSubtractButton;
  vtkWindowCloseEventCallback *WindowCloseEventCallback;
  vtkKWMenuButtonWithLabel *MeshTypeMenuButton;
  vtkKWRadioButtonSet *AddSubtractButton;
  vtkKWComboBox *ElementSetCombobox;
  vtkKWScaleWithLabel *OpacityScale;
  vtkKWFrame *ExtrudeFrame;
  vtkKWEntryWithLabel *ExtrudeLengthEntry;
  vtkKWEntryWithLabel *NumberOfDivisionsEntry;
  vtkKWMimxCreateFEMeshFromBBGroup *CreateFEMeshFromBBGroup;
private:
  vtkKWMimxDefineElSetGroup(const vtkKWMimxDefineElSetGroup&); // Not implemented
  void operator=(const vtkKWMimxDefineElSetGroup&); // Not implemented
  
  int SelectionState;
  int DimensionState;
  bool SurfaceOnlyMode;
  int WithdrawOnApply;
  vtkMimxMeshActor *MeshActor;  
};

#endif

