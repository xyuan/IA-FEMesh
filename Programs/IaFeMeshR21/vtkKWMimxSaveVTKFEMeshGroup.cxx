/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveVTKFEMeshGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/08/09 03:20:34 $
Version:   $Revision: 1.29 $

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

#include "vtkKWMimxSaveVTKFEMeshGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"

#include "vtkLinkedListWrapper.h"

#include "vtkActor.h"
#include "vtkMimxMeshActor.h"
#include "vtkUnstructuredGrid.h"

#include "vtkKWApplication.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWNotebook.h"
#include "vtkKWOptions.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkObjectFactory.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWComboBox.h"
#include "vtkKWPushButton.h"
#include "vtkRenderer.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkMimxMeshActor.h"

#include "vtkUnstructuredGridWriter.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSaveVTKFEMeshGroup);
vtkCxxRevisionMacro(vtkKWMimxSaveVTKFEMeshGroup, "$Revision: 1.29 $");

//----------------------------------------------------------------------------
vtkKWMimxSaveVTKFEMeshGroup::vtkKWMimxSaveVTKFEMeshGroup()
{
  this->ObjectListComboBox = NULL;
  this->FileBrowserDialog = NULL;
  this->ComponentFrame = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxSaveVTKFEMeshGroup::~vtkKWMimxSaveVTKFEMeshGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
	if(this->ComponentFrame)
	  this->ComponentFrame->Delete();
 }
//----------------------------------------------------------------------------
void vtkKWMimxSaveVTKFEMeshGroup::CreateWidget()
{
  if(this->IsCreated())
  {
  vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();
  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Save FE Mesh (VTK file format)");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->MainFrame->GetWidgetName());
/*
  if (!this->ComponentFrame)	
     this->ComponentFrame = vtkKWFrameWithLabel::New();
  this->ComponentFrame->SetParent(this->MainFrame);
  this->ComponentFrame->Create();
  this->ComponentFrame->SetLabelText("Mesh");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
    this->ComponentFrame->GetWidgetName());
*/    
  if (!this->ObjectListComboBox)	
     this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();  
  ObjectListComboBox->SetParent(this->MainFrame);
  ObjectListComboBox->Create();
  ObjectListComboBox->SetLabelText("Mesh : ");
  ObjectListComboBox->SetLabelWidth( 15 );
  ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ObjectListComboBox->GetWidgetName());

  this->ApplyButton->SetParent(this->MainFrame);
  this->ApplyButton->Create();
  this->ApplyButton->SetReliefToFlat();
  this->ApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
  this->ApplyButton->SetCommand(this, "SaveVTKFEMeshApplyCallback");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand y -padx 5 -pady 6", 
	  this->ApplyButton->GetWidgetName());

  this->CancelButton->SetParent(this->MainFrame);
  this->CancelButton->Create();
  this->CancelButton->SetReliefToFlat();
  this->CancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
  this->CancelButton->SetCommand(this, "SaveVTKFEMeshCancelCallback");
  this->GetApplication()->Script(
    "pack %s -side right -anchor ne -expand y -padx 5 -pady 6", 
    this->CancelButton->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkKWMimxSaveVTKFEMeshGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSaveVTKFEMeshGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxSaveVTKFEMeshGroup::SaveVTKFEMeshApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("FE Mesh selection required");;
		return 0;
	}

  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid FE Mesh");
		combobox->SetValue("");
		return 0;
	}

  vtkUnstructuredGrid *ugrid = vtkMimxMeshActor::SafeDownCast(this->FEMeshList
     ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
  const char *filename = this->FEMeshList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	if(!this->FileBrowserDialog)
	{
		this->FileBrowserDialog = vtkKWLoadSaveDialog::New() ;
		this->FileBrowserDialog->SaveDialogOn();
		this->FileBrowserDialog->SetApplication(this->GetApplication());
//		dialog->SetParent(this->RenderWidget->GetParentTopLevel()) ;
		this->FileBrowserDialog->Create();	
		this->FileBrowserDialog->RetrieveLastPathFromRegistry("FEMeshDataPath");
		this->FileBrowserDialog->SetTitle ("Save Mesh - VTK Format");
		this->FileBrowserDialog->SetFileTypes ("{{VTK files} {.vtk}}");
		this->FileBrowserDialog->SetDefaultExtension (".vtk");
	}
	this->FileBrowserDialog->SetInitialFileName(filename);
	this->FileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	this->FileBrowserDialog->Invoke();
	if(this->FileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		if(this->FileBrowserDialog->GetFileName())
		{
			const char *filename = FileBrowserDialog->GetFileName();
			this->GetApplication()->SetRegistryValue(
				1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( filename ).c_str());
			this->FileBrowserDialog->SaveLastPathToRegistry("LastPath");
			vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
			writer->SetFileName(this->FileBrowserDialog->GetFileName());
			writer->SetInput(ugrid);
			writer->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
			writer->Update();
			writer->RemoveObserver(callback);
			if(callback->GetState())
			{
				writer->Delete();
				return 0;
			}
			writer->Delete();		
			this->GetMimxMainWindow()->SetStatusText("Saved VTK Mesh");
			return 1;
		}
	}
	return 0;
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveVTKFEMeshGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveVTKFEMeshGroup::SaveVTKFEMeshCancelCallback()
{
//  this->MainFrame->UnpackChildren();
  this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName());
  this->MenuGroup->SetMenuButtonsEnabled(1);
    this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
}
//------------------------------------------------------------------------------
void vtkKWMimxSaveVTKFEMeshGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->FEMeshList->GetNumberOfItems(); i++)
	{
		ObjectListComboBox->GetWidget()->AddValue(
			this->FEMeshList->GetItem(i)->GetFileName());
		
		vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->FEMeshList->GetItem(i));  
    bool viewedItem = meshActor->GetMeshVisibility();
    
		if ( (defaultItem == -1) && ( viewedItem ) )
		{
		  defaultItem = i;
		}
	}
	
	if ((this->FEMeshList->GetNumberOfItems() > 0) && (defaultItem == -1))
    defaultItem = this->FEMeshList->GetNumberOfItems()-1;
    
	if (defaultItem != -1)
  {
    ObjectListComboBox->GetWidget()->SetValue(
          this->FEMeshList->GetItem(defaultItem)->GetFileName());
  }
}
//---------------------------------------------------------------------------------
