/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMeshViewerApplication.h,v $
Language:  C++
Date:      $Date: 2008/02/05 21:38:17 $
Version:   $Revision: 1.1 $

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

#ifndef __vtkKWMeshViewerApplication_h
#define __vtkKWMeshViewerApplication_h

#include "vtkKWApplication.h"
#include "vtkKWRegistryHelper.h"


class vtkSlicerTheme;
class vtkKWTopLevel;

class vtkKWMeshViewerApplication : public vtkKWApplication
{
public:
  static vtkKWMeshViewerApplication* New();
  vtkTypeRevisionMacro(vtkKWMeshViewerApplication,vtkKWApplication);
 
  virtual void AddAboutCopyrights(ostream &);
  virtual void InstallTheme ( vtkKWTheme *theme );
  virtual void InstallDefaultTheme ( );
  virtual void DisplayAboutDialog(vtkKWTopLevel* master);
  
  // Description:
  // Set/Get the application font family
  void SetApplicationFontFamily ( const char *family);
  const char *GetApplicationFontFamily ( ) const;

  // Description:
  // Set/Get the application font size
  void SetApplicationFontSize ( const char *size );
  const char *GetApplicationFontSize ( ) const;
  
protected:
  vtkKWMeshViewerApplication();
  ~vtkKWMeshViewerApplication();
  
  vtkSlicerTheme *SlicerTheme;

  char ApplicationFontSize [vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char ApplicationFontFamily [vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  
private:
  vtkKWMeshViewerApplication(const vtkKWMeshViewerApplication&);   // Not implemented.
  void operator=(const vtkKWMeshViewerApplication&);  // Not implemented.
  
  
};

#endif
