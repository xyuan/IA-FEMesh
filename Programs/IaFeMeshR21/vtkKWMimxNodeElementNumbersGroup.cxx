/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxNodeElementNumbersGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/06/17 00:30:32 $
Version:   $Revision: 1.6 $

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

#include "vtkKWMimxNodeElementNumbersGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"

#include "vtkLinkedListWrapper.h"
#include "vtkMimxApplyNodeElementNumbers.h"

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkRenderer.h"

#include "vtkKWApplication.h"
#include "vtkKWEntry.h"
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
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWCheckButton.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


#include "vtkUnstructuredGridWriter.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxNodeElementNumbersGroup);
vtkCxxRevisionMacro(vtkKWMimxNodeElementNumbersGroup, "$Revision: 1.6 $");

//----------------------------------------------------------------------------
vtkKWMimxNodeElementNumbersGroup::vtkKWMimxNodeElementNumbersGroup()
{
  this->ObjectListComboBox = NULL;
  this->NodeNumberEntry = NULL;
  this->ElementNumberEntry = NULL;
  this->NodeSetNameEntry = NULL;
  this->ElementSetNameEntry = NULL;
  this->NodeNumberCheckButton = NULL;
  this->ElementNumberCheckButton = NULL;
  this->IdLabel = NULL;
	this->NumberLabel = NULL;
	this->NodeLabel = NULL;
	this->ElementLabel = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxNodeElementNumbersGroup::~vtkKWMimxNodeElementNumbersGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->NodeNumberEntry)
	  this->NodeNumberEntry->Delete();
  if(this->ElementNumberEntry)
	  this->ElementNumberEntry->Delete();
  if(this->NodeSetNameEntry)
	  this->NodeSetNameEntry->Delete();
  if(this->ElementSetNameEntry)
	  this->ElementSetNameEntry->Delete();
  if(this->NodeNumberCheckButton)
	  this->NodeNumberCheckButton->Delete();
  if(this->ElementNumberCheckButton)
	  this->NodeNumberCheckButton->Delete();
	if (this->IdLabel)
	  this->IdLabel;
	if (this->NumberLabel)
	  this->NumberLabel;
	if (this->NodeLabel)
	  this->NodeLabel;
	if (this->ElementLabel)
	  this->ElementLabel;
	
 }
//----------------------------------------------------------------------------
void vtkKWMimxNodeElementNumbersGroup::CreateWidget()
{
  if(this->IsCreated())
  {
  vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();
  if(!this->ObjectListComboBox)	
  {
     this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();
  }
  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Node & Elements");

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    this->MainFrame->GetWidgetName());

  //ObjectListComboBox->SetParent(this->MainFrame);
  //ObjectListComboBox->Create();
  //ObjectListComboBox->SetLabelText("Mesh : ");
  //ObjectListComboBox->SetLabelWidth(20);
  //ObjectListComboBox->GetWidget()->ReadOnlyOn();
  //ObjectListComboBox->GetWidget()->SetCommand(this, "SelectionChangedCallback");
  //this->GetApplication()->Script(
  //  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
  //  ObjectListComboBox->GetWidgetName());

  //// check button for node numbers
  //this->NodeNumberCheckButton = vtkKWCheckButtonWithLabel::New();
  //this->NodeNumberCheckButton->SetParent(this->MainFrame);
  //this->NodeNumberCheckButton->SetLabelText("Set Node Numbers : ");
  ////this->NodeNumberCheckButton->SetLabelWidth(20);
  //this->NodeNumberCheckButton->SetLabelPositionToLeft();
  //this->NodeNumberCheckButton->Create();
  //this->NodeNumberCheckButton->GetWidget()->SetCommand(this, "ApplyNodeNumbersCallback");
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand n -padx 6 -pady 6", 
	 // this->NodeNumberCheckButton->GetWidgetName());

  if(!this->NumberLabel)
	  this->NumberLabel = vtkKWLabel::New();
	this->NumberLabel->SetParent(this->MainFrame);
  this->NumberLabel->Create();
  this->NumberLabel->SetText("#");
  this->GetApplication()->Script(
	  "grid %s -row 0 -column 1 -sticky nswe -padx 2 -pady 2",
	  this->NumberLabel->GetWidgetName());
	  
	if(!this->IdLabel)
	  this->IdLabel = vtkKWLabel::New();
	this->IdLabel->SetParent(this->MainFrame);
  this->IdLabel->Create();
  this->IdLabel->SetText("Label");
  this->GetApplication()->Script(
	  "grid %s -row 0 -column 2 -sticky nswe -padx 2 -pady 2",
	  this->IdLabel->GetWidgetName());
  
  if(!this->NodeLabel)
	  this->NodeLabel = vtkKWLabel::New();
	this->NodeLabel->SetParent(this->MainFrame);
  this->NodeLabel->Create();
  this->NodeLabel->SetText("Node");
  this->GetApplication()->Script(
	  "grid %s -row 1 -column 0 -sticky ne -padx 2 -pady 2",
	  this->NodeLabel->GetWidgetName());
	
	if(!this->ElementLabel)
	  this->ElementLabel = vtkKWLabel::New();
	this->ElementLabel->SetParent(this->MainFrame);
  this->ElementLabel->Create();
  this->ElementLabel->SetText("Element");
  this->GetApplication()->Script(
	  "grid %s -row 2 -column 0 -sticky ne -padx 2 -pady 2",
	  this->ElementLabel->GetWidgetName());
	  
	  	  
  

  if(!this->NodeNumberEntry)
	  this->NodeNumberEntry = vtkKWEntry::New();
  this->NodeNumberEntry->SetParent(this->MainFrame);
  this->NodeNumberEntry->Create();
  this->NodeNumberEntry->SetWidth(8); 
  //this->NodeNumberEntry->SetLabelText("Starting Node Number : ");
  //this->NodeNumberEntry->SetLabelWidth(20);
  this->NodeNumberEntry->SetValueAsInt( 1 );
  this->NodeNumberEntry->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "grid %s -row 1 -column 1 -sticky ne -padx 2 -pady 2", 
	  this->NodeNumberEntry->GetWidgetName());

  if(!this->NodeSetNameEntry)
	  this->NodeSetNameEntry = vtkKWEntry::New();
  this->NodeSetNameEntry->SetParent(this->MainFrame);
  this->NodeSetNameEntry->Create();
  //this->NodeSetNameEntry->SetWidth(16);
  //this->NodeSetNameEntry->SetLabelText("Node Set Name : ");
  //this->NodeSetNameEntry->SetLabelWidth(20);
  this->GetApplication()->Script(
	  "grid %s -row 1 -column 2 -sticky nswe -padx 2 -pady 2", 
	  this->NodeSetNameEntry->GetWidgetName());
	  
  // check button for node numbers
  //this->ElementNumberCheckButton = vtkKWCheckButtonWithLabel::New();
  //this->ElementNumberCheckButton->SetParent(this->MainFrame);
  //this->ElementNumberCheckButton->SetLabelText("Set Element Numbers : ");
  //this->ElementNumberCheckButton->SetLabelPositionToLeft();
  ////this->ElementNumberCheckButton->SetLabelWidth(20);
  //this->ElementNumberCheckButton->Create();
  //this->ElementNumberCheckButton->GetWidget()->SetCommand(this, "ApplyElementNumbersCallback");
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand n -padx 6 -pady 6 -fill x", 
	 // this->ElementNumberCheckButton->GetWidgetName());

  

  if(!this->ElementNumberEntry)
	  this->ElementNumberEntry = vtkKWEntry::New();

  this->ElementNumberEntry->SetParent(this->MainFrame);
  this->ElementNumberEntry->Create();
  this->ElementNumberEntry->SetWidth(8);
  //this->ElementNumberEntry->SetLabelText("Starting Element Number : ");
  //this->ElementNumberEntry->SetLabelWidth(20);
  this->ElementNumberEntry->SetValueAsInt(1);
  this->ElementNumberEntry->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "grid %s -row 2 -column 1 -sticky ne -padx 2 -pady 2", 
	  this->ElementNumberEntry->GetWidgetName());

  if(!this->ElementSetNameEntry)
	  this->ElementSetNameEntry = vtkKWEntry::New();

  this->ElementSetNameEntry->SetParent(this->MainFrame);
  this->ElementSetNameEntry->Create();
  //this->ElementSetNameEntry->SetWidth(16);
  //this->ElementSetNameEntry->SetLabelText("Element Set Name : ");
  //this->ElementSetNameEntry->SetLabelWidth(20);
  this->GetApplication()->Script(
	  "grid %s -row 2 -column 2 -sticky nswe -padx 2 -pady 2", 
	  this->ElementSetNameEntry->GetWidgetName());
	  
  //this->ApplyButton->SetParent(this->MainFrame);
  //this->ApplyButton->Create();
  //this->ApplyButton->SetText("Apply");
  //this->ApplyButton->SetCommand(this, "EditNodeElementNumbersApplyCallback");
  //this->GetApplication()->Script(
	 // "pack %s -side left -anchor nw -expand y -padx 20 -pady 6", 
	 // this->ApplyButton->GetWidgetName());
/*
  this->DoneButton->SetParent(this->MainFrame);
  this->DoneButton->Create();
  this->DoneButton->SetText("Done");
  this->DoneButton->SetCommand(this, "EditNodeElementNumbersDoneCallback");
  this->GetApplication()->Script(
    "pack %s -side left -anchor nw -expand y -padx 20 -pady 6", 
    this->DoneButton->GetWidgetName());
*/
  //this->CancelButton->SetParent(this->MainFrame);
  //this->CancelButton->Create();
  //this->CancelButton->SetText("Cancel");
  //this->CancelButton->SetCommand(this, "EditNodeElementNumbersCancelCallback");
  //this->GetApplication()->Script(
  //  "pack %s -side right -anchor ne -expand y -padx 20 -pady 6", 
  //  this->CancelButton->GetWidgetName());

  //this->ElementNumberEntry->SetEnabled(0);
  //this->ElementSetNameEntry->SetEnabled(0);
  //this->NodeNumberEntry->SetEnabled(0);
}
//----------------------------------------------------------------------------
void vtkKWMimxNodeElementNumbersGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxNodeElementNumbersGroup::UpdateEnableState()
{
	//this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
//int vtkKWMimxNodeElementNumbersGroup::EditNodeElementNumbersApplyCallback()
//{
//	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
//	callback->SetState(0);
//  if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
//  {
//	callback->ErrorMessage("FE mesh selection required");
//	return 0;
//  }
//  int nodenum, elenum;
//  nodenum = this->NodeNumberEntry->GetWidget()->GetValueAsInt();
//  elenum = this->ElementNumberEntry->GetWidget()->GetValueAsInt();
//  const char *elementset = this->ElementSetNameEntry->GetWidget()->GetValue();
//  if(nodenum < 1 )
//  {
//	  callback->ErrorMessage("Starting node number should be > 0");
//	  return 0;
//  }
//  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
//	const char *name = combobox->GetValue();
//
//	int num = combobox->GetValueIndex(name);
//	if(num < 0 || num > combobox->GetNumberOfValues()-1)
//	{
//		callback->ErrorMessage("Choose valid FE Mesh");
//		combobox->SetValue("");
//		return 0;
//	}
//
//  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
//	  this->FEMeshList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
//  vtkMimxApplyNodeElementNumbers *apply = new vtkMimxApplyNodeElementNumbers;
//  apply->SetUnstructuredGrid(ugrid);
//  if(this->NodeNumberCheckButton->GetWidget()->GetSelectedState())
//  {
//	  apply->SetStartingNodeNumber(nodenum);
//	  apply->ApplyNodeNumbers();
//  }
//  if(this->ElementNumberCheckButton->GetWidget()->GetSelectedState())
//  {
//	  if(elenum < 1 )
//	  {
//		  callback->ErrorMessage("Starting element number should be > 0");
//		  return 0;
//	  }
//	  apply->SetElementSetName(elementset);
//	  apply->SetStartingElementNumber(elenum);
//	  apply->ApplyElementNumbers();
//  }
//  delete apply;
//  
//  this->GetMimxMainWindow()->SetStatusText("Edited Mesh Node/Element Numbers");
//  
//  return 1;
//}
//----------------------------------------------------------------------------
void vtkKWMimxNodeElementNumbersGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
//void vtkKWMimxNodeElementNumbersGroup::EditNodeElementNumbersCancelCallback()
//{
////  this->MainFrame->UnpackChildren();
//  this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName());
//  this->MenuGroup->SetMenuButtonsEnabled(1);
//    this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
//}
//------------------------------------------------------------------------------
//void vtkKWMimxNodeElementNumbersGroup::UpdateObjectLists()
//{
//	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
//	
//	int defaultItem = -1;
//	for (int i = 0; i < this->FEMeshList->GetNumberOfItems(); i++)
//	{
//		ObjectListComboBox->GetWidget()->AddValue(
//			this->FEMeshList->GetItem(i)->GetFileName());
//			
//	  int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
//			this->FEMeshList->GetItem(i)->GetActor());
//		if ( (defaultItem == -1) && ( viewedItem ) )
//		{
//		  defaultItem = i;
//		}
//	}
//	
//	if (defaultItem != -1)
//  {
//    ObjectListComboBox->GetWidget()->SetValue(
//          this->FEMeshList->GetItem(defaultItem)->GetFileName());
//  }
//}
//--------------------------------------------------------------------------------
//void vtkKWMimxNodeElementNumbersGroup::SelectionChangedCallback(const char *dummy)
//{
//	  if(strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
//	  {
//		  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
//		  const char *name = combobox->GetValue();
//		  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
//			  this->FEMeshList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
//		  //if(ugrid->GetCellData()->GetNumberOfArrays() <= 2)
//		  //{
//		  if(ugrid->GetPointData()->GetArray("Node_Numbers"))
//		  {
//			  int nodenum = vtkIntArray::SafeDownCast(
//				  ugrid->GetPointData()->GetArray("Node_Numbers"))->GetValue(0);
//			  this->NodeNumberEntry->GetWidget()->SetValueAsInt(nodenum);
//		  }
//		  else
//		  {
//			  this->NodeNumberEntry->GetWidget()->SetValue("");
//		  }
//		  if(ugrid->GetCellData()->GetArray("Element_Numbers"))
//		  {
//			  int elementnum = vtkIntArray::SafeDownCast(
//				  ugrid->GetCellData()->GetArray("Element_Numbers"))->GetValue(0);
//			  this->ElementNumberEntry->GetWidget()->SetValueAsInt(elementnum);
//			  for (int i =0; i < ugrid->GetCellData()->GetNumberOfArrays(); i++)
//			  {
//				  if(strcmp(ugrid->GetCellData()->GetArray(i)->GetName(),"Element_Numbers"))
//				  {
//					  this->ElementSetNameEntry->GetWidget()->SetValue(
//						  ugrid->GetCellData()->GetArray(i)->GetName());
//				  }
//			  }
//		  }
//		  else
//		  {
//			this->ElementNumberEntry->GetWidget()->SetValue("");
//			this->ElementSetNameEntry->GetWidget()->SetValue("");
//		  }
//	  }
//}
//---------------------------------------------------------------------------------------------
//void vtkKWMimxNodeElementNumbersGroup::EditNodeElementNumbersDoneCallback()
//{
//	if(this->EditNodeElementNumbersApplyCallback())
//		this->EditNodeElementNumbersCancelCallback();
//}
//---------------------------------------------------------------------------------
//void vtkKWMimxNodeElementNumbersGroup::ApplyNodeNumbersCallback(int State)
//{
//	this->NodeNumberEntry->SetEnabled(State);
//}
//----------------------------------------------------------------------------------------------
//void vtkKWMimxNodeElementNumbersGroup::ApplyElementNumbersCallback(int State)
//{
//	this->ElementSetNameEntry->SetEnabled(State);
//	this->ElementNumberEntry->SetEnabled(State);
//}
//----------------------------------------------------------------------------------------------
