/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewProperties.cxx,v $
Language:  C++
Date:      $Date: 2008/08/09 03:20:34 $
Version:   $Revision: 1.43 $

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

#include "vtkKWMimxViewProperties.h"

#include "vtkLinkedListWrapper.h"
#include "vtkLinkedListWrapperTree.h"

#include "vtkKWApplication.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWIcon.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWPushButton.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkRenderer.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkMimxMeshActor.h"
#include "vtkKWFrameWithScrollbar.h"
#include "vtkKWCheckButton.h"
#include "vtkKWMimxViewPropertiesGroup.h"
#include "vtkKWDialog.h"
#include "vtkMimxImageActor.h"

#include "vtkActor.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxViewProperties);
vtkCxxRevisionMacro(vtkKWMimxViewProperties, "$Revision: 1.43 $");

//----------------------------------------------------------------------------
vtkKWMimxViewProperties::vtkKWMimxViewProperties()
{
  this->MultiColumnList = NULL;
  this->ObjectList = vtkLinkedListWrapper::New();
  this->MimxMainWindow = NULL;
  this->MainFrame = NULL;
  this->DoUndoTree = NULL;
  this->ViewPropertiesGroup = NULL;
  this->ViewButton = NULL;
  this->DisplayButton = NULL;
  this->ViewPropertyDialog = NULL;
  this->DisplayPropertyDialog = NULL;
  this->ObjectId = 0;
}

//----------------------------------------------------------------------------
vtkKWMimxViewProperties::~vtkKWMimxViewProperties()
{
  if(this->MainFrame)
	  this->MainFrame->Delete();
  if(this->ObjectList)
	  this->ObjectList->Delete();
	if (this->ViewButton)
	  this->ViewButton->Delete();
	if (this->DisplayButton)
	  this->DisplayButton->Delete();
	if (this->ViewPropertyDialog)
	  this->ViewPropertyDialog->Delete();
	if (this->DisplayPropertyDialog)
	  this->DisplayPropertyDialog->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::CreateWidget()
{
	if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}
	this->Superclass::CreateWidget();
	if(!this->MainFrame)
	{
		this->MainFrame = vtkKWFrameWithLabel::New();
		//this->MainFrame = vtkKWFrameWithScrollbar::New();
	}
	this->MainFrame->SetParent(this->GetParent());
	this->MainFrame->Create();
	this->MainFrame->GetFrame()->SetReliefToGroove();
	this->MainFrame->SetLabelText("Object Manager");
  this->MainFrame->AllowFrameToCollapseOn();
	this->MainFrame->SetHeight(25);
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    this->MainFrame->GetWidgetName());
  if(!this->MultiColumnList)
  {
    this->MultiColumnList = vtkKWMultiColumnListWithScrollbars::New();
  }
  this->MultiColumnList->SetParent(this->MainFrame->GetFrame());
  this->MultiColumnList->Create();
  this->MultiColumnList->GetWidget()->ClearStripeBackgroundColor();
  this->MultiColumnList->SetHorizontalScrollbarVisibility(0);
  this->MultiColumnList->GetWidget()->SetSelectionBackgroundColor(1.0,1.0,1.0);
  this->MultiColumnList->GetWidget()->SetHeight(5);
  //this->MultiColumnList->SetHorizontalScrollbarVisibility(1);
  //this->MultiColumnList->SetVerticalScrollbarVisibility(1);
  //this->MultiColumnList->GetWidget()->SetHeight(5);
 // this->MultiColumnList->SetWidth(10);
//  this->MultiColumnList->GetWidget()->GetMovableColumnsOn();
//  this->MultiColumnList->SetPotentialCellColorsChangedCommand(
//    this->MultiColumnList, "ScheduleRefreshColorsOfAllCellsWithWindowCommand");
  int col_index;

  // Add the columns 
  
  col_index = this->MultiColumnList->GetWidget()->AddColumn(NULL);
  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  this->MultiColumnList->GetWidget()->SetColumnLabelImageToPredefinedIcon(
	  col_index, vtkKWIcon::IconEye);
	this->MultiColumnList->GetWidget()->SetColumnWidth( col_index, 2);
	
	col_index = this->MultiColumnList->GetWidget()->AddColumn("Name");
  this->MultiColumnList->GetWidget()->SetColumnWidth( col_index, 30);
  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  this->MultiColumnList->GetWidget()->SetColumnSortedCommand(this, "SortedCommandCallback");
  
  col_index = this->MultiColumnList->GetWidget()->AddColumn("Type");
  this->MultiColumnList->GetWidget()->SetColumnWidth( col_index, 10);
  this->MultiColumnList->GetWidget()->SetConfigurationOption("-font", "arial 8 bold");
  //this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  //this->MultiColumnList->GetWidget()->SetColumnSortedCommand(this, "SortedCommandCallback");
  
  col_index = this->MultiColumnList->GetWidget()->AddColumn("");
  this->MultiColumnList->GetWidget()->SetColumnWidth( col_index, 0);
  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  
  /***VAM***/
  // Add Column to store the type and Index - make index invisible?
  
 //this->MultiColumnList->InsertCellText(0, 0, "");
 // this->MultiColumnList->InsertCellTextAsInt(0, 1, 0);
 // this->MultiColumnList->SetCellWindowCommandToCheckButton(0, 1);
 // this->MultiColumnList->GetCellWindowAsCheckButton(0,1)->SetEnabled(0);
  this->MultiColumnList->GetWidget()->SetSortArrowVisibility(0);
  this->MultiColumnList->GetWidget()->ColumnSeparatorsVisibilityOff();
  this->MultiColumnList->GetWidget()->SetHeight( 4 );
  
//  this->MultiColumnList->SetSelectioModeToSingle();
//  this->MultiColumnList->RowSeparatorsVisibilityOff();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
    this->MultiColumnList->GetWidgetName());

/*  
  if(!this->ViewButton)
  {
    this->ViewButton = vtkKWPushButton::New();
  }
  this->ViewButton->SetParent(this->MainFrame->GetFrame());
  this->ViewButton->Create();
  this->ViewButton->SetText("View");
  this->ViewButton->SetCommand(this, "DisplayPropertyCallback");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand y -padx 20 -pady 6", 
	  this->ViewButton->GetWidgetName());

  if(!this->DisplayButton)
  {
    this->DisplayButton = vtkKWPushButton::New();
  }
  this->DisplayButton->SetParent(this->MainFrame->GetFrame());
  this->DisplayButton->Create();
  this->DisplayButton->SetText("Display");
  this->DisplayButton->SetCommand(this, "ViewPropertyCallback");
  this->GetApplication()->Script(
    "pack %s -side right -anchor ne -expand y -padx 20 -pady 6", 
    this->DisplayButton->GetWidgetName());
*/    
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxViewProperties::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::VisibilityCallback(const char *objectId, int flag)
{
  for (int i=0; i<this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
  {
    const char* cellText = this->MultiColumnList->GetWidget()->GetCellText(i, 3);
    for (int j=0;j<this->ObjectList->GetNumberOfItems(); j++)
    {
		const char *actorId = this->ObjectList->GetItem(j)->GetUniqueId();
      if (strcmp(cellText, actorId) == 0 && strcmp(objectId, actorId) == 0)
      {
        //if (this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->GetSelectedState())
        //{
		  if(flag)
		  {
          if(this->ObjectList->GetItem(j)->GetDataType() == ACTOR_FE_MESH)
          {
            vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(j));
            meshActor->ShowMesh();
			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,1);
			//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			return;
          }
          else if (this->ObjectList->GetItem(j)->GetDataType() == ACTOR_IMAGE)
          {
            vtkMimxImageActor::SafeDownCast(this->ObjectList->GetItem(j))->DisplayActor(1);
			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,1);
			//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			return;
          }
          else
          {
            this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
              this->ObjectList->GetItem(j)->GetActor());
			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,1);
			//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			return;
          }
 //         this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,1);
        }
      	else
      	{
    		  //std::cout << "Type: " << this->ObjectList->GetItem(j)->GetDataType() << std::endl;
    		  if(this->ObjectList->GetItem(j)->GetDataType() == ACTOR_FE_MESH)
          {
            vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(j));
            meshActor->HideMesh();
			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,0);
			//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			return;
          }
          else if (this->ObjectList->GetItem(j)->GetDataType() == ACTOR_IMAGE)
          {
            vtkMimxImageActor::SafeDownCast(this->ObjectList->GetItem(j))->DisplayActor(0);
			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,0);
			//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			return;
          }
          else
          {
            this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
      			  this->ObjectList->GetItem(j)->GetActor());
			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,0);
			//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			return;
    	  }
//    			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,0);
      	}
      }
    }
  }
  
  
  
  /*
  
  for (int i=0; i<this->ObjectList->GetNumberOfItems(); i++)
  {
    if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->GetSelectedState())
    {
      if(this->ObjectList->GetItem(i)->GetDataType() == ACTOR_FE_MESH)
      {
        vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(i));
        meshActor->ShowMesh();
      }
      else
      {
        this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
          this->ObjectList->GetItem(i)->GetActor());
      }
      this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,1);
    }
  	else
  	{
		  if(this->ObjectList->GetItem(i)->GetDataType() == ACTOR_FE_MESH)
      {
        vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(i));
        meshActor->HideMesh();
      }
      else
      {
        this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
  			  this->ObjectList->GetItem(i)->GetActor());
			}
			this->MultiColumnList->GetWidget()->SetCellTextAsInt(i,0,0);
  	}
  }
  */  
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::AddObjectList(vtkMimxActorBase *actor)
{
	char UniqueObjectId[64];
	sprintf(UniqueObjectId, "Object-%d", this->ObjectId);
	this->ObjectId++;
	actor->SetUniqueId( UniqueObjectId );
	
	this->ObjectList->AppendItem(actor);
	
	/* Add the Object to the Display MultiColumnList */
	int rowIndex = this->ObjectList->GetNumberOfItems()-1;
	
	this->MultiColumnList->GetWidget()->InsertCellTextAsInt(rowIndex, 0, 1);
  this->MultiColumnList->GetWidget()->SetCellWindowCommandToCheckButton( rowIndex, 0);
  
  char commandName[256];
  sprintf(commandName, "VisibilityCallback %s", UniqueObjectId);
  
	this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(rowIndex,0)->SetCommand(this, commandName);	
  this->MultiColumnList->GetWidget()->InsertCellText(rowIndex, 1, this->ObjectList->GetItem(rowIndex)->GetFileName());
  //this->MultiColumnList->GetWidget()->SetCellConfigurationOptionAsText(rowIndex, 1, "-font", "arial 8 bold");
		
  switch (actor->GetDataType())
  {
    case ACTOR_FE_MESH:
      this->MultiColumnList->GetWidget()->InsertCellText(rowIndex, 2, "Mesh");
      this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
      break;
    case ACTOR_POLYDATA_SURFACE:
      this->MultiColumnList->GetWidget()->InsertCellText(rowIndex, 2, "Surface");
      this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
      break;
    case ACTOR_BUILDING_BLOCK:
      this->MultiColumnList->GetWidget()->InsertCellText(rowIndex, 2, "Block");
      this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
      break;
    case ACTOR_IMAGE:
      this->MultiColumnList->GetWidget()->InsertCellText(rowIndex, 2, "Image");
      vtkMimxImageActor::SafeDownCast(actor)->DisplayActor(1);
      break;
  }
  this->MultiColumnList->GetWidget()->InsertCellText(rowIndex, 3, UniqueObjectId);
  
  sprintf(commandName, "CreateNameCellCallback %s %d", UniqueObjectId, actor->GetDataType()); 
  this->MultiColumnList->GetWidget()->SetCellWindowCommand(rowIndex, 1, this, commandName);
  
  //this->MultiColumnList->GetWidget()->InsertCellText(rowIndex, 1, "");
  
  //InsertCellText(rowIndex, 1, this->ObjectList->GetItem(rowIndex)->GetFileName());

	//this->MultiColumnList->GetWidget()->SetSelectionChangedCommand(NULL, "SetViewProperties");
  
	//this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();

//	this->UpdateVisibility();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::UpdateVisibility()
{
	for (int i=0; i<this->ObjectList->GetNumberOfItems(); i++)
	{
		if (this->ObjectList->GetItem(i)->GetDataType() == ACTOR_FE_MESH)
    {
      vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(i));
      bool isVisible = meshActor->GetMeshVisibility();
      if (isVisible)
      {
        this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->SetSelectedState(1);
      }
      else
      {
        this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->SetSelectedState(0);
      }
    }
    else
    {
  		if(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
  			this->ObjectList->GetItem(i)->GetActor()))
  			this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->
  			SetSelectedState(1);
  		else
  			this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->
  			SetSelectedState(0);
		}
	}
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::UpdateVisibilityList()
{
	for (int i=0; i<this->ObjectList->GetNumberOfItems(); i++)
	{
		if (this->ObjectList->GetItem(i)->GetDataType() == ACTOR_FE_MESH)
		{
			if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->GetSelectedState())
			{
				vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(i))->ShowMesh();
			}
			else
			{
				vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(i))->HideMesh();
			}
		}
		else
		{
			if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->GetSelectedState())
			{
				this->MimxMainWindow->GetRenderWidget()->AddViewProp(this->ObjectList->GetItem(i)->GetActor());
			}
			else
			{
				this->MimxMainWindow->GetRenderWidget()->RemoveViewProp(this->ObjectList->GetItem(i)->GetActor());
			}
		}
	}
	this->MimxMainWindow->GetRenderWidget()->Render();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::DeleteObjectList(int DataType, int Position)
{
	// match the position from one list to the other
	int i, currpos;
	int poscount = 0;

	for(i=0; i< this->ObjectList->GetNumberOfItems(); i++)
	{
		if(this->ObjectList->GetItem(i)->GetDataType() == DataType)
		{
			if(poscount == Position)
			{
				currpos = i;
				break;
			}
			poscount ++;
		}
	}
	this->MultiColumnList->GetWidget()->DeleteRow(currpos);
	if(DataType == 6)
	{
		vtkMimxMeshActor::SafeDownCast(
			this->ObjectList->GetItem(currpos))->HideMesh();
		vtkMimxMeshActor::SafeDownCast(
			this->ObjectList->GetItem(currpos))->HideAllElementSetLegends();
		vtkMimxMeshActor::SafeDownCast(
			this->ObjectList->GetItem(currpos))->HideAllElementSets();
	}
	else
	{
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			this->ObjectList->GetItem(currpos)->GetActor());
	}
	this->ObjectList->RemoveItem(currpos);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::DeleteObjectList(const char *name)
{
	// match the position from one list to the other
	int i;	
	for (i=0; i<this->ObjectList->GetNumberOfItems(); i++)
	{
		const char *name1 = this->MultiColumnList->GetWidget()->GetCellText(i,1);
		if(!strcmp(name, this->MultiColumnList->GetWidget()->GetCellText(i,1)))
		{
			break;
		}
	}
	this->MultiColumnList->GetWidget()->DeleteRow(i);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		this->ObjectList->GetItem(i)->GetActor());
	this->ObjectList->RemoveItem(i);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::ViewPropertyCallback(const char *objectId, const char *itemText)
{

	if (!this->ViewPropertyDialog)
	{
	  this->ViewPropertyDialog = vtkKWMimxViewPropertiesGroup::New();
	}
	  this->ViewPropertyDialog->SetMimxMainWindow(this->GetMimxMainWindow());
	  this->ViewPropertyDialog->SetMultiColumnList(this->MultiColumnList);
	  this->ViewPropertyDialog->SetObjectList(
		  this->GetMimxMainWindow()->GetViewProperties()->GetObjectList());
	  this->ViewPropertyDialog->SetApplication(this->GetApplication());
	  this->MultiColumnList->GetWidget()->SetSelectionChangedCommand(this->ViewPropertyDialog, "SetViewProperties");
	  this->ViewPropertyDialog->SetItemId( objectId );
	  this->ViewPropertyDialog->SetItemName(itemText);
	  this->ViewPropertyDialog->Create();
	  this->ViewPropertyDialog->SetViewProperties();  
	  this->ViewPropertyDialog->Display();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::DisplayPropertyCallback()
{
	if (!this->DisplayPropertyDialog)
	{
	  this->DisplayPropertyDialog = vtkKWMimxDisplayPropertiesGroup::New();
	  this->DisplayPropertyDialog->SetMimxMainWindow(this->GetMimxMainWindow());
	  this->DisplayPropertyDialog->SetApplication(this->GetApplication());
	  this->DisplayPropertyDialog->Create();
	}
	this->DisplayPropertyDialog->Display();
	
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::CreateNameCellCallback(const char *objectId, int actorType, const char *tableWidgetName, int row, int col, const char *widgetName)
{
  vtkKWPushButton *cellButton = vtkKWPushButton::New();
  cellButton->SetWidgetName(widgetName);
  cellButton->SetApplication(this->GetApplication());
  cellButton->Create();
  cellButton->SetFont("arial 8 bold");
  cellButton->SetReliefToFlat();
  cellButton->SetBorderWidth(0);
  cellButton->SetText(this->MultiColumnList->GetWidget()->GetCellText(row, col));
  /* The next line caused a sorting issue */
  //this->MultiColumnList->GetWidget()->InsertCellText(row, col, "");

  if (actorType != ACTOR_IMAGE)
  {
    char callbackCommand[256];
    sprintf(callbackCommand, "ViewPropertyCallback %s %s", objectId, this->MultiColumnList->GetWidget()->GetCellText(row, col));
    cellButton->SetCommand(this, callbackCommand);
  }
//  this->MultiColumnList->GetWidget()->RefreshColorsOfCellWithWindowCommand(row, col);
//  this->MultiColumnList->GetWidget()->RefreshAllCellsWithWindowCommand ();
//  this->MultiColumnList->GetWidget()->ScheduleRefreshColorsOfAllCellsWithWindowCommand();
  /*
  app->Script(
    "proc CreateCompletionCellCallback {tw row col w} { "
    "  frame $w -bg #882233 -relief groove -bd 2 -height 10 -width [expr [%s GetCellTextAsDouble $row $col] * 0.01 * 70] ;"
    "  %s AddBindingsToWidgetName $w "
    "}", mcl1->GetTclName(), mcl1->GetTclName());
  */
}    
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::SortedCommandCallback( )
{
  for (int i=0; i<this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
  {
    const char *id = this->MultiColumnList->GetWidget()->GetCellText(i, 3);
    char commandText[64];
    sprintf(commandText, "VisibilityCallback %s", id);
    this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,0)->SetCommand(this, commandText);
  } 
}
//----------------------------------------------------------------------------
void vtkKWMimxViewProperties::EnableViewPropertyList( int mode )
{
  this->MultiColumnList->SetEnabled( mode );
	this->MultiColumnList->UpdateEnableState(); 
}
//----------------------------------------------------------------------------
