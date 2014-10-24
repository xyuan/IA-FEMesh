/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewPropertiesOptionGroup.h,v $
Language:  C++
Date:      $Date: 2008/06/11 21:47:27 $
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
// .NAME vtkKWMimxViewPropertiesOptionGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxViewPropertiesOptionGroup_h
#define __vtkKWMimxViewPropertiesOptionGroup_h

#include "vtkKWTopLevel.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxMeshActor.h"

class vtkKWCheckButtonWithLabel;
class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWEntryWithLabel;
class vtkKWPushButton;
class vtkKWMenuButtonWithLabel;
class vtkKWComboBoxWithLabel;

class vtkKWMimxViewPropertiesOptionGroup : public vtkKWTopLevel
{
public:
  static vtkKWMimxViewPropertiesOptionGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxViewPropertiesOptionGroup,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  virtual void Update();
  virtual void UpdateEnableState();
  //void ViewMaterialPropertyCallback(int mode);
  //void ViewPropertyLegendCallback(int mode);
  //void ViewElementSetCallback(int mode);
  void ColorModeCallback(int mode);
  void UpdateRangeCallback();
  void SpecifyRangeCallback(int mode);
  void SpecifyTitleCallback(int mode);
  void DeselectAllButtons();
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(MeshActor, vtkMimxMeshActor);
  vtkSetStringMacro(ElementSetName); 
  vtkSetStringMacro(ArrayName);
  vtkGetObjectMacro(DecimalPrecisionWidget, vtkKWComboBoxWithLabel);
  vtkGetObjectMacro(SpecifyTitleButton, vtkKWCheckButtonWithLabel);
  vtkGetObjectMacro(LegendTitle, vtkKWEntryWithLabel);
  void SetDecimalPrecisionCallback( char *value );
  void SetLegendTitleCallback ( char *title );
  void ResetValues();
protected:
	vtkKWMimxViewPropertiesOptionGroup();
	~vtkKWMimxViewPropertiesOptionGroup();
	virtual void CreateWidget();
  
private:
  vtkKWMimxViewPropertiesOptionGroup(const vtkKWMimxViewPropertiesOptionGroup&); // Not implemented
  void operator=(const vtkKWMimxViewPropertiesOptionGroup&); // Not implemented
  
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkMimxMeshActor *MeshActor;  
  char *ElementSetName;
  char *ArrayName;
  
  vtkKWFrame *OptionsFrame;
  vtkKWMenuButtonWithLabel *ColorMenuButton;
  vtkKWFrameWithLabel *RangeFrame;
  vtkKWCheckButtonWithLabel *SpecifyRangeButton;
  vtkKWEntryWithLabel *RangeMinimum;
  vtkKWEntryWithLabel *RangeMaximum;
  vtkKWPushButton *UpdateRange;
  
  vtkKWFrameWithLabel *LegendFrame;
  vtkKWComboBoxWithLabel *DecimalPrecisionWidget;
  vtkKWCheckButtonWithLabel *SpecifyTitleButton;
  vtkKWEntryWithLabel *LegendTitle;
  vtkKWPushButton *CloseButton;
  
  double* GetRange();
};

#endif

