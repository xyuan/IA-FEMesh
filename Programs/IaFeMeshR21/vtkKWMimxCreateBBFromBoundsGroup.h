/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateBBFromBoundsGroup.h,v $
Language:  C++
Date:      $Date: 2008/08/17 22:30:24 $
Version:   $Revision: 1.13.4.1 $

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
// .NAME vtkKWMimxCreateBBFromBoundsGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxCreateBBFromBoundsGroup_h
#define __vtkKWMimxCreateBBFromBoundsGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxMainWindow.h"

class vtkKWComboBoxWithLabel;
class vtkKWFrameWithLabel;
class vtkKWFrame;
class vtkKWRadioButton;
class vtkKWEntryWithLabel;
class vtkMimxCreateBuildingBlockFromPickWidget;
class vtkUnstructuredGrid;
class vtkRightButtonBBManualCreateCallback;

class vtkKWMimxCreateBBFromBoundsGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxCreateBBFromBoundsGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxCreateBBFromBoundsGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void SurfaceBoundCallback( );
  void ManualBoundCallback( );
  int CreateBBFromBoundsApplyCallback();
  void CreateBBFromBoundsCancelCallback();
  void UpdateObjectLists();
  void EnableRubberBandPick( vtkPolyData *surface, double extrusionLength );
  void DisableRubberBandPick();
  void ExtrusionLengthChangedCallback(const char *value);
  void AddBuildingBlockToDisplay(
	  vtkUnstructuredGrid *ugrid, const char *namePrefix, const char *foundationName, vtkIdType count);
  
protected:
	vtkKWMimxCreateBBFromBoundsGroup();
	~vtkKWMimxCreateBBFromBoundsGroup();
	virtual void CreateWidget();
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWFrame *ModeFrame;
  vtkKWRadioButton *SurfaceBoundButton;
  vtkKWRadioButton *ManualBoundButton;
  vtkKWFrame *EntryFrame;
  vtkKWEntryWithLabel *ExtrusionLengthEntry;
  vtkMimxCreateBuildingBlockFromPickWidget *CreateBuildingBlockFromPickWidget;
  vtkRightButtonBBManualCreateCallback *RightButtonPressCallback;
  int MaualBBCreationCount;
private:
  vtkKWMimxCreateBBFromBoundsGroup(const vtkKWMimxCreateBBFromBoundsGroup&); // Not implemented
  void operator=(const vtkKWMimxCreateBBFromBoundsGroup&); // Not implemented
 };

#endif

