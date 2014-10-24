/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxBBMenuGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/07/28 15:06:07 $
Version:   $Revision: 1.32 $

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

#include "vtkKWMimxApplication.h"
#include "vtkKWMimxBBMenuGroup.h"
#include "vtkKWMimxCreateBBFromBoundsGroup.h"
#include "vtkKWMimxCreateBBMeshSeedGroup.h"
#include "vtkKWMimxEditBBGroup.h"
#include "vtkKWMimxCreateFEMeshFromBBGroup.h"
#include "vtkKWMimxViewProperties.h"
#include "vtkKWMimxSaveVTKBBGroup.h"
#include "vtkKWMimxEditBBMeshSeedGroup.h"
#include "vtkKWMimxSaveVTKFEMeshGroup.h"
#include "vtkKWMimxDeleteObjectGroup.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkKWMimxEditFEMeshLaplacianSmoothGroup.h"
#include "vtkKWMimxCreateExtrudeFEMeshGroup.h"
#include "vtkKWMimxNodeElementNumbersGroup.h"
#include "vtkKWMimxMergeMeshGroup.h"
#include "vtkKWMimxExportAbaqusFEMeshGroup.h"
#include "vtkKWMimxMergeBBGroup.h"
#include "vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.h"
#include "vtkKWMimxCreateBBFromRubberBandPickGroup.h"
#include "vtkMimxErrorCallback.h"
#include "vtkCellTypes.h"

#include "vtkMimxUnstructuredGridWidget.h"

#include "vtkMimxActorBase.h"
#include "vtkMimxUnstructuredGridActor.h"

#include "vtkActor.h"
#include "vtkDataSet.h"
#include "vtkDataSetMapper.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"

#include "vtkKWApplication.h"
#include "vtkKWCheckButton.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
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
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup.h"
#include "vtkKWMimxCreateElementSetPickFaceGroup.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkKWComboBoxWithLabel.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxBBMenuGroup);
vtkCxxRevisionMacro(vtkKWMimxBBMenuGroup, "$Revision: 1.32 $");

//----------------------------------------------------------------------------
vtkKWMimxBBMenuGroup::vtkKWMimxBBMenuGroup()
{
	this->ObjectMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton = NULL;
	this->TypeMenuButton = NULL;
  this->BBoxList = vtkLinkedListWrapper::New();
  this->CreateBBFromBounds = NULL;
  this->CreateBBMeshSeed = NULL;
  this->EditBB = NULL;
  this->MainFrame = NULL;
  this->SaveVTKBBGroup = NULL;
  this->EditBBMeshSeedGroup = NULL;
  this->FileBrowserDialog = NULL;
  this->DeleteObjectGroup = NULL;
  this->MergeBBGroup = NULL;
  this->SurfaceMenuGroup = NULL;
  this->CreateBBFromRubberBandPickGroup = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxBBMenuGroup::~vtkKWMimxBBMenuGroup()
{
	if (this->BBoxList) 
	{
    for (int i=0; i < this->BBoxList->GetNumberOfItems(); i++) {
      vtkMimxUnstructuredGridActor::SafeDownCast(
        this->BBoxList->GetItem(i))->Delete();
    }
		this->BBoxList->Delete();
	}

  if (this->OperationMenuButton)
    this->OperationMenuButton->Delete();
  if (this->TypeMenuButton)
    this->TypeMenuButton->Delete();
	if (this->ObjectMenuButton)
	this->ObjectMenuButton->Delete();
  if(this->CreateBBFromBounds)
    this->CreateBBFromBounds->Delete();
  if(this->EditBB)
    this->EditBB->Delete();
  if(this->CreateBBMeshSeed)
    this->CreateBBMeshSeed->Delete();
  if(this->SaveVTKBBGroup)
	  this->SaveVTKBBGroup->Delete();
  if (this->EditBBMeshSeedGroup)
		this->EditBBMeshSeedGroup->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
  if(this->DeleteObjectGroup)
	  this->DeleteObjectGroup->Delete();
  if(this->MergeBBGroup)
	  this->MergeBBGroup->Delete();
  if(this->CreateBBFromRubberBandPickGroup)
	  this->CreateBBFromRubberBandPickGroup->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::CreateWidget()
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
  //this->MainFrame->SetLabelText("Building Block Operations");

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
		"Building Block",this, "BBMenuCallback");*/
	/* Move to Operation 
  this->ObjectMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Building Block Mesh Seed",this, "BBMeshSeedMenuCallback");
  */
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
	//this->OperationMenuButton->SetLabelWidth(10);
	this->OperationMenuButton->GetWidget()->SetWidth(40);
	this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 15", 
		this->OperationMenuButton->GetWidgetName());
//	this->OperationMenuButton->SetEnabled(1);
	
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Create",this, "CreateBBFromBoundsCallback");
	//this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	//	"Manual Block Definition",this, "CreateBBFromRubberBandPickCallback");	
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Load",this, "LoadVTKBBCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Build/Edit",this, "EditBBCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Delete",this, "DeleteBBCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Save",this, "SaveVTKBBCallback");
  //this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
  //  "Assign Mesh Seeds",this, "CreateBBMeshSeedCallback");
  //this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    //"Mesh Seeds",this, "EditBBMeshSeedCallback");
    
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
	this->OperationMenuButton->GetWidget()->SetValue( "Create" );	
	this->CreateBBCallback();
	
}
//----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
/*
void vtkKWMimxBBMenuGroup::BBMenuCallback()
{
//  this->HideAllDialogBoxes();
	this->OperationMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->OperationMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Load",this, "LoadBBCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Save",this, "SaveBBCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Create",this, "CreateBBCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Delete",this, "DeleteBBCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Edit",this, "EditBBCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Assign Mesh Seeds",this, "CreateBBMeshSeedCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Edit Mesh Seeds",this, "EditBBMeshSeedCallback");

	this->OperationMenuButton->SetEnabled(1);
	//if(!this->BBViewProperties)
	//{
	//	this->BBViewProperties = vtkKWMimxViewProperties::New();
	//	this->BBViewProperties->SetDataType(2);
	//	this->BBViewProperties->SetParent(this->GetParent());
	//	this->BBViewProperties->SetObjectList(this->BBoxList);
	//	this->BBViewProperties->SetMimxMainWindow(this->GetMimxMainWindow());
	//	this->BBViewProperties->Create();
	//}

	//if(this->MimxViewProperties)
	//{
	//	this->GetApplication()->Script("pack forget %s", 
	//		this->MimxViewProperties->GetMainFrame()->GetWidgetName());
	//}

	//this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5", 
	//	this->BBViewProperties->GetMainFrame()->GetWidgetName());

}
*/
//----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::LoadBBCallback()
{
//  this->HideAllDialogBoxes();
  this->TypeMenuButton->SetLabelText("Format :");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"VTK",this, "LoadVTKBBCallback");
	this->TypeMenuButton->SetEnabled(1);
}
//----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::CreateBBCallback()
{
  //this->HideAllDialogBoxes();
  this->TypeMenuButton->SetLabelText("Type :");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"From Bounds",this, "CreateBBFromBoundsCallback");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"From Merging",this, "CreateBBFromMergingCallback");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"From RubberBand Pick", this, "CreateBBFromRubberBandPickCallback");

	this->TypeMenuButton->SetEnabled(1);
}
//----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::LoadVTKBBCallback()
{
 // this->HideAllDialogBoxes();
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	callback->SetState(0);
  if(!this->FileBrowserDialog)
  {
	  this->FileBrowserDialog = vtkKWLoadSaveDialog::New();
	this->FileBrowserDialog->SetApplication(this->GetApplication());
	this->FileBrowserDialog->Create();
  }
	this->FileBrowserDialog->SetDefaultExtension(".vtk");
	this->FileBrowserDialog->SetFileTypes("{{VTK files} {.vtk}}");
	this->FileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	this->FileBrowserDialog->SetTitle("Load Building block structure (VTK file format)");
	this->FileBrowserDialog->Invoke();
	if(this->FileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		if(this->FileBrowserDialog->GetFileName())
		{
			std::string bbFileName = vtksys::SystemTools::CollapseFullPath( this->FileBrowserDialog->GetFileName() );
			this->GetApplication()->SetRegistryValue(
				1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( bbFileName ).c_str());
			this->FileBrowserDialog->SaveLastPathToRegistry("LastPath");
			vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
			reader->SetFileName( bbFileName.c_str() );
			reader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
			reader->Update();
			reader->RemoveObserver(callback);
			if(!callback->GetState())
			{
				if(!this->CheckCellTypesInUgrid(reader->GetOutput()))
				{
					callback->ErrorMessage("File read in contains elements other than hexahedra");
					reader->Delete();
					return;
				}
				this->BBoxList->AppendItem(vtkMimxUnstructuredGridActor::New());

				this->DoUndoTree->AppendItem(new Node);
				int item = this->DoUndoTree->GetNumberOfItems()-1;
				int bbIndex = this->BBoxList->GetNumberOfItems()-1;
				this->DoUndoTree->GetItem(item)->Data = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(bbIndex));
				this->DoUndoTree->GetItem(item)->Child = NULL;
				this->DoUndoTree->GetItem(item)->Parent = NULL;
				
				this->BBoxList->GetItem(bbIndex)->SetDataType(ACTOR_BUILDING_BLOCK);
				vtkMimxUnstructuredGridActor *ugridActor = 
				  vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(bbIndex));
				ugridActor->GetDataSet()->DeepCopy(reader->GetOutput());
				ugridActor->GetDataSet()->Modified();
				this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->BBoxList->GetItem(bbIndex)->GetActor());
				this->GetMimxMainWindow()->GetRenderWidget()->Render();
				this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
				this->BBoxList->GetItem(bbIndex)->SetFilePath( bbFileName.c_str() );
				this->BBoxList->GetItem(bbIndex)->SetFoundationName( this->ExtractFileName(bbFileName.c_str()) );
				this->UpdateObjectLists();
				this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(this->BBoxList->GetItem(bbIndex));
				
				vtkKWMimxApplication *app
          = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
        std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( bbFileName );
        //std::cout << "BB : " << bbFileName << std::endl;
        //std::cout << "Working Path Dir : " << surfaceDirectory << std::endl;
        app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
      
				this->GetMimxMainWindow()->SetStatusText("Loaded Building Block");
			}
			reader->Delete();
		}
	}
}
//----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//---------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::CreateBBFromBoundsCallback()
{
   if (!this->CreateBBFromBounds)
  {
       this->CreateBBFromBounds = vtkKWMimxCreateBBFromBoundsGroup::New();
     	this->CreateBBFromBounds->SetApplication(this->GetApplication());
     	CreateBBFromBounds->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
       this->CreateBBFromBounds->SetSurfaceList(this->SurfaceList);
       this->CreateBBFromBounds->SetBBoxList(this->BBoxList);
        this->CreateBBFromBounds->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateBBFromBounds->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->CreateBBFromBounds->SetMenuGroup(this);
		this->CreateBBFromBounds->SetDoUndoTree(this->DoUndoTree);
//		this->CreateBBFromBounds->SetSurfaceMenuGroup(this->GetSurfaceMenuGroup());
     	CreateBBFromBounds->Create();
   }
   else
   {
	   this->CreateBBFromBounds->UpdateObjectLists();
   }
   //this->SurfaceMenuGroup->SetEnabled(0);
     this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
   this->SetMenuButtonsEnabled(0);
      this->GetApplication()->Script(
        "pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
        CreateBBFromBounds->GetMainFrame()->GetWidgetName()); 
 }

//-----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::CreateBBMeshSeedCallback()
{
 
  if (!this->CreateBBMeshSeed)
  {
	  this->CreateBBMeshSeed = vtkKWMimxCreateBBMeshSeedGroup::New();

	  this->CreateBBMeshSeed->SetApplication(this->GetApplication());
	  this->CreateBBMeshSeed->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
	  this->CreateBBMeshSeed->SetBBoxList(this->BBoxList);
	  this->CreateBBMeshSeed->SetMenuGroup(this);
	  this->CreateBBMeshSeed->SetMimxMainWindow(this->GetMimxMainWindow());
	  this->CreateBBMeshSeed->SetViewProperties(
		  this->GetMimxMainWindow()->GetViewProperties());
	  this->CreateBBMeshSeed->Create();
  }
  else
  {
	  this->CreateBBMeshSeed->UpdateObjectLists();
  }
  this->SetMenuButtonsEnabled(0);
    this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
	  CreateBBMeshSeed->GetMainFrame()->GetWidgetName());
}
//-----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::EditBBMeshSeedCallback()
{
	if (!this->EditBBMeshSeedGroup)
	{
		this->EditBBMeshSeedGroup = vtkKWMimxEditBBMeshSeedGroup::New();

		this->EditBBMeshSeedGroup->SetApplication(this->GetApplication());
		this->EditBBMeshSeedGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->EditBBMeshSeedGroup->SetBBoxList(this->BBoxList);
		this->EditBBMeshSeedGroup->SetMenuGroup(this);
		this->EditBBMeshSeedGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->EditBBMeshSeedGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
//		this->SetMenuButtonsEnabled(0);
		this->EditBBMeshSeedGroup->Create();
	}
	else
	{
		this->EditBBMeshSeedGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		EditBBMeshSeedGroup->GetMainFrame()->GetWidgetName());
}
//-----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::EditBBCallback()
{
  //this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
  //this->TypeMenuButton->GetWidget()->SetValue("");
  //this->TypeMenuButton->SetEnabled(0);
  if (!this->EditBB)
  {
	  this->EditBB = vtkKWMimxEditBBGroup::New();

	  this->EditBB->SetApplication(this->GetApplication());
	  this->EditBB->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
	  this->EditBB->SetBBoxList(this->BBoxList);
	  this->EditBB->SetDoUndoTree(this->DoUndoTree);
	  this->EditBB->SetMimxMainWindow(this->GetMimxMainWindow());
	  this->EditBB->Create();
	  this->EditBB->SetViewProperties(
		  this->GetMimxMainWindow()->GetViewProperties());
	  this->EditBB->SetMenuGroup(this);
//	  this->SetMenuButtonsEnabled(0);
  }
  else
  {
	  this->EditBB->UpdateObjectLists();
  }
  this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
    this->EditBB->GetMainFrame()->GetWidgetName());

}
//-----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::HideAllDialogBoxes()
{
  if (this->CreateBBFromBounds)
  {
    this->GetApplication()->Script("pack forget %s", 
      this->CreateBBFromBounds->GetMainFrame()->GetWidgetName());
  }
  if (this->CreateBBMeshSeed)
  {
    this->GetApplication()->Script("pack forget %s", 
      this->CreateBBMeshSeed->GetMainFrame()->GetWidgetName());
  }
  if (this->EditBB)
  {
	  this->GetApplication()->Script("pack forget %s", 
		  this->EditBB->GetMainFrame()->GetWidgetName());
  }
  if (this->EditBBMeshSeedGroup)
  {
    this->GetApplication()->Script("pack forget %s", 
		  this->EditBBMeshSeedGroup->GetMainFrame()->GetWidgetName());
  }
}

//-----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::SaveBBCallback()
{
  this->TypeMenuButton->SetLabelText("Format :");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"VTK",this, "SaveVTKBBCallback");
	this->TypeMenuButton->SetEnabled(1);
}
//-----------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::SaveVTKBBCallback()
{
	if(!this->SaveVTKBBGroup)
	{
		this->SaveVTKBBGroup = vtkKWMimxSaveVTKBBGroup::New();
		this->SaveVTKBBGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->SaveVTKBBGroup->SetBBoxList(this->BBoxList);
		this->SaveVTKBBGroup->SetMenuGroup(this);
		this->SaveVTKBBGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->SaveVTKBBGroup->Create();
//		this->SetMenuButtonsEnabled(0);
	}
	else
	{
		this->SaveVTKBBGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->SaveVTKBBGroup->GetMainFrame()->GetWidgetName());
}

//---------------------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::DeleteBBCallback()
{
	if(!this->DeleteObjectGroup)
	{
		this->DeleteObjectGroup = vtkKWMimxDeleteObjectGroup::New();
		this->DeleteObjectGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(this->BBoxList);
		this->DeleteObjectGroup->SetDoUndoTree(this->DoUndoTree);
		this->DeleteObjectGroup->SetSurfaceList(NULL);		
		this->DeleteObjectGroup->SetMenuGroup(this);
		this->DeleteObjectGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->DeleteObjectGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->DeleteObjectGroup->Create();
		//this->TypeMenuButton->GetWidget()->SetValue("");
		//this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
		//this->TypeMenuButton->SetEnabled(0);
//		this->SetMenuButtonsEnabled(0);
	}
	else
	{
		this->DeleteObjectGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(this->BBoxList);
		this->DeleteObjectGroup->SetDoUndoTree(this->DoUndoTree);
		this->DeleteObjectGroup->SetSurfaceList(NULL);		
		this->DeleteObjectGroup->UpdateObjectLists();
	}
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->SetMenuButtonsEnabled(0);

	this->DeleteObjectGroup->GetObjectListComboBox()->SetLabelText("Building Block: ");
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->DeleteObjectGroup->GetMainFrame()->GetWidgetName());
}

//--------------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::UpdateObjectLists()
{
	if(this->CreateBBFromBounds)
		this->CreateBBFromBounds->UpdateObjectLists();
	if(this->CreateBBMeshSeed)
		this->CreateBBMeshSeed->UpdateObjectLists();
	if(this->EditBB)
		this->EditBB->UpdateObjectLists();
	if(this->SaveVTKBBGroup)
		this->SaveVTKBBGroup->UpdateObjectLists();
	if(this->EditBBMeshSeedGroup)
		this->EditBBMeshSeedGroup->UpdateObjectLists();
	if(this->DeleteObjectGroup)
		this->DeleteObjectGroup->UpdateObjectLists();
}

//------------------------------------------------------------------------------------------
int vtkKWMimxBBMenuGroup::CheckCellTypesInUgrid(vtkUnstructuredGrid *Input)
{
	vtkCellTypes *celltypes = vtkCellTypes::New();
	Input->GetCellTypes(celltypes);
	int i;
	for(i =0; i < celltypes->GetNumberOfTypes(); i++)
	{
		if(celltypes->GetCellType(i) != 12)
		{
			celltypes->Delete();
			return 0;
		}
	}
	celltypes->Delete();
	return 1;
}

//------------------------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::CreateBBFromMergingCallback()
{
	if (!this->MergeBBGroup)
	{
		this->MergeBBGroup = vtkKWMimxMergeBBGroup::New();
		this->MergeBBGroup->SetApplication(this->GetApplication());
		this->MergeBBGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->MergeBBGroup->SetSurfaceList(this->SurfaceList);
		this->MergeBBGroup->SetBBoxList(this->BBoxList);
		this->MergeBBGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->MergeBBGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->MergeBBGroup->SetDoUndoTree(this->DoUndoTree);
		this->MergeBBGroup->SetMenuGroup(this);
		this->MergeBBGroup->Create();
	}
	else
	{
		this->MergeBBGroup->UpdateObjectLists();
	}
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->SetMenuButtonsEnabled(0);
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		MergeBBGroup->GetMainFrame()->GetWidgetName()); 
}
//----------------------------------------------------------------------------------------------------
void vtkKWMimxBBMenuGroup::CreateBBFromRubberBandPickCallback()
{
	if (!this->CreateBBFromRubberBandPickGroup)
	{
		this->CreateBBFromRubberBandPickGroup = vtkKWMimxCreateBBFromRubberBandPickGroup::New();
		this->CreateBBFromRubberBandPickGroup->SetApplication(this->GetApplication());
		this->CreateBBFromRubberBandPickGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->CreateBBFromRubberBandPickGroup->SetSurfaceList(this->SurfaceList);
		this->CreateBBFromRubberBandPickGroup->SetBBoxList(this->BBoxList);
		this->CreateBBFromRubberBandPickGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateBBFromRubberBandPickGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->CreateBBFromRubberBandPickGroup->SetDoUndoTree(this->DoUndoTree);
		this->CreateBBFromRubberBandPickGroup->SetMenuGroup(this);
		this->CreateBBFromRubberBandPickGroup->Create();
	}
	else
	{
		this->CreateBBFromRubberBandPickGroup->UpdateObjectLists();
	}
	this->CreateBBFromRubberBandPickGroup->CreateBBFromRubberBandPickCallback( 1 );
	this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->SetMenuButtonsEnabled(0);
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		CreateBBFromRubberBandPickGroup->GetMainFrame()->GetWidgetName()); 
}
//----------------------------------------------------------------------------------------------------
