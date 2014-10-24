/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewProperties.h,v $
Language:  C++
Date:      $Date: 2008/08/05 02:32:48 $
Version:   $Revision: 1.28 $

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
// .NAME vtkKWMimxViewProperties - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxViewProperties_h
#define __vtkKWMimxViewProperties_h

#include "vtkKWCompositeWidget.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkKWMimxViewPropertiesGroup.h"
#include "vtkKWMimxDisplayPropertiesGroup.h"

class vtkKWFrameWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkLinkedListWrapper;
class vtkKWFrameWithScrollbar;
class vtkLinkedListWrapperTree;
class vtkKWMultiColumnList;
class vtkKWMimxViewPropertiesGroup;
class vtkKWPushButton;


class vtkKWMimxViewProperties : public vtkKWCompositeWidget
{
public:
  static vtkKWMimxViewProperties* New();
  vtkTypeRevisionMacro(vtkKWMimxViewProperties,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(DoUndoTree, vtkLinkedListWrapperTree);
  vtkGetObjectMacro(MainFrame, vtkKWFrameWithLabel);
  vtkGetObjectMacro(MultiColumnList, vtkKWMultiColumnListWithScrollbars);
  vtkSetObjectMacro(ViewPropertiesGroup, vtkKWMimxViewPropertiesGroup);
  vtkGetObjectMacro(ObjectList, vtkLinkedListWrapper);
  void VisibilityCallback(const char *objectId, int );
  void AddObjectList(vtkMimxActorBase*);
  void DeleteObjectList(int DataType, int Position );
  void DeleteObjectList(const char *name);
  void UpdateVisibility();
  void UpdateVisibilityList();
  void ViewPropertyCallback(const char *objectId, const char *name);
  void DisplayPropertyCallback();
  void CreateNameCellCallback(const char *objectId, int actorType, const char *tableWidgetName, int row, int col, const char *widgetName);
  void SortedCommandCallback();
  void EnableViewPropertyList( int mode );
  
  
protected:
	vtkKWMimxViewProperties();
	~vtkKWMimxViewProperties();
	virtual void CreateWidget();
  vtkKWMultiColumnListWithScrollbars *MultiColumnList;
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkLinkedListWrapper *ObjectList;
  vtkKWFrameWithLabel *MainFrame;
  vtkLinkedListWrapperTree *DoUndoTree;
  vtkKWMimxViewPropertiesGroup *ViewPropertiesGroup;
  vtkKWPushButton *ViewButton;
  vtkKWPushButton *DisplayButton;
  vtkKWMimxViewPropertiesGroup *ViewPropertyDialog;
  vtkKWMimxDisplayPropertiesGroup *DisplayPropertyDialog;
  //int DataType;
private:
  vtkKWMimxViewProperties(const vtkKWMimxViewProperties&); // Not implemented
  void operator=(const vtkKWMimxViewProperties&); // Not implemented
  
  int ObjectId;
  
 };

#endif

