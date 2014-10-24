/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxQualityMenuGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/07/29 02:14:49 $
Version:   $Revision: 1.19 $

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

#include "vtkKWMimxQualityMenuGroup.h"
#include "vtkKWMimxMainNotebook.h"

#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxImageViewProperties.h"

#include "vtkMimxActorBase.h"
#include "vtkKWMimxDeleteObjectGroup.h"
#include "vtkMimxImageActor.h"

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
#include "vtkKWMimxEvaluateMeshQualityGroup.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkKWMimxEditFEMeshLaplacianSmoothGroup.h"
#include "vtkKWMimxExportAbaqusFEMeshGroup.h"
#include "vtkKWMimxSaveVTKFEMeshGroup.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxQualityMenuGroup);
vtkCxxRevisionMacro(vtkKWMimxQualityMenuGroup, "$Revision: 1.19 $");

//----------------------------------------------------------------------------
vtkKWMimxQualityMenuGroup::vtkKWMimxQualityMenuGroup()
{
	this->ObjectMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton = NULL;
	this->TypeMenuButton = NULL;
  this->FileBrowserDialog = NULL;
  this->EvaluateMeshInterface = NULL;
  this->FEMeshLaplacianSmoothGroup = NULL;
  this->ExportAbaqusFEMeshGroup = NULL;
	this->SaveVTKFEMeshGroup = NULL;
}
//----------------------------------------------------------------------------
vtkKWMimxQualityMenuGroup::~vtkKWMimxQualityMenuGroup()
{
	this->ObjectMenuButton->Delete();

  if(this->OperationMenuButton)
    this->OperationMenuButton->Delete();
  if(this->TypeMenuButton)
    this->TypeMenuButton->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
  if(this->EvaluateMeshInterface)
	  this->EvaluateMeshInterface->Delete();
	if(this->FEMeshLaplacianSmoothGroup)
	  this->FEMeshLaplacianSmoothGroup->Delete();
	if(this->ExportAbaqusFEMeshGroup)
    this->ExportAbaqusFEMeshGroup->Delete();
  if(this->SaveVTKFEMeshGroup)
    this->SaveVTKFEMeshGroup->Delete();
 }
//----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::CreateWidget()
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
  //this->MainFrame->SetLabelText("Quality Operations");

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
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
	this->OperationMenuButton->GetWidget()->SetWidth(40);
	//this->OperationMenuButton->SetLabelWidth(10);
	this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 15", 
		this->OperationMenuButton->GetWidgetName());
	this->OperationMenuButton->SetEnabled(1);
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Evaluate / Display Mesh Quality",this, "EvaluateMeshCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Mesh Improvement",this, "SmoothLaplacianFEMeshCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Export Abaqus File",this, "ExportAbaqusFEMeshCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Save",this, "SaveVTKFEMeshCallback");
  //this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	//	"Improve",this, "ImproveMeshCallback");
		
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
	//this->MessageDialog->SetApplication(this->GetApplication());
	
	this->OperationMenuButton->GetWidget()->SetValue( "Evaluate / Display Mesh Quality" );	
	//this->MeshImprovementCallback();
	
}
//----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::EvaluateMeshCallback()
{
  this->TypeMenuButton->GetWidget()->SetValue("");
  this->TypeMenuButton->SetEnabled(0);
//  this->HideAllDialogBoxes();
/*
  this->TypeMenuButton->SetLabelText("Metric :");
  this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
  this->TypeMenuButton->GetWidget()->SetValue("");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Volume",this, "EvaluateMeshVolumeCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Edge Collapse",this, "EvaluateMeshEdgeCollapseCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Jacobian",this, "EvaluateMeshJacobianCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Skew",this, "EvaluateMeshSkewCallback");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Angle Out Of Bounds",this, "EvaluateMeshAngleCallback");
  this->TypeMenuButton->SetEnabled(1);
*/  
  CreateMeshQualityGroup();
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::MeshImprovementCallback()
{
//  this->HideAllDialogBoxes();
  this->TypeMenuButton->SetLabelText("Type :");
  this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
  this->TypeMenuButton->GetWidget()->SetValue("");
  this->TypeMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Laplacian Smoothing",this, "SmoothLaplacianFEMeshCallback");
  this->TypeMenuButton->SetEnabled(1);
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::EvaluateMeshVolumeCallback()
{
  CreateMeshQualityGroup();
  this->EvaluateMeshInterface->SetQualityTypeToVolume();
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::EvaluateMeshEdgeCollapseCallback()
{
  CreateMeshQualityGroup();
  this->EvaluateMeshInterface->SetQualityTypeToEdgeCollapse();
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::EvaluateMeshJacobianCallback()
{
  CreateMeshQualityGroup();
  this->EvaluateMeshInterface->SetQualityTypeToJacobian();
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::EvaluateMeshSkewCallback()
{
  CreateMeshQualityGroup();
  this->EvaluateMeshInterface->SetQualityTypeToSkew();
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::EvaluateMeshAngleCallback()
{
  CreateMeshQualityGroup();
  this->EvaluateMeshInterface->SetQualityTypeToAngle();
}

//-----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::CreateMeshQualityGroup()
{
  if (!this->EvaluateMeshInterface)
  {
    this->EvaluateMeshInterface = vtkKWMimxEvaluateMeshQualityGroup::New();
    this->EvaluateMeshInterface->SetApplication(this->GetApplication());
    this->EvaluateMeshInterface->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
    this->EvaluateMeshInterface->SetFEMeshList(this->FEMeshList);
    this->EvaluateMeshInterface->SetMimxMainWindow(this->GetMimxMainWindow());
	  this->EvaluateMeshInterface->SetViewProperties(
		   this->GetMimxMainWindow()->GetViewProperties());
	  this->EvaluateMeshInterface->SetMenuGroup(this);
    this->EvaluateMeshInterface->Create();
  }
  else
  {
	  this->EvaluateMeshInterface->UpdateObjectLists();
  }
  this->EvaluateMeshInterface->ClearStatsEntry();
  
  this->SetMenuButtonsEnabled(0);
    this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
    this->EvaluateMeshInterface->GetMainFrame()->GetWidgetName());
 
}
//------------------------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::SmoothLaplacianFEMeshCallback()
{
	if(!this->FEMeshLaplacianSmoothGroup)
	{
		this->FEMeshLaplacianSmoothGroup = vtkKWMimxEditFEMeshLaplacianSmoothGroup::New();
		this->FEMeshLaplacianSmoothGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->FEMeshLaplacianSmoothGroup->SetSurfaceList(this->SurfaceList);
		this->FEMeshLaplacianSmoothGroup->SetFEMeshList(this->FEMeshList);
		this->FEMeshLaplacianSmoothGroup->SetBBoxList(this->BBoxList);
		this->FEMeshLaplacianSmoothGroup->SetMenuGroup(this);
		this->FEMeshLaplacianSmoothGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->FEMeshLaplacianSmoothGroup->Create();
	}
	else
	{
		this->FEMeshLaplacianSmoothGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->FEMeshLaplacianSmoothGroup->GetMainFrame()->GetWidgetName());
}
//--------------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::UpdateObjectLists()
{
	if(this->EvaluateMeshInterface)
		this->EvaluateMeshInterface->UpdateObjectLists();
}
//---------------------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::SaveVTKFEMeshCallback()
{
	if(!this->SaveVTKFEMeshGroup)
	{
		this->SaveVTKFEMeshGroup = vtkKWMimxSaveVTKFEMeshGroup::New();
		this->SaveVTKFEMeshGroup->SetParent(this->GetParent() /*this->MainFrame->GetFrame()*/);
		this->SaveVTKFEMeshGroup->SetFEMeshList(this->FEMeshList);
		this->SaveVTKFEMeshGroup->SetMenuGroup(this);
		this->SaveVTKFEMeshGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->SaveVTKFEMeshGroup->Create();
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
void vtkKWMimxQualityMenuGroup::ExportAbaqusFEMeshCallback()
{
	if(!this->ExportAbaqusFEMeshGroup)
	{
		this->ExportAbaqusFEMeshGroup = vtkKWMimxExportAbaqusFEMeshGroup::New();
		this->ExportAbaqusFEMeshGroup->SetParent( this->GetParent() );
		this->ExportAbaqusFEMeshGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->ExportAbaqusFEMeshGroup->SetFEMeshList(this->FEMeshList);
		this->ExportAbaqusFEMeshGroup->SetMenuGroup(this);
		this->ExportAbaqusFEMeshGroup->Create();
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
//----------------------------------------------------------------------------
void vtkKWMimxQualityMenuGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//---------------------------------------------------------------------------
