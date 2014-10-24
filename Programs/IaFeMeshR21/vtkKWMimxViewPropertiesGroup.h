/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewPropertiesGroup.h,v $
Language:  C++
Date:      $Date: 2008/07/04 21:43:26 $
Version:   $Revision: 1.11 $

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
// .NAME vtkKWMimxViewPropertiesGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxViewPropertiesGroup_h
#define __vtkKWMimxViewPropertiesGroup_h

#include "vtkKWTopLevel.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
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
class vtkKWMultiColumnListWithScrollbars;
class vtkKWSpinBoxWithLabel;
class vtkKWScaleWithEntry;
class vtkMeshQualityClass;
class vtkKWCheckButtonWithLabel;
class vtkKWRange;
class vtkPlane;
class vtkPlaneWidget;
class vtkKWRadioButtonSet;
class vtkPlaneWidgetEventCallback;
class vtkKWCheckButton;
class vtkKWPushButton;
class vtkKWDialog;
class vtkKWLabel;

class vtkKWMimxViewPropertiesGroup : public vtkKWTopLevel
{
public:
  static vtkKWMimxViewPropertiesGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxViewPropertiesGroup,vtkKWTopLevel);
  void PrintSelf(ostream& os, vtkIndent indent);
 // const char* GetActivePage();	// return the name of the chosen page
 // int GetActiveOption();	// return the chosen operation
  virtual void Update();
  virtual void UpdateEnableState();
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(DoUndoTree, vtkLinkedListWrapperTree);
  vtkSetObjectMacro(MultiColumnList, vtkKWMultiColumnListWithScrollbars);
  //vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  //vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  //vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  //vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);

 // vtkGetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkGetObjectMacro(MainFrame, vtkKWFrameWithLabel);
  //vtkSetMacro(DataType, int);	// 0 for curve, 1 for surface, 2 for building-block
								// 3 for FE mesh
  virtual void SetActorColor(double color[3]);
  virtual void SetOutlineColor(double color[3]);
  virtual void SetElementSetColor(double color[3]);
  void RepresentationCallback(int type);
  void SetElementSizeFromScaleCallback(double scale);
  void LineWidthCallback(double width);
  void OpacityCallback(double width);
  vtkSetObjectMacro(ObjectList, vtkLinkedListWrapper);
  void SetViewProperties();
  //void UpdateVisibility();
  //void UpdateNodeAndElementData();
  //void SetOverrideColorRangeFromButton(int state);
  //void SetColorRangeCallback(double low, double high);
  vtkGetMacro(ElementsHighlighted,int);
  //void EnableSelectionPlaneCallback(int);
  //void ClearSelectionPlaneCallback();
  //void SetInvertSelectionFromButton(int state);
  void DisplayModeCallback(int mode);
  void ElementSetVisibilityCallback(int index, int flag);
  void ElementSetDisplayPropertyCallback( );
  void SetMeshDisplayPropertyCallback();
  void SetElementSetList();
  void SetItemName(const char *item);
  void SetItemId(const char *id);
  int GetItemRow( );
  vtkSetMacro(SelectionRow, int);
  
protected:
	vtkKWMimxViewPropertiesGroup();
	~vtkKWMimxViewPropertiesGroup();
	virtual void CreateWidget();
  vtkKWMultiColumnListWithScrollbars *MultiColumnList;
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkLinkedListWrapper *ObjectList;
 /* vtkLinkedListWrapper *BBoxList;
  vtkLinkedListWrapper *FEMeshList;
  vtkLinkedListWrapper *ImageList;*/
  vtkKWFrameWithLabel *MainFrame;
  vtkKWFrameWithLabel *PropertyFrame;
  vtkKWFrameWithLabel *ColorFrame;
  vtkKWFrameWithLabel *StyleFrame;
  vtkKWFrameWithLabel *NodeDataFrame;
  vtkKWFrameWithLabel *ElementDataFrame;
  vtkKWChangeColorButton *ActorColorButton;
  vtkKWChangeColorButton *ElementDataColorButton;
  vtkKWMenuButtonWithLabel *DisplayStyleMenuButton;
  vtkKWMenuButtonWithLabel *NodeDataMenuButton;
  vtkKWMenuButtonWithLabel *ElementDataMenuButton;
  vtkKWMenuButtonWithLabel *DisplayLabels;
  vtkKWSpinBoxWithLabel *LinewidthThumbwheel;
  vtkKWSpinBoxWithLabel *OpacityThumbwheel;
  vtkLinkedListWrapperTree *DoUndoTree;
  vtkKWFrameWithLabel *ElementSizeFrame;
  vtkKWSpinBoxWithLabel *ElementSizeScale;
  vtkMeshQualityClass *MeshQualityClass;
  vtkKWCheckButtonWithLabel *OverrideColorButton;
  vtkKWRange *ElementColorRange;
  vtkPlane *SavedImplicitPlane;
  vtkPlaneWidget *SavedPlaneWidget;
  int ElementsHighlighted;
  vtkKWFrameWithLabel *PlaneControlFrame;
  vtkKWCheckButton *InvertSelector;
  vtkKWCheckButton *EnablePlaneButton;
  vtkPlaneWidgetEventCallback *PlaneMoveCallback;
  vtkUnstructuredGrid *InInput;
  vtkKWDialog *ViewPropertiesDialog;
  vtkKWPushButton *CloseButton;
  vtkKWCheckButtonWithLabel *OutlineButton;
  vtkKWChangeColorButton *OutlineColorButton;
  vtkKWMenuButtonWithLabel *ColorMenuButton;
  vtkKWMenuButtonWithLabel *DisplayModeButton;
  vtkKWMultiColumnListWithScrollbars *ElementSetList;
  vtkKWLabel *ObjectNameLabel;
  
  int SelectionRow;
  bool inializedElementSetList;
  char ItemName[256];
  char ItemId[64];
  //int DataType;
private:
  vtkKWMimxViewPropertiesGroup(const vtkKWMimxViewPropertiesGroup&); // Not implemented
  void operator=(const vtkKWMimxViewPropertiesGroup&); // Not implemented
 };

#endif

