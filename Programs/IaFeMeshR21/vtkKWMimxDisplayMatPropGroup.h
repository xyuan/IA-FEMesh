/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxDisplayMatPropGroup.h,v $
Language:  C++
Date:      $Date: 2008/07/22 02:00:09 $
Version:   $Revision: 1.4 $

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
// .NAME vtkKWMimxDisplayMatPropGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxDisplayMatPropGroup_h
#define __vtkKWMimxDisplayMatPropGroup_h

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


class vtkKWMimxDisplayMatPropGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxDisplayMatPropGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxDisplayMatPropGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  void InvertPlaneCallback(int mode);
  void ClippingPlaneCallback(int mode);
  void ViewPropertyLegendCallback( int mode );
  //void ViewElementSetCallback(int mode);
  
  void DisplayCancelCallback();
  void SelectionChangedCallback(const char *Selection);
  void ElementSetChangedCallback(const char *Selection);
  void DisplayOptionsCallback( );
  
protected:
	vtkKWMimxDisplayMatPropGroup();
	~vtkKWMimxDisplayMatPropGroup();
	virtual void CreateWidget();
	void RemovePreviousSelectionDisplay();

  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWComboBoxWithLabel *ElementSetComboBox;
  vtkKWFrame *ViewFrame;
  vtkKWCheckButton *ViewLegendButton;
  vtkKWCheckButton *EnablePlaneButton;
  vtkKWCheckButtonWithLabel *InvertPlaneButton;
  vtkKWPushButton *DisplayOptionsButton;
  vtkKWMimxViewPropertiesOptionGroup *ViewOptionsGroup;
  
private:
  vtkKWMimxDisplayMatPropGroup(const vtkKWMimxDisplayMatPropGroup&); // Not implemented
  void operator=(const vtkKWMimxDisplayMatPropGroup&); // Not implemented
  
  char meshName[64];
  char elementSetName[64];
  char elementSetSelectionPrevious[64];
};

#endif

