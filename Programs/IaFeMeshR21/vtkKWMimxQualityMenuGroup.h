/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxQualityMenuGroup.h,v $
Language:  C++
Date:      $Date: 2008/07/29 02:14:49 $
Version:   $Revision: 1.5 $

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
// .NAME vtkKWMimxQualityMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxQualityMenuGroup_h
#define __vtkKWMimxQualityMenuGroup_h

#include "vtkKWMimxMainMenuGroup.h"

class vtkKWFileBrowserDialog;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxImageViewProperties;
class vtkKWMimxEvaluateMeshQualityGroup;
class vtkKWMimxEditFEMeshLaplacianSmoothGroup;
class vtkKWMimxExportAbaqusFEMeshGroup;
class vtkKWMimxSaveVTKFEMeshGroup;


class vtkKWMimxQualityMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxQualityMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxQualityMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void EvaluateMeshVolumeCallback();
  void EvaluateMeshEdgeCollapseCallback();
  void EvaluateMeshJacobianCallback();
  void EvaluateMeshSkewCallback();
  void EvaluateMeshAngleCallback();
  void EvaluateMeshCallback();
  void CreateMeshQualityGroup();
  void MeshImprovementCallback();
  void SmoothLaplacianFEMeshCallback();
  void SaveVTKFEMeshCallback();
  void ExportAbaqusFEMeshCallback();
  
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkSetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  
  void UpdateObjectLists();
  
protected:
	vtkKWMimxQualityMenuGroup();
	~vtkKWMimxQualityMenuGroup();
	vtkKWFileBrowserDialog *FileBrowserDialog;
	vtkKWMimxSaveVTKFEMeshGroup *SaveVTKFEMeshGroup;
  vtkKWMimxExportAbaqusFEMeshGroup *ExportAbaqusFEMeshGroup;
	virtual void CreateWidget();
	
private:
  vtkKWMimxQualityMenuGroup(const vtkKWMimxQualityMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxQualityMenuGroup&); // Not implemented

  vtkKWMimxEvaluateMeshQualityGroup *EvaluateMeshInterface;
  vtkKWMimxEditFEMeshLaplacianSmoothGroup *FEMeshLaplacianSmoothGroup;

};

#endif
