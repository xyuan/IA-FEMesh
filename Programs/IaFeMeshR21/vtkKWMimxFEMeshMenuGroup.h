/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxFEMeshMenuGroup.h,v $
Language:  C++
Date:      $Date: 2008/04/25 21:31:09 $
Version:   $Revision: 1.24 $

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

#ifndef __vtkKWMimxFEMeshMenuGroup_h
#define __vtkKWMimxFEMeshMenuGroup_h

#include "vtkKWMimxMainMenuGroup.h"

#include "vtkKWMimxMainWindow.h"

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
class vtkKWMimxEditElementSetNumbersGroup;
class vtkKWMimxEditNodeSetNumbersGroup;

class vtkKWMimxMergeMeshGroup;
class vtkKWMimxExportAbaqusFEMeshGroup;
class vtkKWMimxMergeBBGroup;
class vtkKWMimxCreateElementSetPickFaceGroup;
class vtkMimxUnstructuredGridActor;
class vtkKWMimxEditBBMeshSeedGroup;

class vtkUnstructuredGrid;
class vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup;
class vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup;
class vtkKWMimxCreateFEMeshNodeSetGroup;
class vtkKWMimxCreateFEMeshElementSetGroup;

class vtkKWMimxFEMeshMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxFEMeshMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxFEMeshMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void LoadFEMeshCallback();
  void CreateFEMeshCallback();
  void DeleteFEMeshCallback();
  void LoadVTKFEMeshCallback();
  void EditFEMeshCallback();
  void CreateFEMeshFromBBCallback();
  void SaveFEMeshCallback();
  void SaveVTKFEMeshCallback();
  
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(ImageList, vtkLinkedListWrapper);
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
 
  void HideAllDialogBoxes();
  void SmoothLaplacianFEMeshCallback();
  void CreateFEMeshFromSurfaceExtrusionCallback();
  void EditElementNumbersCallback();
  void EditNodeNumbersCallback();
  void CreateFEMeshFromMergingCallback();
  void ExportFEMeshCallback();
  void ExportAbaqusFEMeshCallback();
  void EditImageBasedMatPropsCallback();
  void EditCreateElementSetPickFaceCallback();
  void CreateFEMeshFromSurfaceExtractionCallback();
  void CreateFEMeshNodeSetCallback();
  void CreateFEMeshElementSetCallback();
  void EditBBMeshSeedCallback();
  
protected:
	vtkKWMimxFEMeshMenuGroup();
	~vtkKWMimxFEMeshMenuGroup();

  vtkKWMimxCreateFEMeshFromBBGroup *FEMeshFromBB;
  vtkKWMimxSaveVTKFEMeshGroup *SaveVTKFEMeshGroup;
	virtual void CreateWidget();
  vtkKWLoadSaveDialog *FileBrowserDialog;
  vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
  vtkKWMimxEditFEMeshLaplacianSmoothGroup *FEMeshLaplacianSmoothGroup;
  vtkKWMimxCreateExtrudeFEMeshGroup *CreateExtrudeFEMeshGroup;
  vtkKWMimxEditElementSetNumbersGroup *EditElementSetNumbersGroup;
  vtkKWMimxEditNodeSetNumbersGroup *EditNodeSetNumbersGroup;
  vtkKWMimxMergeMeshGroup *MergeMeshGroup;
  vtkKWMimxExportAbaqusFEMeshGroup *ExportAbaqusFEMeshGroup;
  vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup *ImageMatPropGroup;
  vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup *FEMeshFromSurfaceExtractionGroup;
  vtkKWMimxCreateElementSetPickFaceGroup *CreateElementSetPickFaceGroup;
  vtkKWMimxCreateFEMeshNodeSetGroup *CreateFEMeshNodeSetGroup;
  vtkKWMimxCreateFEMeshElementSetGroup *CreateFEMeshElementSetGroup;
  vtkKWMimxEditBBMeshSeedGroup *EditBBMeshSeedGroup;
  
  void UpdateObjectLists();
  int CheckCellTypesInUgrid(vtkUnstructuredGrid *);


//vtkLinkedListWrapper *ImageList;
private:
  vtkKWMimxFEMeshMenuGroup(const vtkKWMimxFEMeshMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxFEMeshMenuGroup&); // Not implemented
 };

#endif
