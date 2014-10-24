/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxBBMenuGroup.h,v $
Language:  C++
Date:      $Date: 2008/05/22 23:25:23 $
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
// .NAME vtkKWMimxFEMeshMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxBBMenuGroup_h
#define __vtkKWMimxBBMenuGroup_h

#include "vtkKWMimxMainMenuGroup.h"

#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

class vtkKWMimxCreateBBFromBoundsGroup;
class vtkKWMimxCreateBBMeshSeedGroup;
class vtkKWMimxEditBBGroup;
class vtkKWMimxCreateFEMeshFromBBGroup;
class vtkKWMimxViewProperties;
class vtkKWMimxSaveVTKBBGroup;
class vtkKWMimxEditBBMeshSeedGroup;
class vtkKWLoadSaveDialog;
class vtkKWMimxSaveVTKFEMeshGroup;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxEditFEMeshLaplacianSmoothGroup;
class vtkKWMimxCreateExtrudeFEMeshGroup;
class vtkMimxUnstructuredGridActor;
class vtkKWMimxNodeElementNumbersGroup;
class vtkKWMimxMergeMeshGroup;
class vtkKWMimxExportAbaqusFEMeshGroup;
class vtkKWMimxMergeBBGroup;
class vtkKWMimxCreateElementSetPickFaceGroup;
class vtkMimxUnstructuredGridActor;

class vtkUnstructuredGrid;
class vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup;
class vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup;
class vtkKWMimxSurfaceMenuGroup;
class vtkKWMimxCreateBBFromRubberBandPickGroup;

class vtkKWMimxBBMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxBBMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxBBMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  //void BBMenuCallback();
  void LoadBBCallback();
  void CreateBBCallback();
  void DeleteBBCallback();
  void LoadVTKBBCallback();
  void CreateBBFromBoundsCallback();
  void CreateBBMeshSeedCallback();
  void EditBBMeshSeedCallback();
  void EditBBCallback();
  void SaveBBCallback();
  void SaveVTKBBCallback();
  void CreateBBFromMergingCallback();
  void CreateBBFromRubberBandPickCallback();
  
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);
  vtkSetObjectMacro(SurfaceMenuGroup, vtkKWMimxSurfaceMenuGroup);
  vtkGetObjectMacro(SurfaceMenuGroup, vtkKWMimxSurfaceMenuGroup);
  void HideAllDialogBoxes();
  
protected:
	vtkKWMimxBBMenuGroup();
	~vtkKWMimxBBMenuGroup();

  vtkKWMimxCreateBBFromBoundsGroup *CreateBBFromBounds;
  vtkKWMimxCreateBBMeshSeedGroup *CreateBBMeshSeed;
  vtkKWMimxEditBBGroup *EditBB;
  vtkKWMimxSaveVTKBBGroup *SaveVTKBBGroup;
  vtkKWMimxEditBBMeshSeedGroup *EditBBMeshSeedGroup;
  vtkKWLoadSaveDialog *FileBrowserDialog;
  vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
  vtkKWMimxMergeBBGroup *MergeBBGroup;
  vtkKWMimxCreateBBFromRubberBandPickGroup *CreateBBFromRubberBandPickGroup;
  virtual void CreateWidget();

void UpdateObjectLists();
int CheckCellTypesInUgrid(vtkUnstructuredGrid *);


vtkKWMimxSurfaceMenuGroup *SurfaceMenuGroup;

private:
  vtkKWMimxBBMenuGroup(const vtkKWMimxBBMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxBBMenuGroup&); // Not implemented
 };

#endif
