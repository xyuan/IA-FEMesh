/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxDisplayPropertiesGroup.h,v $
Language:  C++
Date:      $Date: 2008/04/28 02:59:24 $
Version:   $Revision: 1.9 $

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
// .NAME vtkKWMimxDisplayPropertiesGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxDisplayPropertiesGroup_h
#define __vtkKWMimxDisplayPropertiesGroup_h

#include "vtkKWTopLevel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

class vtkKWChangeColorButton;
class vtkKWFrameWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWThumbWheel;
class vtkLinkedListWrapper;
class vtkKWFrameWithScrollbar;
class vtkLinkedListWrapperTree;
class vtkKWMultiColumnList;
class vtkKWSpinBoxWithLabel;
class vtkKWPushButtonWithLabel;
class vtkKWLoadSaveDialog;
class vtkKWDialog;
class vtkKWPushButton;
class vtkKWPushButtonSet;
class vtkKWRadioButtonSet;

class vtkKWMimxDisplayPropertiesGroup : public vtkKWTopLevel
{
public:
  static vtkKWMimxDisplayPropertiesGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxDisplayPropertiesGroup,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
 // const char* GetActivePage();	// return the name of the chosen page
 // int GetActiveOption();	// return the chosen operation
  virtual void Update();
  virtual void UpdateEnableState();
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  
  virtual void SetBackGroundColor(double color[3]);
  void WindowShapshot();
  void PositiveXViewCallback();
  void PositiveYViewCallback();
  void PositiveZViewCallback();
  void NegativeXViewCallback();
  void NegativeYViewCallback();
  void NegativeZViewCallback();
  void PerspectiveViewCallback();
  void ParallelViewCallback();
  void ResetViewCallback();
  void ShowLocalAxesActor( );
  
  void SetTextColor(double *);
  void SetTextColor( double red, double green, double blue );
  double *GetTextColor( );
   
protected:
	vtkKWMimxDisplayPropertiesGroup();
	~vtkKWMimxDisplayPropertiesGroup();
	virtual void CreateWidget();
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkLinkedListWrapper *ObjectList;
  vtkKWFrameWithLabel *BackGroundFrame;
  vtkKWFrameWithLabel *CameraFrame;
  vtkKWChangeColorButton *BackGroundColorButton;
  vtkKWPushButtonSet *ViewAxisChoiceButton;
  vtkKWLoadSaveDialog *LoadSaveDialog;
  vtkKWTopLevel *DisplayPropertiesDialog;
  vtkKWPushButton *CloseButton;
  vtkKWChangeColorButton *TextColorButton;
  double TextColor[3];
  
   //int DataType;
private:
  vtkKWMimxDisplayPropertiesGroup(const vtkKWMimxDisplayPropertiesGroup&); // Not implemented
  void operator=(const vtkKWMimxDisplayPropertiesGroup&); // Not implemented
 };

#endif

