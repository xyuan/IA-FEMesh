/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSurfaceMenuGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/07/30 02:19:22 $
Version:   $Revision: 1.38 $

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

#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkKWMimxApplication.h"
#include "vtkKWMimxSaveSTLSurfaceGroup.h"
#include "vtkKWMimxSaveVTKSurfaceGroup.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"

#include "vtkMimxActorBase.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkKWMimxViewProperties.h"
#include "vtkKWMimxDeleteObjectGroup.h"
#include "vtkKWMimxCreateSurfaceFromContourGroup.h"

#include "vtkActor.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkSTLReader.h"
#include "vtkExecutive.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkKWMessageDialog.h"
#include "vtkMimxErrorCallback.h"

#include "vtkKWApplication.h"
#include "vtkKWCheckButton.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrameWithScrollbar.h"
#include "vtkKWIcon.h"
#include "vtkKWInternationalization.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWNotebook.h"
#include "vtkKWOptions.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkKWComboBoxWithLabel.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSurfaceMenuGroup);
vtkCxxRevisionMacro(vtkKWMimxSurfaceMenuGroup, "$Revision: 1.38 $");

//----------------------------------------------------------------------------
vtkKWMimxSurfaceMenuGroup::vtkKWMimxSurfaceMenuGroup()
{
	this->ObjectMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton = NULL;
	this->TypeMenuButton = NULL;
	this->SurfaceList = vtkLinkedListWrapper::New();
//  this->MimxViewProperties = NULL;
  this->FileBrowserDialog = NULL;
  this->SaveSTLGroup = NULL;
  this->SaveVTKGroup = NULL;
  this->DeleteObjectGroup = NULL;
  this->CreateSurfaceFromContourGroup = NULL;
}
//----------------------------------------------------------------------------
vtkKWMimxSurfaceMenuGroup::~vtkKWMimxSurfaceMenuGroup()
{
	if (this->SurfaceList) {
		for (int i=0; i < this->SurfaceList->GetNumberOfItems(); i++) {
			vtkMimxSurfacePolyDataActor::SafeDownCast(
        this->SurfaceList->GetItem(i))->Delete();
		}
		this->SurfaceList->Delete();
	}
	this->ObjectMenuButton->Delete();
  if(this->OperationMenuButton)
    this->OperationMenuButton->Delete();
  if(this->TypeMenuButton)
    this->TypeMenuButton->Delete();
 /* if(this->MimxViewProperties)
    this->MimxViewProperties->Delete();*/
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
  if(this->SaveSTLGroup)
	this->SaveSTLGroup->Delete();
  if(this->SaveVTKGroup)
	  this->SaveVTKGroup->Delete();
  if(this->DeleteObjectGroup)
	  this->DeleteObjectGroup->Delete();
  if(this->CreateSurfaceFromContourGroup)
	  this->CreateSurfaceFromContourGroup->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::CreateWidget()
{
	if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}
	this->Superclass::CreateWidget();
	// add menu button with options for various Object
	// for surface
  if(!this->MainFrame)
    this->MainFrame = vtkKWFrame::New();

  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Surface Operations");

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->MainFrame->GetWidgetName());

	/*
	this->ObjectMenuButton->SetParent(this->MainFrame->GetFrame());
	this->ObjectMenuButton->Create();
	this->ObjectMenuButton->SetBorderWidth(0);
	this->ObjectMenuButton->SetReliefToGroove();
	this->ObjectMenuButton->SetLabelText("Object :");
	this->ObjectMenuButton->SetPadX(2);
	this->ObjectMenuButton->SetPadY(2);
	this->ObjectMenuButton->SetWidth(30);
	this->ObjectMenuButton->SetLabelWidth(10);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
		this->ObjectMenuButton->GetWidgetName());
	this->ObjectMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Surface",this, "SurfaceMenuCallback");*/
  //this->ObjectMenuButton->GetWidget()->SetValue("Surface");
	// declare operations menu 
	if(!this->OperationMenuButton)	
		this->OperationMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton->SetParent(this->MainFrame);
	this->OperationMenuButton->Create();
	this->OperationMenuButton->SetBorderWidth(0);
	this->OperationMenuButton->SetReliefToGroove();
	//this->OperationMenuButton->SetLabelText("Operation :");
	this->OperationMenuButton->SetPadX(2);
	this->OperationMenuButton->SetPadY(2);
	//this->OperationMenuButton->SetWidth(30);
	this->OperationMenuButton->GetWidget()->SetWidth(40);
	//this->OperationMenuButton->SetLabelWidth(10);
	this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 15", 
		this->OperationMenuButton->GetWidgetName());
	this->OperationMenuButton->SetEnabled(1);
	//this->OperationMenuButton->GetWidget()->IndicatorVisibilityOff();
	
	// The next operation needs to be moved to a different location
	//this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	//	"Create",this, "CreateSurfaceCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Load",this, "LoadSurfaceCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Delete",this, "DeleteSurfaceCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Save",this, "SaveSurfaceCallback");
	
	// declare operations menu

	if(!this->TypeMenuButton)	
		this->TypeMenuButton = vtkKWMenuButtonWithLabel::New();
	this->TypeMenuButton->SetParent(this->MainFrame);
	this->TypeMenuButton->Create();
/*
	this->TypeMenuButton->SetBorderWidth(0);
	this->TypeMenuButton->SetReliefToGroove();
	this->TypeMenuButton->SetLabelText("Type :");
	this->TypeMenuButton->SetPadX(2);
	this->TypeMenuButton->SetPadY(2);
	this->TypeMenuButton->SetWidth(30);
	this->TypeMenuButton->SetLabelWidth(10);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
		this->TypeMenuButton->GetWidgetName());
	this->TypeMenuButton->SetEnabled(0);
*/
	//this->MessageDialog->SetApplication(this->GetApplication());
	
	this->OperationMenuButton->GetWidget()->SetValue( "Load" );	
	//this->LoadSurfaceCallback();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::LoadSurfaceCallback()
{
	if(!this->FileBrowserDialog)
	{
		this->FileBrowserDialog = vtkKWFileBrowserDialog::New();
		this->FileBrowserDialog->SetApplication(this->GetApplication());
		this->FileBrowserDialog->Create();
	}
	this->FileBrowserDialog->SetFileTypes("{{STL files} {.stl}} {{VTK files} {.vtk}}");
	this->FileBrowserDialog->SetDefaultExtension(".stl");
	this->FileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	this->FileBrowserDialog->Invoke();
	if(this->FileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
		callback->SetState(0);

		if(!this->FileBrowserDialog->GetFileName())
		{
			callback->ErrorMessage("File name not chosen");
			return;
		}

		std::string surfaceFileName = vtksys::SystemTools::CollapseFullPath( this->FileBrowserDialog->GetFileName() );
		std::string extension = vtksys::SystemTools::LowerCase( 
		                        vtksys::SystemTools::GetFilenameLastExtension( surfaceFileName ) );
		this->GetApplication()->SetRegistryValue(
			1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( surfaceFileName ).c_str());
		this->FileBrowserDialog->SaveLastPathToRegistry("LastPath");
		
		vtkSTLReader *stlReader = vtkSTLReader::New();
		vtkPolyDataReader *vtkReader = vtkPolyDataReader::New();
		
		if (extension == ".stl")
		{
    	stlReader->SetFileName( surfaceFileName.c_str() );
    	stlReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
    	stlReader->Update();
    }
    else
    {
      vtkReader->SetFileName( surfaceFileName.c_str() );
  		vtkReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  		vtkReader->Update();
    }
    
		if(!callback->GetState())
		{
			this->SurfaceList->AppendItem(vtkMimxSurfacePolyDataActor::New());
			int item = this->SurfaceList->GetNumberOfItems()-1;
			this->SurfaceList->GetItem(item)->SetDataType(ACTOR_POLYDATA_SURFACE);
			this->SurfaceList->GetItem(item)->SetFilePath( surfaceFileName.c_str() );
			this->SurfaceList->GetItem(item)->SetFoundationName( this->ExtractFileName(surfaceFileName.c_str() ) );
			vtkMimxSurfacePolyDataActor *actor = vtkMimxSurfacePolyDataActor::SafeDownCast(
			  this->SurfaceList->GetItem(item));
			if (extension == ".stl")
			{
  			actor->GetDataSet()->DeepCopy( stlReader->GetOutput() );
			}
			else
			{
  			actor->GetDataSet()->DeepCopy( vtkReader->GetOutput() );
			}
			actor->GetDataSet()->Modified();
			this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
				this->SurfaceList->GetItem(item)->GetActor());
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
			this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->SurfaceList->GetItem(item) );
			this->UpdateObjectLists();
			
			vtkKWMimxApplication *app
          = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
      std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( surfaceFileName );
      app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
      //std::cout << "Surface : " << surfaceFileName << std::endl;
      //std::cout << "Working Path Dir : " << surfaceDirectory << std::endl;
			this->GetMimxMainWindow()->SetStatusText("Loaded Surface");
		}
		else
		{
  		callback->ErrorMessage("Failed to load the requested file.");
		}
		stlReader->Delete();
		vtkReader->Delete();
	}
	
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::SaveSurfaceCallback()
{

  if (!this->SaveSTLGroup)
	{
		this->SaveSTLGroup = vtkKWMimxSaveSTLSurfaceGroup::New();
		this->SaveSTLGroup->SetParent( this->GetParent() );
		this->SaveSTLGroup->SetSurfaceList(this->SurfaceList);
		this->SaveSTLGroup->SetMenuGroup(this);
		this->SaveSTLGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->SaveSTLGroup->Create();
	}
	else
	{
		this->SaveSTLGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->SaveSTLGroup->GetMainFrame()->GetWidgetName());
		
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::CreateSurfaceCallback()
{
  this->TypeMenuButton->SetLabelText("Type :");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"From Picked Points",this, "CreateSurfaceFromContourCallback");
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->SetEnabled(1);
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::DeleteSurfaceCallback()
{
	if(!this->DeleteObjectGroup)
	{
		this->DeleteObjectGroup = vtkKWMimxDeleteObjectGroup::New();
		this->DeleteObjectGroup->SetParent( this->GetParent() );
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(this->SurfaceList);
		this->DeleteObjectGroup->SetMenuGroup(this);
		this->DeleteObjectGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->DeleteObjectGroup->Create();
		this->TypeMenuButton->GetWidget()->SetValue("");
		this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
		this->TypeMenuButton->SetEnabled(0);
	}
	else{
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(this->SurfaceList);
		this->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->DeleteObjectGroup->GetObjectListComboBox()->SetLabelText("Surface: ");
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->DeleteObjectGroup->GetMainFrame()->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::CreateSurfaceFromContourCallback()
{
	if (!this->CreateSurfaceFromContourGroup)
	{
		this->CreateSurfaceFromContourGroup = vtkKWMimxCreateSurfaceFromContourGroup::New();
		this->CreateSurfaceFromContourGroup->SetApplication(this->GetApplication());
		this->CreateSurfaceFromContourGroup->SetParent( this->GetParent() );
		this->CreateSurfaceFromContourGroup->SetFEMeshList(this->FEMeshList);
		this->CreateSurfaceFromContourGroup->SetSurfaceList(this->SurfaceList);
		this->CreateSurfaceFromContourGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateSurfaceFromContourGroup->SetMenuGroup(this);
		this->CreateSurfaceFromContourGroup->Create();
	}
	else
	{
		this->CreateSurfaceFromContourGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->CreateSurfaceFromContourGroup->GetMainFrame()->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::UpdateObjectLists()
{
	if(this->SaveSTLGroup)
		this->SaveSTLGroup->UpdateObjectLists();
	if(this->SaveVTKGroup)
		this->SaveVTKGroup->UpdateObjectLists();
	if(this->DeleteObjectGroup)
		this->DeleteObjectGroup->UpdateObjectLists();
	if(this->CreateSurfaceFromContourGroup)
		this->CreateSurfaceFromContourGroup->UpdateObjectLists();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//---------------------------------------------------------------------------
