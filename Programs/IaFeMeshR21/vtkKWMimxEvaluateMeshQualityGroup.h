/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxEvaluateMeshQualityGroup.h,v $
Language:  C++
Date:      $Date: 2008/08/14 05:01:52 $
Version:   $Revision: 1.15 $

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

#ifndef __vtkKWMimxEvaluateMeshQualityGroup_h
#define __vtkKWMimxEvaluateMeshQualityGroup_h

#include "vtkKWWindow.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkKWMimxGroupBase.h"

#include <string>
// include definition of custom class for mesh quality widget


class vtkKWRenderWidget;
class vtkKWScale;
class vtkKWCheckButton;
class vtkKWRadioButton;
class vtkKWRadioButtonSet;
class vtkKWPushButtonSet;
class vtkKWListBoxWithScrollbars;
class vtkKWThumbWheel;
class vtkKWRange;
class vtkKWCheckButtonWithLabel;
class vtkKWText;
class vtkKWComboBoxWithLabel;
class vtkOrientationMarkerWidget;
class vtkKWLoadSaveButton;
class vtkIntArray;
class vtkKWEntryWithLabel;
class vtkKWPushButton;
class vtkKWDialog;
class vtkKWFrame;
class vtkDoubleArray;
class vtkIdList;
class vtkKWLoadSaveDialog;
class vtkKWMenuButtonWithLabel;
class vtkKWLabel;
class vtkKWMimxViewPropertiesOptionGroup;

#define MESH_QUALITY_VOLUME    1
#define MESH_QUALITY_EDGE      2
#define MESH_QUALITY_JACOBIAN  3
#define MESH_QUALITY_SKEW      4
#define MESH_QUALITY_ANGLE     5
#define MESH_QUALITY_MIN_ANGLE 6
#define MESH_QUALITY_MAX_ANGLE 7




class vtkKWMimxEvaluateMeshQualityGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxEvaluateMeshQualityGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxEvaluateMeshQualityGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  void UpdateObjectLists();

  
  // Description: Callback functions for User Interface

  int EvaluateMeshQualityCancelCallback();
  int EvaluateMeshQualityApplyCallback(int qualityType);
  int ViewDistortedElemenetsCallback();
  int DistortedElementDialogCancelCallback();
  int DistortedElementDialogSaveCallback();
  
  int InvertPlaneCallback(int mode);
  int ClippingPlaneCallback(int mode);
  int ViewQualityLegendCallback(int mode);
  
  int SetQualityTypeToVolume() {QualityType = MESH_QUALITY_VOLUME;
  return 1;};
  int SetQualityTypeToEdgeCollapse() {QualityType = MESH_QUALITY_EDGE;
  return 1;};
  int SetQualityTypeToJacobian() {QualityType = MESH_QUALITY_JACOBIAN;
  return 1;};
  int SetQualityTypeToSkew() {QualityType = MESH_QUALITY_SKEW;
  return 1;};
  int SetQualityTypeToAngle() {QualityType = MESH_QUALITY_ANGLE;
  return 1;};

  void ClearStatsEntry();  
  void SelectionChangedCallback(const char* selection);
  void DisplayOptionsCallback(); 
protected:
  vtkKWMimxEvaluateMeshQualityGroup();
  ~vtkKWMimxEvaluateMeshQualityGroup();
  
  vtkKWComboBoxWithLabel *MeshListComboBox;
  vtkIntArray *OriginalPosition;
  
  // Description:
  // Create the widget.
  virtual void CreateWidget();
 
  //vtkKWFrameWithLabel *ElementSizeFrame;
  //vtkKWFrameWithLabel *ElementOpacityFrame;
  //vtkKWFrameWithLabel *PlaneControlFrame;
  //vtkKWFrameWithLabel *ReportFrame;
  vtkKWFrameWithLabel *DisplayFrame;
  vtkKWFrame *ViewFrame;
  vtkKWFrameWithLabel *ComponentFrame;
  //vtkKWScale *SliceScale; 
  //vtkKWCheckButtonWithLabel *OverrideColorButton;
  //vtkKWRange *ElementColorRange; 
  //vtkKWScale *ElementOpacity; 

  //vtkKWRadioButtonSet *PlaneSelectionControls;
  //vtkKWCheckButton *InvertSelector;
  //vtkKWCheckButtonWithLabel *HighlightCellsButton;
  //vtkKWCheckButtonWithLabel *RealTimeWarningButton;
  //vtkKWCheckButton *OutlineSelector;
  //vtkKWCheckButton *ClippedOutlineSelector;
  //vtkKWCheckButton *InteriorOutlineSelector;
  //vtkKWCheckButton *FilledElementSelector;
  //vtkKWCheckButton *HedgehogSelector;
  //vtkKWCheckButton *OrientationCueSelector;
  //vtkKWComboBoxWithLabel *DecimalDisplayPrecisionWidget;
  vtkKWEntryWithLabel *NumberOfDistortedEntry;
  vtkKWEntryWithLabel *NumberOfElementsEntry;
  vtkKWEntryWithLabel *QualityMinimumEntry;
  vtkKWEntryWithLabel *QualityMaximumEntry;
  vtkKWEntryWithLabel *QualityAverageEntry;
  vtkKWEntryWithLabel *QualityVarianceEntry;
  vtkKWPushButton *SaveButton;
  vtkKWCheckButton *ViewQualityButton;
  vtkKWCheckButton *ClippingPlaneButton;
  vtkKWCheckButton *ViewLegendButton;
  vtkKWCheckButtonWithLabel *InvertPlaneButton;
  vtkKWTopLevel *DistortedElementDialog;
  vtkKWFrame *DistortedButtonFrame;
  vtkKWPushButton *SaveDistortedButton;
  vtkKWPushButton *CancelDistortedButton;
  vtkKWListBoxWithScrollbars *DistortedElementsReport;
  vtkKWLoadSaveDialog *FileBrowserDialog;
  vtkKWFrame *ButtonFrame;
  vtkKWFrame *SummaryFrame;
  vtkKWFrame *SummaryReviewFrame;
  vtkKWMenuButtonWithLabel *QualityTypeButton;
  vtkKWLabel *QualityTypeLabel;
  vtkKWLabel *DistoredListLabel;
  vtkKWPushButton *DisplayOptionsButton;
  vtkKWMimxViewPropertiesOptionGroup* ViewOptionsGroup;
private:
  vtkKWMimxEvaluateMeshQualityGroup(const vtkKWMimxEvaluateMeshQualityGroup&);   // Not implemented.
  void operator=(const vtkKWMimxEvaluateMeshQualityGroup&);  // Not implemented.
  
  vtkIdList *DistortedElementList;
  vtkDoubleArray *DistortedMeshQuality;
  int NumberOfCells;
  int QualityType;
  char meshName[64];
  char qualityName[64];
  char PreviousSelection[64];
  double minimumQuality;
  double maximumQuality;
  double averageQuality;
  double varianceQuality;
  //void AddOrientationWidget(void);
  
};

#endif
