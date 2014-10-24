/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSurfaceMenuGroup.h,v $
Language:  C++
Date:      $Date: 2008/05/27 02:33:15 $
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
// .NAME vtkKWMimxSurfaceMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxSurfaceMenuGroup_h
#define __vtkKWMimxSurfaceMenuGroup_h

#include "vtkKWMimxMainMenuGroup.h"

class vtkKWFileBrowserDialog;
class vtkKWMimxSaveSTLSurfaceGroup;
class vtkKWMimxSaveVTKSurfaceGroup;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxCreateSurfaceFromContourGroup;

class vtkKWMimxSurfaceMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxSurfaceMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSurfaceMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void LoadSurfaceCallback();
  void SaveSurfaceCallback();
  void DeleteSurfaceCallback();
  void CreateSurfaceCallback();
  void CreateSurfaceFromContourCallback();
  vtkSetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  void UpdateObjectLists();
protected:
	vtkKWMimxSurfaceMenuGroup();
	~vtkKWMimxSurfaceMenuGroup();
	vtkKWFileBrowserDialog *FileBrowserDialog;
	virtual void CreateWidget();
	vtkKWMimxSaveSTLSurfaceGroup *SaveSTLGroup;
	vtkKWMimxSaveVTKSurfaceGroup *SaveVTKGroup;
	vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
	vtkKWMimxCreateSurfaceFromContourGroup *CreateSurfaceFromContourGroup;

private:
  vtkKWMimxSurfaceMenuGroup(const vtkKWMimxSurfaceMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxSurfaceMenuGroup&); // Not implemented
 };

#endif
