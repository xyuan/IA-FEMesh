/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMainMenuGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/07/28 15:06:07 $
Version:   $Revision: 1.20 $

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

#include "vtkKWMimxMainMenuGroup.h"
#include "vtkKWMimxViewProperties.h"

#include "vtkKWApplication.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithScrollbar.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWIcon.h"
#include "vtkKWInternationalization.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWNotebook.h"
#include "vtkKWOptions.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkLinkedListWrapper.h"
#include "vtkKWMimxMainWindow.h"

#include <itksys/SystemTools.hxx>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>


#include <string.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxMainMenuGroup);

vtkCxxRevisionMacro(vtkKWMimxMainMenuGroup, "$Revision: 1.20 $");

//----------------------------------------------------------------------------

vtkKWMimxMainMenuGroup::vtkKWMimxMainMenuGroup()
{
	MimxMainWindow = NULL;
//  this->ObjectList = NULL;
  this->SurfaceList = NULL;
  this->BBoxList = NULL;
  this->FEMeshList = NULL;
  this->ImageList = NULL;
  this->MainFrame = NULL;
//  this->MimxViewProperties = NULL;
  this->ErrorState = 0;
  //this->MessageDialog = vtkKWMessageDialog::New();
  this->DoUndoTree = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxMainMenuGroup::~vtkKWMimxMainMenuGroup()
{
  if(this->MainFrame)
    this->MainFrame->Delete();
  //this->MessageDialog->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxMainMenuGroup::CreateWidget()
{
	if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}
	this->Superclass::CreateWidget();
}
//----------------------------------------------------------------------------
void vtkKWMimxMainMenuGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxMainMenuGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
	OperationMenuButton->SetEnabled(GetEnabled());
	TypeMenuButton->SetEnabled(GetEnabled());
}
//----------------------------------------------------------------------------
void vtkKWMimxMainMenuGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxMainMenuGroup::SetMenuButtonsEnabled(int State)
{
	//this->ObjectMenuButton->SetEnabled(State);
	this->OperationMenuButton->SetEnabled(State);
	this->TypeMenuButton->SetEnabled(State);
}
//----------------------------------------------------------------------------
const char* vtkKWMimxMainMenuGroup::ExtractFileName(const char* FName)
{
	std::string fileName = vtksys::SystemTools::GetFilenameWithoutExtension(FName);
  
  char *fileNameCopy = new char[fileName.length()+1];
  strcpy( fileNameCopy, fileName.c_str() );
  
	return fileNameCopy;
}
////------------------------------------------------------------------------------
//void vtkKWMimxMainMenuGroup::ErrorMessage(vtkObject* vtkNotUsed(object), 
//												 unsigned long event,
//												 void* vtkNotUsed(clientdata), 
//												 void* calldata)
//{
//	const char* message = reinterpret_cast<const char*>( calldata );
//	vtkKWMimxMainMenuGroup::ErrorMessage(message);
//}
////-------------------------------------------------------------------------------
//void vtkKWMimxMainMenuGroup::ErrorMessage(const char *Message)
//{
////	vtkKWMessageDialog *Dialog = vtkKWMessageDialog::New();
//	MessageDialog->SetStyleToOkCancel();
//	MessageDialog->Create();
//	//msg_dlg1->SetPosition(10, 10);
//	//msg_dlg1->SetSize(300, 300);
//	MessageDialog->SetTitle("Your attention please!");
//	MessageDialog->SetText(Message);
//	MessageDialog->Invoke();
//	MessageDialog->Delete();
////	if(Dialog->GetStatus() == vtkKWDialog::StatusOK)
////	ErrorState = 1;
//}
