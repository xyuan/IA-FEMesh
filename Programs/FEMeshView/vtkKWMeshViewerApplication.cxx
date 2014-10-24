/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMeshViewerApplication.cxx,v $
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

#include "vtkKWMeshViewerApplication.h"

#include "vtkObjectFactory.h"
#include "vtkKWWindowBase.h"
#include "vtkSlicerTheme.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWRegistryHelper.h"
#include "vtkKWTopLevel.h"
#include "vtkKWMessageDialog.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMeshViewerApplication );
vtkCxxRevisionMacro(vtkKWMeshViewerApplication, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkKWMeshViewerApplication::vtkKWMeshViewerApplication()
{
  
  vtkKWFrameWithLabel::SetDefaultLabelFontWeightToNormal( );
  this->SlicerTheme = vtkSlicerTheme::New ( );
  
  strcpy (this->ApplicationFontSize, "small" );
  strcpy ( this->ApplicationFontFamily, "Arial" );
    
}

vtkKWMeshViewerApplication::~vtkKWMeshViewerApplication ( ) 
{
  if ( this->SlicerTheme )
    {
    this->SlicerTheme->Delete ( );
    this->SlicerTheme = NULL;
    }
    
}


void vtkKWMeshViewerApplication::InstallDefaultTheme ( )
{
  InstallTheme( this->SlicerTheme );
}

//---------------------------------------------------------------------------
void vtkKWMeshViewerApplication::InstallTheme ( vtkKWTheme *theme )
{
  if ( theme != NULL ) 
  {
    if ( vtkSlicerTheme::SafeDownCast (theme) == this->SlicerTheme ) {
        this->SetTheme (this->SlicerTheme );
    } else {
        this->SetTheme ( theme );
    }
  }
}

//----------------------------------------------------------------------------
void vtkKWMeshViewerApplication::SetApplicationFontFamily ( const char *family)
{
    if (family)
    {
        if (strcmp(this->ApplicationFontFamily, family) != 0
        && strlen(family) < vtkKWRegistryHelper::RegistryKeyValueSizeMax)
        {
            strcpy(this->ApplicationFontFamily, family);
            this->Modified();
        }
    }
}
//----------------------------------------------------------------------------
const char *vtkKWMeshViewerApplication::GetApplicationFontFamily () const
{
  return this->ApplicationFontFamily;
}


//----------------------------------------------------------------------------
void vtkKWMeshViewerApplication::SetApplicationFontSize ( const char *size)
{
    if (size )
    {
        if (strcmp(this->ApplicationFontSize, size) != 0
        && strlen(size) < vtkKWRegistryHelper::RegistryKeyValueSizeMax)
        {
            strcpy(this->ApplicationFontSize, size);
            this->Modified();
        }
    }
}
//----------------------------------------------------------------------------
const char *vtkKWMeshViewerApplication::GetApplicationFontSize () const
{
  return this->ApplicationFontSize;
}


//----------------------------------------------------------------------------
void vtkKWMeshViewerApplication::AddAboutCopyrights(ostream &os)
{
  os << "IA-FEMesh is developed by the ";
  os << "Musculoskeletal Imaging, Modelling and Experimentation (MIMX) Program" << std::endl;
  os << "Center for Computer Aided Design" << std::endl;
  os << "The University of Iowa" << std::endl;
  os << "Iowa City, IA 52242" << std::endl;

  os << "See http://www.ccad.uiowa.edu/mimx/ for Copyright Information" << std::endl << std::endl;
  os << "IA-FEMesh is built upon:" << std::endl;
  os << "  Slicer3 http://www.na-mic.org/" << std::endl;
  os << "  VTK http://www.vtk.org/copyright.php" << std::endl;
  os << "  ITK http://www.itk.org/HTML/Copyright.htm" << std::endl;
  os << "  KWWidgets http://www.kitware.com/Copyright.htm" << std::endl;
  os << "  Tcl/Tk http://www.tcl.tk" << std::endl << std::endl;
  os << "Supported is provided by the National Institutes of Health R21." << std::endl;

#if 0
  // example of the extra detail needed:
  //
     << tcl_major << "." << tcl_minor << "." << tcl_patch_level << endl
     << "  - Copyright (c) 1989-1994 The Regents of the University of "
     << "California." << endl
     << "  - Copyright (c) 1994 The Australian National University." << endl
     << "  - Copyright (c) 1994-1998 Sun Microsystems, Inc." << endl
     << "  - Copyright (c) 1998-2000 Ajuba Solutions." << endl;
#endif
}

void vtkKWMeshViewerApplication::DisplayAboutDialog(vtkKWTopLevel* master)
{
  if (this->InExit)
    {
    return;
    }

  if (!this->AboutDialog)
    {
    this->AboutDialog = vtkKWMessageDialog::New();
    }

  if (!this->AboutDialog->IsCreated())
    {
    this->AboutDialog->SetApplication(this);
    this->AboutDialog->SetMasterWindow(master);
    this->AboutDialog->HideDecorationOn();
    this->AboutDialog->Create();
    this->AboutDialog->SetBorderWidth(1);
    this->AboutDialog->SetReliefToSolid();
    }

  this->ConfigureAboutDialog();

  this->AboutDialog->Invoke();
}

