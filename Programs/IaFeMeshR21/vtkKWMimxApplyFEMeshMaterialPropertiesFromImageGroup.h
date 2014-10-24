/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.h,v $
Language:  C++
Date:      $Date: 2008/08/03 20:20:24 $
Version:   $Revision: 1.13 $

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
// .NAME vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup_h
#define __vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

class vtkKWComboBoxWithLabel;
class vtkIntArray;
class vtkKWEntryWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMimxDefineElSetGroup;

class vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void ApplyFEMeshMaterialPropertiesFromImageDoneCallback();
  int ApplyFEMeshMaterialPropertiesFromImageApplyCallback();
  void ApplyFEMeshMaterialPropertiesFromImageCancelCallback();
  void UpdateObjectLists();
  void FEMeshSelectionChangedCallback(const char *Selection);
  void ElementSetChangedCallback(const char *Selection);
  void DefineElementSetCallback();
  void BinCheckButtonCallback(int mode);
  void SpecifyRangeButtonCallback(int mode);
  int DefineConversionCallback();
  int ConversionApplyCallback();
  int ConversionCancelCallback();
  /*
  void ViewMaterialPropertyCallback( int mode );
  int ClippingPlaneCallback(int mode);
  void ViewPropertyLegendCallback( int mode );
  */
protected:
	vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup();
	~vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup();
	virtual void CreateWidget();
  vtkKWComboBoxWithLabel *ImageListComboBox;
  vtkKWComboBoxWithLabel *FEMeshListComboBox;
  vtkKWComboBoxWithLabel *ElementSetComboBox;
  vtkKWEntryWithLabel *PoissonsRatioEntry;
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWPushButton *DefineElSetButton;
  vtkKWMimxDefineElSetGroup *DefineElementSetDialog;
  vtkKWCheckButton *BinCheckButton;
  vtkKWFrameWithLabel *BinFrame;
  vtkKWCheckButtonWithLabel *SpecifyRangeButton;
  vtkKWEntryWithLabel *MinimumBinEntry;
  vtkKWEntryWithLabel *MaximumBinEntry;
  vtkKWEntryWithLabel *NumOfBinsEntry;
  vtkKWFrameWithLabel *ImageConstantFrame;
  vtkKWEntryWithLabel *ImageConstantA;
  vtkKWEntryWithLabel *ImageConstantB;
  vtkKWEntryWithLabel *ImageConstantC;
  vtkKWTopLevel *ConversionDialog;
  vtkKWLabel *EquationLabel;
  vtkKWPushButton *DialogApplyButton;
  vtkKWPushButton *DialogCancelButton;
  vtkKWPushButton *DefineConversionButton;
  vtkKWMenuButtonWithLabel *ImageConversionType;
  /*
  vtkKWFrameWithLabel *ViewFrame;
  vtkKWCheckButtonWithLabel *ViewPropertyButton;
  vtkKWCheckButtonWithLabel *ViewLegendButton;
  vtkKWMenuButtonWithLabel *ClippingPlaneMenuButton;
  */
  char elementSetPrevious[256];
private:
  vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup(const vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup&); // Not implemented
  void operator=(const vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup&); // Not implemented
  
  char meshName[64];
  char elementSetName[64];
  double ConversionA;
  double ConversionB;
  double ConversionC;
  char ConversionType[64];
  
 };

#endif

