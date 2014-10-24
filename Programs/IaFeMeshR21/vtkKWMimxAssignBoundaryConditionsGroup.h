/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxAssignBoundaryConditionsGroup.h,v $
Language:  C++
Date:      $Date: 2008/08/09 14:41:45 $
Version:   $Revision: 1.25 $

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
// .NAME vtkKWMimxAssignBoundaryConditionsGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxAssignBoundaryConditionsGroup_h
#define __vtkKWMimxAssignBoundaryConditionsGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxMainWindow.h"

class vtkActorCollection;
class vtkCollection;
class vtkStringArray;
class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWFrame;
class vtkKWNotebook;
class vtkKWMenuButton;
class vtkGlyph3D;
class vtkKWMimxDefineNodeSetGroup;
class vtkKWTopLevel;
class vtkKWTreeWithScrollbars;
class vtkMimxBoundaryConditionActor;
class vtkKWScale;
class vtkKWSpinBoxWithLabel;
class vtkKWComboBox;
class vtkKWLabel;
class vtkKWEntry;

class vtkKWMimxAssignBoundaryConditionsGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxAssignBoundaryConditionsGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxAssignBoundaryConditionsGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  //void AssignBoundaryConditionsDoneCallback();
  void AssignBoundaryConditionsCancelCallback();
  int AssignBoundaryConditionsApplyCallback();
  void UpdateObjectLists();
  void SelectionChangedCallback(const char *Selection);
  void NodeSetChangedCallback(const char *Selection);
  void BoundaryConditionTypeSelectionChangedCallback(const char *Selection);
  void StepNumberChangedCallback(const char *StepNum);
  void AddStepNumberCallback();
  void ViewBoundaryConditionsCallback(int Mode);	// Default for Force
  void DeleteStepNumberCallback();
  void GlyphSizeChangedCallback(const char *Entry);
  int  GetNotebookStep();
  void DefineNodeSetCallback();
  void SummarizeCallback();
  void DialogCancelCallback();
  void SummaryDialogCancelCallback();
  void UpdateVisibility(int Mode);
  void UpdateBCVisibilityList( );
  void OpacityCallback(double Val);
  void StepControlCallback();
  void NodePrintCallback(int mode);
  void NodeOutputCallback(int mode);
  void ElementPrintCallback(int mode);
  void ElementOutputCallback(int mode);
  void DialogApplyCallback();
  vtkUnstructuredGrid* IsMeshValid();
  void NodeSetOutputApplyCallback();
  void ElementSetOutputApplyCallback();
  void GenerateStepParameters(int StepNum);
  void AssignValuesToStepWindow(int StepNum);
  vtkStringArray* GetLowerStepWithParameters(int StepNum, vtkUnstructuredGrid *ugrid);
  void OutputElementSetChangedCallback(const char *Entry);
  void OutputNodeSetChangedCallback(const char *Entry);
  void CopyLowerStepNodeAndElementOutput(int StepNum, vtkUnstructuredGrid *Ugrid);
protected:
	vtkKWMimxAssignBoundaryConditionsGroup();
	~vtkKWMimxAssignBoundaryConditionsGroup();
	int GetValue();
	double GetValue(int Direction, const char *BoundaryCondition, const char *NodeSetName);
	virtual void CreateWidget();
	virtual void ProcessCallbackCommandEvents(
    vtkObject *caller, unsigned long event, void *calldata);
    
	vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWPushButton *DefineNodeSetPushButton;
  vtkKWNotebook *StepNotebook;
  vtkKWComboBox *NodeSetComboBox;
  vtkKWMenuButton *BoundaryConditionTypeComboBox;
  vtkKWFrame *StepFrame;
  vtkKWFrame *DirectionFrame;
  vtkKWEntryWithLabel *DirectionXEntry;
  vtkKWEntryWithLabel *DirectionYEntry;
  vtkKWEntryWithLabel *DirectionZEntry;
  vtkKWEntryWithLabel *GlyphSizeEntry;
  vtkKWComboBoxWithLabel *StepNumberComboBox;
  vtkKWPushButton *AddStepPushButton;
  vtkKWPushButton *DeleteStepPushButton;
  vtkKWCheckButtonWithLabel *ViewBoundaryConditionsButton;
  vtkKWPushButton *SummaryButton;
  vtkKWMimxDefineNodeSetGroup *DefineNodeSetDialog;
  vtkKWTopLevel *SummaryDialog;
  vtkKWTreeWithScrollbars *BCTree;
  vtkKWPushButton *CancelDialogButton;
  vtkKWFrame *InfoFrame;
  vtkKWFrame *ButtonFrame;
  vtkKWLabel *NodeSetLabel;
  vtkKWLabel *SubHeadingLabel;
  vtkKWEntry *SubHeadingEntry;
  vtkKWFrame *SubHeadingFrame;
  
  vtkKWTopLevel *StepControlDialog;
  vtkKWLabel *StepParameterLabel;
  vtkKWFrameWithLabel *StepParameterFrame;
  vtkKWEntryWithLabel *StepIncrementEntry;
  vtkKWCheckButtonWithLabel *NlGeomButton;
  vtkKWCheckButtonWithLabel *UnsymmButton;
  vtkKWMenuButton *AnalysisTypeMenu;
  vtkKWEntryWithLabel *TimeIncrementEntry;
  vtkKWCheckButtonWithLabel *NodePrintButton;
  vtkKWFrameWithLabel *NodePrintFrame;
  vtkKWComboBoxWithLabel *NodeSetMenu;
  vtkKWEntryWithLabel *NodePrintFrequencyEntry;
  vtkKWCheckButtonWithLabel *NodePrintSummaryButton;
  vtkKWCheckButtonWithLabel *NodePrintTotalButton;
  vtkKWFrameWithLabel *ElementPrintFrame;
  vtkKWCheckButtonWithLabel *ElementPrintButton;
  vtkKWComboBoxWithLabel *ElementSetMenu;
  vtkKWMenuButtonWithLabel *ElementPositionSetMenu;
  vtkKWEntryWithLabel *ElementPrintFrequencyEntry;
  vtkKWMenuButtonWithLabel *ElementPrintPositionMenu;
  vtkKWCheckButtonWithLabel *ElementPrintSummaryButton;
  vtkKWCheckButtonWithLabel *ElementPrintTotalButton;
  vtkKWCheckButtonWithLabel *NodeOutputButton;
  vtkKWCheckButtonWithLabel *ElementOutputButton;
  vtkKWMenuButtonWithLabel *ElementOutputPositionMenu;
  vtkKWPushButton *StepControlApplyButton;
  vtkKWPushButton *NodeSetOutputApplyButton;
  vtkKWPushButton *ElementSetOutputApplyButton;
  vtkKWPushButton *StepControlCancelButton;
  vtkKWPushButton *DefineControlPushButton;
  vtkKWEntryWithLabel *NodePrintVariablesEntry;
  vtkKWEntryWithLabel *NodeOutputVariablesEntry;
  vtkKWEntryWithLabel *ElementPrintVariablesEntry;
  vtkKWEntryWithLabel *ElementOutputVariablesEntry;
  vtkKWMenuButtonWithLabel *AmplitudeTypeMenu;
  
  void ConcatenateStrings(const char*, const char*, 
  						const char*, const char*, const char*, char*);
  int IsStepEmpty(vtkUnstructuredGrid *ugrid);
  void DeleteBoundaryConditionActors();
  void CreateBoundaryConditionActors();
  void CreateBoundaryConditionActor(const char *NodeSetName, const char *BoundaryConditionType);
  void DeleteBoundaryConditionActor(const char *NodeSetName, const char *BoundaryConditionType);
  void ModifyBoundaryConditionActor(const char *NodeSetName, const char *BoundaryConditionType);
  void ModifyBoundaryConditionActors();
  int DoesBoundaryConditionExist(const char *NodeSetName, const char *BoundaryConditionType);
  void ShowBoundaryConditionRepresentation(int ConditionNum);
  void HideBoundaryConditionRepresentation(int ConditionNum);

  void ComputeBoundaryCondRepresentation( const char*, const char*, double xValue, 
	  double yValue, double zValue);
//  void ComputeBoundaryCondRepresentation( const char*, const char*, double XValue, double YValue, double ZValue);
  int CancelStatus;
  ////Force
  //vtkActor *ForceGlyphActorX;
  //vtkActor *ForceGlyphActorY;
  //vtkActor *ForceGlyphActorZ;
  //vtkGlyph3D *ForceGlyphX;
  //vtkGlyph3D *ForceGlyphY;
  //vtkGlyph3D *ForceGlyphZ;
  ////Displacement
  //vtkActor *DisplacementGlyphActorX;
  //vtkActor *DisplacementGlyphActorY;
  //vtkActor *DisplacementGlyphActorZ;
  //vtkGlyph3D *DisplacementGlyphX;
  //vtkGlyph3D *DisplacementGlyphY;
  //vtkGlyph3D *DisplacementGlyphZ;
  ////Rotation
  //vtkActor *RotationGlyphActorX;
  //vtkActor *RotationGlyphActorY;
  //vtkActor *RotationGlyphActorZ;
  //vtkGlyph3D *RotationGlyphX;
  //vtkGlyph3D *RotationGlyphY;
  //vtkGlyph3D *RotationGlyphZ;
  ////Moment
  //vtkActor *MomentGlyphActorX;
  //vtkActor *MomentGlyphActorY;
  //vtkActor *MomentGlyphActorZ;
  //vtkGlyph3D *MomentGlyphX;
  //vtkGlyph3D *MomentGlyphY;
  //vtkGlyph3D *MomentGlyphZ;
  //
  vtkCollection *ActorCollection;
  vtkCollection *GlyphCollection;
  vtkKWFrame *GlyphFrame;
  vtkKWScale *MeshOpacityScale;
  vtkKWMultiColumnListWithScrollbars *MultiColumnList;
  vtkKWSpinBoxWithLabel *OpacityThumbwheel;
  vtkKWFrameWithLabel *ViewFrame;
  double GlyphSize;
  char nodeSetSelectionPrevious[256];
  char stepNumPrevious[64];
  char boundaryConditionTypePrevious[64];
  double sizePrevious;
  double factorX;
  double factorY;
  double factorZ;
//vtkKWComboBoxWithLabel *ViewDirectionComboBox;
  vtkStringArray *NodeSetNameArray;
  vtkStringArray *BoundaryConditionTypeArray;
  vtkStringArray *BoundaryConditionArray;
private:
  vtkKWMimxAssignBoundaryConditionsGroup(const vtkKWMimxAssignBoundaryConditionsGroup&); // Not implemented
  void operator=(const vtkKWMimxAssignBoundaryConditionsGroup&); // Not implemented
  
  vtkMimxBoundaryConditionActor *BoundaryConditionActor;
  int startNotebookPage;
  int endNotebookPage;
  int currentNotebookPage;
};

#endif

