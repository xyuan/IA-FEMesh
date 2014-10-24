/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageMenuGroup.h,v $
Language:  C++
Date:      $Date: 2008/07/11 03:12:06 $
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
// .NAME vtkKWMimxImageMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxImageMenuGroup_h
#define __vtkKWMimxImageMenuGroup_h

#include "vtkKWMimxMainMenuGroup.h"

class vtkKWFileBrowserDialog;
class vtkKWMimxDeleteObjectGroup;
class vtkKWMimxImageViewProperties;
class vtkFocalPlaneContourRepresentation;
class vtkContourWidget;
class vtkImageActor;

class vtkKWMimxImageMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxImageMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxImageMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  //void ImageMenuCallback();
//  void ImageMaskMenuCallback();
  void LoadImageCallback();
  void ImportImageCallback();
  void TraceImageCallback();
  void DeleteImageCallback();
  //void LoadImageMaskCallback();
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
//  void UpdateObjectLists();
protected:
	vtkKWMimxImageMenuGroup();
	~vtkKWMimxImageMenuGroup();
	vtkKWFileBrowserDialog *FileBrowserDialog;
	virtual void CreateWidget();
	vtkKWMimxImageViewProperties *MimxImageViewProperties;
  vtkFocalPlaneContourRepresentation *ContourRepresentation;
  vtkContourWidget *ContourWidget;
  vtkImageActor *ImageTraceActor;
  vtkKWMimxDeleteObjectGroup *DeleteObjectGroup;
  
private:
  vtkKWMimxImageMenuGroup(const vtkKWMimxImageMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxImageMenuGroup&); // Not implemented
 };

#endif
