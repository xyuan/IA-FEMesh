/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMainNotebook.cxx,v $
Language:  C++
Date:      $Date: 2008/07/10 01:46:02 $
Version:   $Revision: 1.39 $

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

#include "vtkKWMimxMainNotebook.h"

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkSTLReader.h"

#include "vtkKWApplication.h"
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
#include "vtkKWUserInterfacePanel.h"
#include "vtkKWOptions.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxImageMenuGroup.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWUserInterfacePanel.h"
#include "vtkKWMimxQualityMenuGroup.h"
#include "vtkKWMimxMaterialPropertyMenuGroup.h"
#include "vtkKWMimxBoundaryConditionsMenuGroup.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include <vtkKWUserInterfaceManagerNotebook.h>
#include <vtkKWNotebook.h>
#include <vtkKWEvent.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxMainNotebook);
vtkCxxRevisionMacro(vtkKWMimxMainNotebook, "$Revision: 1.39 $");

//----------------------------------------------------------------------------
vtkKWMimxMainNotebook::vtkKWMimxMainNotebook()
{
	this->MainFrame = NULL;
	this->Notebook = NULL;
	this->MimxMainWindow = NULL;
	this->SurfaceMenuGroup = NULL;
	this->BBMenuGroup = NULL;
	this->FEMeshMenuGroup = NULL;
	this->ImageMenuGroup = NULL;
  this->QualityMenuGroup = NULL;
	this->DoUndoTree = NULL;
	this->MaterialPropertyMenuGroup = NULL;
	this->BoundaryConditionsMenuGroup = NULL;
	
	startNotebookPage = 1;
  showNumberOfPages = 3;
  
  for (int i=0; i<9; i++)
  {
    this->tabLabels[i] = new char[32];
  }
  strcpy(this->tabLabels[0], "<");
  strcpy(this->tabLabels[1], "Image");
  strcpy(this->tabLabels[2], "Surface");
  strcpy(this->tabLabels[3], "Block(s)");
  strcpy(this->tabLabels[4], "Mesh");
  strcpy(this->tabLabels[5], "Quality");
  strcpy(this->tabLabels[6], "Materials");
  strcpy(this->tabLabels[7], "Load/BC");
  strcpy(this->tabLabels[8], ">");
}

//----------------------------------------------------------------------------
vtkKWMimxMainNotebook::~vtkKWMimxMainNotebook()
{
	if(this->Notebook)
		this->Notebook->Delete();
	if(this->SurfaceMenuGroup)
		this->SurfaceMenuGroup->Delete();
	if(this->FEMeshMenuGroup)
		this->FEMeshMenuGroup->Delete();
	if(this->BBMenuGroup)
		this->BBMenuGroup->Delete();
	if(this->ImageMenuGroup)
		this->ImageMenuGroup->Delete();
  if(this->MainFrame)
		this->MainFrame->Delete();
  if(this->MaterialPropertyMenuGroup)
	  this->MaterialPropertyMenuGroup->Delete();
  if(this->BoundaryConditionsMenuGroup)
	  this->BoundaryConditionsMenuGroup->Delete();
	  
	for (int i=0; i<9; i++)
  {
    delete this->tabLabels[i];
  }
  
}
//----------------------------------------------------------------------------
void vtkKWMimxMainNotebook::CreateWidget()
{
	if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}
	this->Superclass::CreateWidget();
	
	//if (!this->MainFrame)	
	//  this->MainFrame = vtkKWFrameWithLabel::New();
	//this->MainFrame->SetParent(this->GetParent());
 // this->MainFrame->Create();
 // this->MainFrame->GetFrame()->SetReliefToGroove();
 // this->MainFrame->SetLabelText("Operations");
 // this->MainFrame->AllowFrameToCollapseOn();
 // this->GetApplication()->Script(
 //   "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
 //   this->MainFrame->GetWidgetName());
    
	this->Notebook = vtkKWUserInterfacePanel::New();
	this->Notebook->SetName("Operations");
	this->Notebook->SetUserInterfaceManager(
		this->GetMimxMainWindow()->GetMainUserInterfaceManager());
	this->Notebook->Create();

  vtkKWUserInterfaceManagerNotebook *testNotebook =  vtkKWUserInterfaceManagerNotebook::SafeDownCast(
                this->GetMimxMainWindow()->GetMainUserInterfaceManager());
  vtkKWNotebook *tmpNote = testNotebook->GetNotebook();
  /* This disables the Pin/Show Menu */
  tmpNote->EnablePageTabContextMenuOff();
    	
	//if (!this->Notebook)	
	//  this->Notebook = vtkKWUserInterfacePanel::New();
	////this->Notebook->SetUserInterfaceManager(this->MimxMainWindow->GetMainUserInterfaceManager());
	//this->Notebook->SetParent( this->GetParent());
	//this->Notebook->Create();
	//this->Notebook->SetWidth( 50 );
	//this->Notebook->SetHeight( 100 );
	//this->Notebook->SetMinimumWidth( 400 );
	//this->Notebook->SetMinimumHeight( 500 ); 

	//this->Notebook->UseFrameWithScrollbarsOn();
	/*
	vtkKWUserInterfacePanel *label_panel = vtkKWUserInterfacePanel::New();
  label_panel->SetName("Display Interface");
  label_panel->SetUserInterfaceManager(this->GetMainUserInterfaceManager());
  label_panel->Create();
	*/
	// 
	//this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -fill y", 
	//	this->Notebook->GetWidgetName());
  
	//this->Notebook->AddPage("<");
	this->Notebook->AddPage("Image");
	
	this->Notebook->AddPage("Surface");
	this->Notebook->AddPage("Block(s)");
	this->Notebook->AddPage("Mesh");
	/* Add this page once Utilities exist*/
	this->Notebook->AddPage("Quality");
	this->Notebook->AddPage("Materials");
	this->Notebook->AddPage("Load/BC");
	//this->Notebook->AddPage(">");
	this->Notebook->Raise();
	this->Notebook->RaisePage("Surface");
	
	//this->Notebook->SetBackgroundColor(1,0,0);
	//this->Notebook->GetFrame(3)->SetWidth(10);
	/*
	tmpNote->HidePage("Mesh");
	tmpNote->HidePage("Quality");
	tmpNote->HidePage("Materials");
	tmpNote->HidePage("Load/BC");
	tmpNote->RaisePage("Surface");
	
	this->AddCallbackCommandObserver(
    tmpNote, vtkKWEvent::NotebookRaisePageEvent);
  */
	if(!this->ImageMenuGroup)	this->ImageMenuGroup = vtkKWMimxImageMenuGroup::New();
	if(!this->SurfaceMenuGroup)	this->SurfaceMenuGroup = vtkKWMimxSurfaceMenuGroup::New();
	if(!this->BBMenuGroup)	this->BBMenuGroup = vtkKWMimxBBMenuGroup::New();
	if(!this->FEMeshMenuGroup)	this->FEMeshMenuGroup = vtkKWMimxFEMeshMenuGroup::New();
	if(!this->QualityMenuGroup)	this->QualityMenuGroup = vtkKWMimxQualityMenuGroup::New();
	if(!this->MaterialPropertyMenuGroup)	
		this->MaterialPropertyMenuGroup = vtkKWMimxMaterialPropertyMenuGroup::New();
	if(!this->BoundaryConditionsMenuGroup)	
		this->BoundaryConditionsMenuGroup = vtkKWMimxBoundaryConditionsMenuGroup::New();

	this->ImageMenuGroup->SetParent(this->Notebook->GetPageWidget("Image"));
	this->ImageMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->ImageMenuGroup->SetApplication(this->GetApplication());
	this->ImageMenuGroup->SetDoUndoTree(this->DoUndoTree);
	this->ImageMenuGroup->Create();
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5", 
		this->ImageMenuGroup->GetWidgetName());


	this->SurfaceMenuGroup->SetParent(this->Notebook->GetPageWidget("Surface"));
	this->SurfaceMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->SurfaceMenuGroup->SetApplication(this->GetApplication());
	this->SurfaceMenuGroup->SetDoUndoTree(this->DoUndoTree);
	this->SurfaceMenuGroup->Create();
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5", 
		this->SurfaceMenuGroup->GetWidgetName());

	this->BBMenuGroup->SetParent(this->Notebook->GetPageWidget("Block(s)"));
	this->BBMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->BBMenuGroup->SetApplication(this->GetApplication());
	this->BBMenuGroup->SetDoUndoTree(this->DoUndoTree);
	this->BBMenuGroup->SetSurfaceMenuGroup(this->SurfaceMenuGroup);

	this->BBMenuGroup->Create();
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5", 
		this->BBMenuGroup->GetWidgetName());
		
	// generate menu items for Mesh Menu
	this->FEMeshMenuGroup->SetParent(this->Notebook->GetPageWidget("Mesh"));
	this->FEMeshMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->FEMeshMenuGroup->SetApplication(this->GetApplication());
	this->FEMeshMenuGroup->SetDoUndoTree(this->DoUndoTree);
	this->FEMeshMenuGroup->Create();
//  this->FEMeshMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->GetApplication()->Script("pack %s -side top -anchor nw  -expand n -padx 2 -pady 5", 
		this->FEMeshMenuGroup->GetWidgetName());
	
	this->QualityMenuGroup->SetParent(this->Notebook->GetPageWidget("Quality"));
	this->QualityMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->QualityMenuGroup->SetApplication(this->GetApplication());
	this->QualityMenuGroup->SetDoUndoTree(this->DoUndoTree);
	this->QualityMenuGroup->Create();
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5", 
		this->QualityMenuGroup->GetWidgetName());

	this->MaterialPropertyMenuGroup->SetParent(this->Notebook->GetPageWidget("Materials"));
	this->MaterialPropertyMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->MaterialPropertyMenuGroup->SetApplication(this->GetApplication());
	this->MaterialPropertyMenuGroup->SetDoUndoTree(this->DoUndoTree);
	this->MaterialPropertyMenuGroup->Create();
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5", 
		this->MaterialPropertyMenuGroup->GetWidgetName());

	this->BoundaryConditionsMenuGroup->SetParent(this->Notebook->GetPageWidget("Load/BC"));
	this->BoundaryConditionsMenuGroup->SetMimxMainWindow(this->GetMimxMainWindow());
	this->BoundaryConditionsMenuGroup->SetApplication(this->GetApplication());
	this->BoundaryConditionsMenuGroup->SetDoUndoTree(this->DoUndoTree);
	this->BoundaryConditionsMenuGroup->Create();
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5", 
		this->BoundaryConditionsMenuGroup->GetWidgetName());

  this->SetLists();
}
//----------------------------------------------------------------------------
void vtkKWMimxMainNotebook::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxMainNotebook::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
	this->SurfaceMenuGroup->SetEnabled(this->GetEnabled());
	this->ImageMenuGroup->SetEnabled(this->GetEnabled());
	this->FEMeshMenuGroup->SetEnabled(this->GetEnabled());
	this->BBMenuGroup->SetEnabled(this->GetEnabled());
	this->QualityMenuGroup->SetEnabled(this->GetEnabled());
	this->MaterialPropertyMenuGroup->SetEnabled(this->GetEnabled());
	this->BoundaryConditionsMenuGroup->SetEnabled(this->GetEnabled());

	vtkKWUserInterfaceManagerNotebook *testNotebook =  vtkKWUserInterfaceManagerNotebook::
		SafeDownCast(this->GetMimxMainWindow()->GetMainUserInterfaceManager());
	vtkKWNotebook *tmpNote = testNotebook->GetNotebook();

	//tmpNote->SetPageEnabled("<", this->GetEnabled());
	//tmpNote->SetPageEnabled(">", this->GetEnabled());
	tmpNote->SetPageEnabled("Image", this->GetEnabled());
	tmpNote->SetPageEnabled("Surface", this->GetEnabled());
	tmpNote->SetPageEnabled("Block(s)", this->GetEnabled());
	tmpNote->SetPageEnabled("Mesh", this->GetEnabled());
	tmpNote->SetPageEnabled("Quality", this->GetEnabled());
	tmpNote->SetPageEnabled("Materials", this->GetEnabled());
	tmpNote->SetPageEnabled("Load/BC", this->GetEnabled());
	this->Notebook->SetEnabled(this->GetEnabled());
}
//----------------------------------------------------------------------------
void vtkKWMimxMainNotebook::SetLists()
{
  // list for surface operations
  this->SurfaceMenuGroup->SetFEMeshList(this->FEMeshMenuGroup->GetFEMeshList());
  this->SurfaceMenuGroup->SetBBoxList(this->BBMenuGroup->GetBBoxList());
  // list for FEMesh operations
  this->FEMeshMenuGroup->SetSurfaceList(this->SurfaceMenuGroup->GetSurfaceList());
  this->FEMeshMenuGroup->SetImageList(this->ImageMenuGroup->GetImageList());
  this->FEMeshMenuGroup->SetBBoxList(this->BBMenuGroup->GetBBoxList());
  // list for Building Block Operations
  this->BBMenuGroup->SetSurfaceList(this->SurfaceMenuGroup->GetSurfaceList());
//  this->BBMenuGroup->SetBBoxList(this->FEMeshMenuGroup->GetBBoxList());
  // list for Quality Operations
  this->QualityMenuGroup->SetFEMeshList(this->FEMeshMenuGroup->GetFEMeshList());

  // list for Property Operations
  this->MaterialPropertyMenuGroup->SetFEMeshList(this->FEMeshMenuGroup->GetFEMeshList());
  this->MaterialPropertyMenuGroup->SetImageList(this->ImageMenuGroup->GetImageList());
//
  this->BoundaryConditionsMenuGroup->SetFEMeshList(this->FEMeshMenuGroup->GetFEMeshList());
  //
  this->QualityMenuGroup->SetBBoxList(this->BBMenuGroup->GetBBoxList());
  this->QualityMenuGroup->SetSurfaceList(this->SurfaceMenuGroup->GetSurfaceList());
}
//----------------------------------------------------------------------------
void vtkKWMimxMainNotebook::ProcessCallbackCommandEvents(vtkObject *caller,
                                                          unsigned long event,
                                                          void *calldata)
{
  const char **cargs = (const char**)calldata;

  vtkKWUserInterfaceManagerNotebook *testNotebook =  vtkKWUserInterfaceManagerNotebook::SafeDownCast(
                this->GetMimxMainWindow()->GetMainUserInterfaceManager());
  vtkKWNotebook *tmpNote = testNotebook->GetNotebook();
  
  bool adjustTabs = false;
  int raisePageIndex = this->startNotebookPage;
  bool raiseTab = false;
  
  // this->GetApplication()->Script("event generate %s <<Escape>>", tmpNote->GetWidgetName());
  
  switch (event)
    {
    case vtkKWEvent::NotebookRaisePageEvent:
    
      if (strcmp(cargs[0], "<") == 0)
      {
        if ( this->startNotebookPage > 1 )
        {
          this->startNotebookPage -= 1;
          adjustTabs = true;
          raiseTab = true;
          raisePageIndex = this->startNotebookPage;
        }
        else
        {
          raiseTab = true;
        }
      }
      else if (strcmp(cargs[0], ">") == 0)
      {
        if ( this->startNotebookPage < 5 )
        {
          this->startNotebookPage += 1;
          adjustTabs = true;
          raiseTab = true;
          raisePageIndex = this->startNotebookPage + this->showNumberOfPages - 1;
        }
        else
        {
          raiseTab = true;
          raisePageIndex = this->startNotebookPage + this->showNumberOfPages - 1;
        }
      }
    } 
  
  if ( adjustTabs )
  {    
    tmpNote->HideAllPages();
    tmpNote->ShowPage("<");
    for (int i=this->startNotebookPage;i<this->startNotebookPage + this->showNumberOfPages;i++)
    {
      tmpNote->ShowPage(this->tabLabels[i]);
    }
    tmpNote->ShowPage(">");
  }
  
  if ( raiseTab )
  {
    tmpNote->RaisePage(this->tabLabels[raisePageIndex]);
  }           
  this->Superclass::ProcessCallbackCommandEvents(caller, event, calldata);
}

//----------------------------------------------------------------------------
void vtkKWMimxMainNotebook::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
