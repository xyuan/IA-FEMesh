/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxConstMatPropElSetGroup.h,v $
Language:  C++
Date:      $Date: 2008/05/29 20:32:20 $
Version:   $Revision: 1.6 $

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
// .NAME vtkKWMimxConstMatPropElSetGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxConstMatPropElSetGroup_h
#define __vtkKWMimxConstMatPropElSetGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxMainWindow.h"

class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWPushButton;
class vtkKWMimxDefineElSetGroup;
class vtkKWMimxViewPropertiesOptionGroup;

class vtkKWMimxConstMatPropElSetGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxConstMatPropElSetGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxConstMatPropElSetGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void ConstMatPropElSetCancelCallback();
  int ConstMatPropElSetApplyCallback();
  void UpdateObjectLists();
  void SelectionChangedCallback(const char *Selection);
  void ElementSetChangedCallback(const char *Selection);
  //void ViewMaterialPropertyCallback( int mode );
  //int ClippingPlaneCallback(int mode);
  //void ViewPropertyLegendCallback( int mode );
  //void ScalarBarRangeComputationCallback( int mode );
  //void ViewElementSetCallback(int mode);
  void DefineElementSetCallback( );
  void ViewOptionsCallback();
  
protected:
	vtkKWMimxConstMatPropElSetGroup();
	~vtkKWMimxConstMatPropElSetGroup();
	virtual void CreateWidget();

  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWComboBoxWithLabel *ElementSetComboBox;
  vtkKWEntryWithLabel *YoungsModulusEntry;
  vtkKWEntryWithLabel *PoissonsRatioEntry;
  //vtkKWFrameWithLabel *ViewFrame;
  //vtkKWCheckButtonWithLabel *ViewPropertyButton;
  //vtkKWCheckButtonWithLabel *ViewLegendButton;
  //vtkKWCheckButtonWithLabel *ScalarBarRangeComputationButton;
  //vtkKWMenuButtonWithLabel *ClippingPlaneMenuButton;
  //vtkKWCheckButtonWithLabel *ViewElementSetMenuButton;
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWPushButton *DefineElSetButton;
  vtkKWPushButton *ViewOptionsButton;
  vtkKWMimxDefineElSetGroup *DefineElementSetDialog;
  char elementSetSelectionPrevious[64];
  void RemovePreviousSelectionDisplay();
  vtkKWMimxViewPropertiesOptionGroup *ViewOptionsGroup;
private:
  vtkKWMimxConstMatPropElSetGroup(const vtkKWMimxConstMatPropElSetGroup&); // Not implemented
  void operator=(const vtkKWMimxConstMatPropElSetGroup&); // Not implemented
  
  char meshName[64];
  char elementSetName[64];
};

#endif

