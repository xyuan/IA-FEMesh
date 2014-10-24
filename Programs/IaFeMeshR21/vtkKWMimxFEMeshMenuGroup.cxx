/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxFEMeshMenuGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/08/02 21:48:57 $
Version:   $Revision: 1.70 $

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
#include "vtkKWMimxFEMeshMenuGroup.h"
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
#include "vtkKWMimxEditElementSetNumbersGroup.h"
#include "vtkKWMimxEditNodeSetNumbersGroup.h"
#include "vtkKWMimxMergeMeshGroup.h"
#include "vtkKWMimxExportAbaqusFEMeshGroup.h"
#include "vtkKWMimxMergeBBGroup.h"
#include "vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxEditBBMeshSeedGroup.h"
#include "vtkCellTypes.h"

#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxEquivalancePoints.h"

#include "vtkMimxUnstructuredGridWidget.h"

#include "vtkMimxActorBase.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkMimxMeshActor.h"

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
#include "vtkKWMimxCreateFEMeshNodeSetGroup.h"
#include "vtkKWMimxCreateFEMeshElementSetGroup.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkKWComboBoxWithLabel.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxFEMeshMenuGroup);
vtkCxxRevisionMacro(vtkKWMimxFEMeshMenuGroup, "$Revision: 1.70 $");

//----------------------------------------------------------------------------
vtkKWMimxFEMeshMenuGroup::vtkKWMimxFEMeshMenuGroup()
{
	this->ObjectMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton = NULL;
	this->TypeMenuButton = NULL;
//  this->BBoxList = vtkLinkedListWrapper::New();
  this->FEMeshList = vtkLinkedListWrapper::New();
  this->ImageList = NULL;
  this->BBoxList = NULL;
  this->FEMeshFromBB = NULL;
  this->MainFrame = NULL;
  this->SaveVTKFEMeshGroup = NULL;
  this->FileBrowserDialog = NULL;
  this->DeleteObjectGroup = NULL;
  this->FEMeshLaplacianSmoothGroup = NULL;
  this->CreateExtrudeFEMeshGroup = NULL;
  this->EditElementSetNumbersGroup = NULL;
  this->MergeMeshGroup = NULL;
  this->ExportAbaqusFEMeshGroup = NULL;
  this->ImageMatPropGroup = NULL;
  this->FEMeshFromSurfaceExtractionGroup = NULL;
  this->CreateElementSetPickFaceGroup = NULL;
  this->CreateFEMeshNodeSetGroup = NULL;
  this->CreateFEMeshElementSetGroup = NULL;
  this->EditBBMeshSeedGroup = NULL;
  this->EditNodeSetNumbersGroup = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxFEMeshMenuGroup::~vtkKWMimxFEMeshMenuGroup()
{
	//if (this->BBoxList) {
 //   for (int i=0; i < this->BBoxList->GetNumberOfItems(); i++) {
 //     vtkMimxUnstructuredGridActor::SafeDownCast(
 //       this->BBoxList->GetItem(i))->Delete();
 //   }
	//	this->BBoxList->Delete();
	//}

  if (this->FEMeshList) {
    for (int i=0; i < this->FEMeshList->GetNumberOfItems(); i++) {
      vtkMimxMeshActor::SafeDownCast(
        this->FEMeshList->GetItem(i))->Delete();
    }
    this->FEMeshList->Delete();
  }

  if (this->OperationMenuButton)
    this->OperationMenuButton->Delete();
  if (this->TypeMenuButton)
    this->TypeMenuButton->Delete();
	if (this->ObjectMenuButton)
	  this->ObjectMenuButton->Delete();
  if (this->FEMeshFromBB)
    this->FEMeshFromBB->Delete();
  if(this->SaveVTKFEMeshGroup)
	  this->SaveVTKFEMeshGroup->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
  if(this->DeleteObjectGroup)
	  this->DeleteObjectGroup->Delete();
  if(this->FEMeshLaplacianSmoothGroup)
	  this->FEMeshLaplacianSmoothGroup->Delete();
  if(this->CreateExtrudeFEMeshGroup)
	  this->CreateExtrudeFEMeshGroup->Delete();
  if(this->EditElementSetNumbersGroup)
	  EditElementSetNumbersGroup->Delete();
  if(this->MergeMeshGroup)
	  this->MergeMeshGroup->Delete();
  if(this->ExportAbaqusFEMeshGroup)
	  this->ExportAbaqusFEMeshGroup->Delete();
  if (this->ImageMatPropGroup)
	  this->ImageMatPropGroup->Delete();
  if(this->FEMeshFromSurfaceExtractionGroup)
	  this->FEMeshFromSurfaceExtractionGroup->Delete();
  if(this->CreateElementSetPickFaceGroup)
	  this->CreateElementSetPickFaceGroup->Delete();
  if (this->CreateFEMeshNodeSetGroup)
	  this->CreateFEMeshNodeSetGroup->Delete();
  if (this->CreateFEMeshElementSetGroup)
	  this->CreateFEMeshElementSetGroup->Delete();
  if (this->EditBBMeshSeedGroup)
	  this->EditBBMeshSeedGroup->Delete();
  if(this->EditNodeSetNumbersGroup)
	  this->EditNodeSetNumbersGroup->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateWidget()
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
  //this->MainFrame->SetHeight(50);
  //this->MainFrame->SetLabelText("Mesh Operations");

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    /*"grid %s -row 0 -column 0 -padx 2 -pady 2 -sticky nswe",*/
    this->MainFrame->GetWidgetName());

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
	this->OperationMenuButton->SetEnabled(1);
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Assign/Edit Mesh Seeds",this, "EditBBMeshSeedCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Create",this, "CreateFEMeshFromBBCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Load",this, "LoadVTKFEMeshCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Renumber Nodes/Elements",this, "EditElementNumbersCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Export ABAQUS File",this, "ExportAbaqusFEMeshCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Delete",this, "DeleteFEMeshCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Save",this, "SaveVTKFEMeshCallback");
	/*  
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Extrude Mesh",this, "CreateFEMeshFromSurfaceExtrusionCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Create Surface Mesh",this, "CreateFEMeshFromSurfaceExtractionCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Create Node Set",this, "CreateFEMeshNodeSetCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Create Element Set",this, "CreateFEMeshElementSetCallback");
  */
	
	  
	// declare type menu
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
	this->OperationMenuButton->GetWidget()->SetValue( "Assign/Edit Mesh Seeds" );	
	//this->EditFEMeshCallback();
	
    
}
//----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
/*
void vtkKWMimxFEMeshMenuGroup::FEMeshMenuCallback()
{
 // this->HideAllDialogBoxes();
  this->OperationMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
  this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
  this->OperationMenuButton->GetWidget()->SetValue("");
  this->TypeMenuButton->GetWidget()->SetValue("");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Load",this, "LoadFEMeshCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Save",this, "SaveFEMeshCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Export",this, "ExportFEMeshCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Create",this, "CreateFEMeshCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Delete",this, "DeleteFEMeshCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Edit",this, "EditFEMeshCallback");

  this->OperationMenuButton->SetEnabled(1);

}
*/

//-----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::HideAllDialogBoxes()
{
  if (this->FEMeshFromBB)
  {
	  this->GetApplication()->Script("pack forget %s", 
		  this->FEMeshFromBB->GetMainFrame()->GetWidgetName());
  }
}
//-----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateFEMeshCallback()
{
//  this->HideAllDialogBoxes();
  this->TypeMenuButton->SetLabelText("Type :");
  this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
  this->TypeMenuButton->GetWidget()->SetValue("");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "From Building Block",this, "CreateFEMeshFromBBCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "From Surface Extrusion",this, "CreateFEMeshFromSurfaceExtrusionCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Merge",this, "CreateFEMeshFromMergingCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "From Surface Extraction",this, "CreateFEMeshFromSurfaceExtractionCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Node Set",this, "CreateFEMeshNodeSetCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Element Set",this, "CreateFEMeshElementSetCallback");

  this->TypeMenuButton->SetEnabled(1);
}
//-----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateFEMeshFromBBCallback()
{
//  this->HideAllDialogBoxes();
  if (!this->FEMeshFromBB)
  {
    this->FEMeshFromBB = vtkKWMimxCreateFEMeshFromBBGroup::New();
    this->FEMeshFromBB->SetApplication(this->GetApplication());
    this->FEMeshFromBB->SetParent( this->GetParent() /*this->MainFrame->GetFrame()*/);
    this->FEMeshFromBB->SetSurfaceList(this->SurfaceList);
    this->FEMeshFromBB->SetBBoxList(this->BBoxList);
    this->FEMeshFromBB->SetFEMeshList(this->FEMeshList);
    this->FEMeshFromBB->SetMimxMainWindow(this->GetMimxMainWindow());
	this->FEMeshFromBB->SetViewProperties(
		this->GetMimxMainWindow()->GetViewProperties());
	this->FEMeshFromBB->SetMenuGroup(this);
//	this->SetMenuButtonsEnabled(0);
    this->FEMeshFromBB->Create();
  }
  else
  {
	  this->FEMeshFromBB->UpdateObjectLists();
  }
  this->SetMenuButtonsEnabled(0);
    this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
    this->FEMeshFromBB->GetMainFrame()->GetWidgetName());
 
}
//-----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::EditFEMeshCallback()
{
  this->TypeMenuButton->SetLabelText("Type :");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Mesh Seed",this, "EditBBMeshSeedCallback");
    
	//this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	//	"Smooth - Laplacian",this, "SmoothLaplacianFEMeshCallback");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Element Numbers",this, "EditElementNumbersCallback");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Node Numbers",this, "EditNodeNumbersCallback");

	this->TypeMenuButton->SetEnabled(1);

}
//-----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::LoadFEMeshCallback()
{
  this->TypeMenuButton->SetLabelText("Format :");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"VTK",this, "LoadVTKFEMeshCallback");

	this->TypeMenuButton->SetEnabled(1);
}
//-----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::SaveFEMeshCallback()
{
  this->TypeMenuButton->SetLabelText("Format :");
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"VTK",this, "SaveVTKFEMeshCallback");

	this->TypeMenuButton->SetEnabled(1);
}
//-----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::LoadVTKFEMeshCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	callback->SetState(0);
	if(!this->FileBrowserDialog)
	{
		this->FileBrowserDialog = vtkKWLoadSaveDialog::New() ;
		this->FileBrowserDialog->SetApplication(this->GetApplication());
		this->FileBrowserDialog->Create();
	}
	this->FileBrowserDialog->SetTitle ("Load VTK Unstructured Grid File Format");
	this->FileBrowserDialog->SetFileTypes ("{{VTK files} {.vtk}}");
	this->FileBrowserDialog->SetDefaultExtension (".vtk");
	this->FileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	this->FileBrowserDialog->Invoke();
	if(FileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		if(FileBrowserDialog->GetFileName())
		{
			std::string meshFileName = vtksys::SystemTools::CollapseFullPath( this->FileBrowserDialog->GetFileName() );
			
			this->FileBrowserDialog->SetLastPath(this->FileBrowserDialog->GetLastPath());
			this->GetApplication()->SetRegistryValue(
				1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( meshFileName ).c_str());
			this->FileBrowserDialog->SaveLastPathToRegistry("LastPath");
			
			vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
			reader->SetFileName( meshFileName.c_str() );
			reader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
			reader->Update();
			if(!callback->GetState())
			{
				vtkMimxMeshActor *meshActor = vtkMimxMeshActor::New();
				this->FEMeshList->AppendItem(meshActor);
				meshActor->SetDataSet(reader->GetOutput());
				meshActor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
				meshActor->SetInteractor( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor() );
				
				this->GetMimxMainWindow()->GetRenderWidget()->Render();
				this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
				int item = this->FEMeshList->GetNumberOfItems()-1;
				this->FEMeshList->GetItem(item)->SetFilePath(meshFileName.c_str());
				this->FEMeshList->GetItem(item)->SetFoundationName(this->ExtractFileName(meshFileName.c_str()));
				this->UpdateObjectLists();
				this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(this->FEMeshList->GetItem(item));
				
				vtkKWMimxApplication *app
          = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
        std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( meshFileName );
        //std::cout << "Mesh : " << meshFileName << std::endl;
        //std::cout << "Working Path Dir : " << surfaceDirectory << std::endl;
        app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
      
				this->GetMimxMainWindow()->SetStatusText("Loaded VTK Mesh");
			}
			reader->Delete();
		}
	}
}

//---------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::DeleteFEMeshCallback()
{
	if(!this->DeleteObjectGroup)
	{
		this->DeleteObjectGroup = vtkKWMimxDeleteObjectGroup::New();
		this->DeleteObjectGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(NULL);
		this->DeleteObjectGroup->SetFEMeshList(this->FEMeshList);
		this->DeleteObjectGroup->SetMenuGroup(this);
		this->DeleteObjectGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->DeleteObjectGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->DeleteObjectGroup->Create();
		//this->TypeMenuButton->GetWidget()->SetValue("");
		//this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
		//this->TypeMenuButton->SetEnabled(0);
	//	this->SetMenuButtonsEnabled(0);
	}
	else
	{
//		this->DeleteObjectGroup->SetViewProperties(this->MimxViewProperties);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(NULL);
		this->DeleteObjectGroup->SetFEMeshList(this->FEMeshList);
		this->DeleteObjectGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->DeleteObjectGroup->GetObjectListComboBox()->SetLabelText("Mesh: ");
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->DeleteObjectGroup->GetMainFrame()->GetWidgetName());
}
//---------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::SaveVTKFEMeshCallback()
{
	if(!this->SaveVTKFEMeshGroup)
	{
		this->SaveVTKFEMeshGroup = vtkKWMimxSaveVTKFEMeshGroup::New();
		this->SaveVTKFEMeshGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->SaveVTKFEMeshGroup->SetFEMeshList(this->FEMeshList);
		this->SaveVTKFEMeshGroup->SetMenuGroup(this);
		this->SaveVTKFEMeshGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->SaveVTKFEMeshGroup->Create();
//		this->SetMenuButtonsEnabled(0);
	}
	else
	{
		this->SaveVTKFEMeshGroup->UpdateObjectLists();
	}

	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
								   this->SaveVTKFEMeshGroup->GetMainFrame()->GetWidgetName());
}
//------------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::SmoothLaplacianFEMeshCallback()
{
	//if(!this->FEMeshLaplacianSmoothGroup)
	//{
	//	this->FEMeshLaplacianSmoothGroup = vtkKWMimxEditFEMeshLaplacianSmoothGroup::New();
	//	this->FEMeshLaplacianSmoothGroup->SetParent(this->MainFrame->GetFrame());
	//	this->FEMeshLaplacianSmoothGroup->SetSurfaceList(this->SurfaceList);
	//	this->FEMeshLaplacianSmoothGroup->SetFEMeshList(this->FEMeshList);
	//	this->FEMeshLaplacianSmoothGroup->SetBBoxList(this->BBoxList);
	//	this->FEMeshLaplacianSmoothGroup->SetMenuGroup(this);
	//	this->FEMeshLaplacianSmoothGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	//	this->FEMeshLaplacianSmoothGroup->Create();
	////	this->SetMenuButtonsEnabled(0);
	//}
	//else
	//{
	//	this->FEMeshLaplacianSmoothGroup->UpdateObjectLists();
	//}
	//this->SetMenuButtonsEnabled(0);
	//  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	//this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
	//	this->FEMeshLaplacianSmoothGroup->GetMainFrame()->GetWidgetName());
}
//------------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateFEMeshFromSurfaceExtrusionCallback()
{
	if (!this->CreateExtrudeFEMeshGroup)
	{
		this->CreateExtrudeFEMeshGroup = vtkKWMimxCreateExtrudeFEMeshGroup::New();
		this->CreateExtrudeFEMeshGroup->SetApplication(this->GetApplication());
		this->CreateExtrudeFEMeshGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->CreateExtrudeFEMeshGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateExtrudeFEMeshGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->CreateExtrudeFEMeshGroup->SetMenuGroup(this);
//		this->CreateExtrudeFEMeshGroup->SetSurfaceList(this->SurfaceList);
		this->CreateExtrudeFEMeshGroup->SetFEMeshList(this->FEMeshList);
//		this->SetMenuButtonsEnabled(0);
		this->CreateExtrudeFEMeshGroup->Create();
	}
	else
	{
		this->CreateExtrudeFEMeshGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->CreateExtrudeFEMeshGroup->GetMainFrame()->GetWidgetName());
}
//--------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::UpdateObjectLists()
{
	if(this->FEMeshFromBB)
		this->FEMeshFromBB->UpdateObjectLists();
	if(this->SaveVTKFEMeshGroup)
		this->SaveVTKFEMeshGroup->UpdateObjectLists();
	if(this->DeleteObjectGroup)
		this->DeleteObjectGroup->UpdateObjectLists();
	if(this->FEMeshLaplacianSmoothGroup)
		this->FEMeshLaplacianSmoothGroup->UpdateObjectLists();
	if(this->CreateExtrudeFEMeshGroup)
		this->CreateExtrudeFEMeshGroup->UpdateObjectLists();
	//if(this->EditElementSetNumbersGroup)
	//	this->EditElementSetNumbersGroup->UpdateObjectLists();
	if(this->FEMeshFromSurfaceExtractionGroup)
		this->FEMeshFromSurfaceExtractionGroup->UpdateObjectLists();
}
//------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::EditElementNumbersCallback()
{
	if(!this->EditElementSetNumbersGroup)
	{
		this->EditElementSetNumbersGroup = vtkKWMimxEditElementSetNumbersGroup::New();
		this->EditElementSetNumbersGroup->SetParent(this->MainFrame);
		this->EditElementSetNumbersGroup->SetFEMeshList(this->FEMeshList);
		this->EditElementSetNumbersGroup->SetMenuGroup(this);
		this->EditElementSetNumbersGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->EditElementSetNumbersGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->EditElementSetNumbersGroup->Create();
	}
	else
	{
		this->EditElementSetNumbersGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
		this->EditElementSetNumbersGroup->GetMainFrame()->GetWidgetName());
}
//-------------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::EditImageBasedMatPropsCallback()
{
//	if(!this->ImageMatPropGroup)
//	{
//		this->ImageMatPropGroup = vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup::New();
//		this->ImageMatPropGroup->SetParent(this->MainFrame->GetFrame());
//		//		this->EditElementSetNumbersGroup->SetSurfaceList(this->SurfaceList);
//		this->ImageMatPropGroup->SetFEMeshList(this->FEMeshList);
//		this->ImageMatPropGroup->SetImageList(this->ImageList);
//		this->ImageMatPropGroup->SetMenuGroup(this);
//		this->ImageMatPropGroup->SetMimxMainWindow(this->GetMimxMainWindow());
//		this->ImageMatPropGroup->SetViewProperties(
//			this->GetMimxMainWindow()->GetViewProperties());
////		this->ImageMatPropGroup->SetViewProperties(this->MimxViewProperties);
//		this->ImageMatPropGroup->Create();
//	}
//	else
//	{
//		this->ImageMatPropGroup->UpdateObjectLists();
//	}
//	this->SetMenuButtonsEnabled(0);
//	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
//
//	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
//		this->ImageMatPropGroup->GetMainFrame()->GetWidgetName());
}
//-------------------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateFEMeshFromMergingCallback()
{
	if (!this->MergeMeshGroup)
	{
		this->MergeMeshGroup = vtkKWMimxMergeMeshGroup::New();
		this->MergeMeshGroup->SetApplication(this->GetApplication());
		this->MergeMeshGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->MergeMeshGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->MergeMeshGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->MergeMeshGroup->SetMenuGroup(this);
		this->MergeMeshGroup->SetFEMeshList(this->FEMeshList);
		this->MergeMeshGroup->Create();
	}
	else
	{
		this->MergeMeshGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->MergeMeshGroup->GetMainFrame()->GetWidgetName());
}
//------------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::ExportFEMeshCallback()
{
	this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	this->TypeMenuButton->GetWidget()->SetValue("");
	this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Abaqus",this, "ExportAbaqusFEMeshCallback");

	this->TypeMenuButton->SetEnabled(1);
}
//------------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::ExportAbaqusFEMeshCallback()
{
	if(!this->ExportAbaqusFEMeshGroup)
	{
		this->ExportAbaqusFEMeshGroup = vtkKWMimxExportAbaqusFEMeshGroup::New();
		this->ExportAbaqusFEMeshGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->ExportAbaqusFEMeshGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->ExportAbaqusFEMeshGroup->SetFEMeshList(this->FEMeshList);
		this->ExportAbaqusFEMeshGroup->SetMenuGroup(this);
		this->ExportAbaqusFEMeshGroup->Create();
		//		this->SetMenuButtonsEnabled(0);
	}
	else
	{
		this->ExportAbaqusFEMeshGroup->UpdateObjectLists();
	}

	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->ExportAbaqusFEMeshGroup->GetMainFrame()->GetWidgetName());
}
//------------------------------------------------------------------------------------------
int vtkKWMimxFEMeshMenuGroup::CheckCellTypesInUgrid(vtkUnstructuredGrid *Input)
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

//----------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateFEMeshFromSurfaceExtractionCallback()
{
	if (!this->FEMeshFromSurfaceExtractionGroup)
	{
		this->FEMeshFromSurfaceExtractionGroup = 
			vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup::New();
		this->FEMeshFromSurfaceExtractionGroup->SetApplication(this->GetApplication());
		this->FEMeshFromSurfaceExtractionGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->FEMeshFromSurfaceExtractionGroup->SetFEMeshList(this->FEMeshList);
		this->FEMeshFromSurfaceExtractionGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->FEMeshFromSurfaceExtractionGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->FEMeshFromSurfaceExtractionGroup->SetMenuGroup(this);
		this->FEMeshFromSurfaceExtractionGroup->Create();
	}
	else
	{
		this->FEMeshFromSurfaceExtractionGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->FEMeshFromSurfaceExtractionGroup->GetMainFrame()->GetWidgetName()); 
}
// ---------------------------------------------------------------------

void vtkKWMimxFEMeshMenuGroup::EditCreateElementSetPickFaceCallback()
{

	//  this->HideAllDialogBoxes();
	if (!this->CreateElementSetPickFaceGroup)
	{
		this->CreateElementSetPickFaceGroup = vtkKWMimxCreateElementSetPickFaceGroup::New();
		this->CreateElementSetPickFaceGroup->SetApplication(this->GetApplication());
		this->CreateElementSetPickFaceGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->CreateElementSetPickFaceGroup->SetSurfaceList(this->SurfaceList);
		this->CreateElementSetPickFaceGroup->SetBBoxList(this->BBoxList);
		this->CreateElementSetPickFaceGroup->SetFEMeshList(this->FEMeshList);
		this->CreateElementSetPickFaceGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateElementSetPickFaceGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
//		this->CreateElementSetPickFaceGroup->SetViewProperties(this->MimxViewProperties);
		this->CreateElementSetPickFaceGroup->SetMenuGroup(this);
		//	this->SetMenuButtonsEnabled(0);
		this->CreateElementSetPickFaceGroup->Create();
	}
	else
	{
		this->CreateElementSetPickFaceGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->CreateElementSetPickFaceGroup->GetMainFrame()->GetWidgetName());
}
//---------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateFEMeshNodeSetCallback()
{
	if (!this->CreateFEMeshNodeSetGroup)
	{
		this->CreateFEMeshNodeSetGroup = 
			vtkKWMimxCreateFEMeshNodeSetGroup::New();
		this->CreateFEMeshNodeSetGroup->SetApplication(this->GetApplication());
		this->CreateFEMeshNodeSetGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->CreateFEMeshNodeSetGroup->SetFEMeshList(this->FEMeshList);
		this->CreateFEMeshNodeSetGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateFEMeshNodeSetGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->CreateFEMeshNodeSetGroup->SetMenuGroup(this);
		this->CreateFEMeshNodeSetGroup->Create();
	}
	else
	{
		this->CreateFEMeshNodeSetGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->CreateFEMeshNodeSetGroup->GetMainFrame()->GetWidgetName()); 
}
//-------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::CreateFEMeshElementSetCallback()
{
	if (!this->CreateFEMeshElementSetGroup)
	{
		this->CreateFEMeshElementSetGroup = 
			vtkKWMimxCreateFEMeshElementSetGroup::New();
		this->CreateFEMeshElementSetGroup->SetApplication(this->GetApplication());
		this->CreateFEMeshElementSetGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->CreateFEMeshElementSetGroup->SetFEMeshList(this->FEMeshList);
		this->CreateFEMeshElementSetGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateFEMeshElementSetGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->CreateFEMeshElementSetGroup->SetMenuGroup(this);
		this->CreateFEMeshElementSetGroup->Create();
	}
	else
	{
		this->CreateFEMeshElementSetGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->CreateFEMeshElementSetGroup->GetMainFrame()->GetWidgetName()); 
}

//------------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::EditBBMeshSeedCallback()
{
	if(!this->EditBBMeshSeedGroup)
	{
		this->EditBBMeshSeedGroup = vtkKWMimxEditBBMeshSeedGroup::New();
		this->EditBBMeshSeedGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->EditBBMeshSeedGroup->SetSurfaceList(this->SurfaceList);
		this->EditBBMeshSeedGroup->SetFEMeshList(this->FEMeshList);
		this->EditBBMeshSeedGroup->SetBBoxList(this->BBoxList);
		this->EditBBMeshSeedGroup->SetMenuGroup(this);
		this->EditBBMeshSeedGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->EditBBMeshSeedGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->EditBBMeshSeedGroup->Create();
	//	this->SetMenuButtonsEnabled(0);
	}
	else
	{
		this->EditBBMeshSeedGroup->UpdateObjectLists();
	}
	this->EditBBMeshSeedGroup->InitializeState();
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
		this->EditBBMeshSeedGroup->GetMainFrame()->GetWidgetName());
}
//-------------------------------------------------------------------------------------
void vtkKWMimxFEMeshMenuGroup::EditNodeNumbersCallback()
{
	if(!this->EditNodeSetNumbersGroup)
	{
		this->EditNodeSetNumbersGroup = vtkKWMimxEditNodeSetNumbersGroup::New();
		this->EditNodeSetNumbersGroup->SetParent(this->MainFrame);
		this->EditNodeSetNumbersGroup->SetFEMeshList(this->FEMeshList);
		this->EditNodeSetNumbersGroup->SetMenuGroup(this);
		this->EditNodeSetNumbersGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->EditNodeSetNumbersGroup->SetViewProperties(
			this->GetMimxMainWindow()->GetViewProperties());
		this->EditNodeSetNumbersGroup->Create();
	}
	else
	{
		this->EditNodeSetNumbersGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
		this->EditNodeSetNumbersGroup->GetMainFrame()->GetWidgetName());
}
//-------------------------------------------------------------------------------------------

