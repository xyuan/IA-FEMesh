/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxEditBBMeshSeedGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/08/14 05:01:52 $
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

#include "vtkKWMimxEditBBMeshSeedGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxPlaceLocalAxesWidget.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxColorCodeMeshSeedActor.h"
#include "vtkScalarBarActor.h"
#include "vtkKWMimxApplication.h"

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkMimxBoundingBoxSource.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkPolyData.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMimxSplitUnstructuredHexahedronGridCell.h"
#include "vtkMimxAddUnstructuredHexahedronGridCell.h"
#include "vtkMimxDeleteUnstructuredHexahedronGridCell.h"
#include "vtkIdList.h"

#include "vtkKWApplication.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWIcon.h"
#include "vtkKWInternationalization.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWNotebook.h"
#include "vtkKWOptions.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkLinkedListWrapper.h"
#include "vtkMath.h"
#include "vtkIntArray.h"
#include "vtkRenderer.h"

#include "vtkObjectFactory.h"
#include "vtkKWPushButton.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWComboBox.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButton.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkProperty.h"

#include "Resources/mimxColorSeeds.h"


#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxEditBBMeshSeedGroup);
vtkCxxRevisionMacro(vtkKWMimxEditBBMeshSeedGroup, "$Revision: 1.39 $");

//----------------------------------------------------------------------------
vtkKWMimxEditBBMeshSeedGroup::vtkKWMimxEditBBMeshSeedGroup()
{
	this->ComponentFrame = NULL;
//  this->BBoxList = vtkLinkedListWrapper::New();
  this->MimxMainWindow = NULL;
  this->ObjectListComboBox = NULL;
  this->VtkInteractionButton = vtkKWRadioButton::New();
  this->LocalAxesWidget = NULL;
  this->EditButton = NULL;
  this->ButtonFrame = NULL;
  this->XMeshSeed = NULL;
  this->YMeshSeed = NULL;
  this->ZMeshSeed = NULL;
  this->ShowMeshSeedCheckButton = NULL;
  this->EditTypeChoiceFrame = NULL;
  this->AxisSelection = vtkKWRadioButtonSet::New();
  this->UserDefinedElementLength = vtkKWEntryWithLabel::New();
  this->CellSelectionFrame = NULL;
  this->AvElementLength = vtkKWEntryWithLabel::New();
  //this->MeshSeedTypeFrame = vtkKWFrameWithLabel::New();
  this->MeshSeedTypeButton = NULL;
  this->EditTypeSelection = NULL;
  this->ColorCodeMeshSeedActor = NULL;
  strcpy(this->PreviousSelection, "");
  this->AverageEdgeLength[0] = 1.0;
  this->AverageEdgeLength[1] = 1.0;
  this->AverageEdgeLength[2] = 1.0;
}

//----------------------------------------------------------------------------
vtkKWMimxEditBBMeshSeedGroup::~vtkKWMimxEditBBMeshSeedGroup()
{
  if(this->ComponentFrame)  
    this->ComponentFrame->Delete();
  if(this->ObjectListComboBox)  
    this->ObjectListComboBox->Delete();
  if(this->LocalAxesWidget)
	  this->LocalAxesWidget->Delete();
  if(this->EditButton)
	  this->EditButton->Delete();
  if (this->EditTypeChoiceFrame)
    this->EditTypeChoiceFrame->Delete();
  if (this->AxisSelection)
    this->AxisSelection->Delete();
  if (this->UserDefinedElementLength)
    this->UserDefinedElementLength->Delete();
  if (this->CellSelectionFrame)
    this->CellSelectionFrame->Delete();
  if(this->XMeshSeed)
	  this->XMeshSeed->Delete();
	if (this->YMeshSeed)
	  this->YMeshSeed->Delete();
	if (this->ZMeshSeed)
	  this->ZMeshSeed->Delete();
	if (this->AvElementLength)
	  this->AvElementLength->Delete();
	//if (this->MeshSeedTypeFrame)
  	//this->MeshSeedTypeFrame->Delete();
	if (this->MeshSeedTypeButton)
  	this->MeshSeedTypeButton->Delete();
	if (this->EditTypeSelection)
  	this->EditTypeSelection->Delete();
	if (this->ShowMeshSeedCheckButton)
	  this->ShowMeshSeedCheckButton->Delete();
	if (this->ColorCodeMeshSeedActor)
		this->ColorCodeMeshSeedActor->Delete();
  if (this->ButtonFrame)
    this->ButtonFrame->Delete();
}
//--------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::SelectionChangedCallback(const char* dummy)
{
	if ( (strcmp(dummy,"") != 0) && (strcmp(dummy, this->PreviousSelection) != 0) )
	{
	  
	  /*
	  if(this->EditButton->GetSelectedState())
	  {
		  this->EditButton->SetSelectedState(1);
		  this->EditBBMeshSeedPickCellCallback();
	  }
	  */
	  this->EditBBMeshSeedPickCellCallback();
	  if ( this->ShowMeshSeedCheckButton->GetWidget()->GetSelectedState() )
	  {
	  //  /* Return the original Actor */
  	//	if ( strcmp(this->PreviousSelection,"") )
  	//	{
   // 		this->ReturnUnstructuredGridActor(this->PreviousSelection);
			//}
	  //  this->ShowMeshSeedCallback(1);
		this->ShowMeshSeedCheckButton->GetWidget()->SetSelectedState(0);
	  }
	  this->ShowMeshSeedCheckButton->GetWidget()->SetSelectedState(1);
	  strcpy(this->PreviousSelection, dummy);
	}
}
//--------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::CreateWidget()
{
  if(this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();

  if (!this->MainFrame)	
	  this->MainFrame = vtkKWFrameWithLabel::New();
  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Edit Mesh Seed");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 0 -fill x", 
    this->MainFrame->GetWidgetName());
  
  if (!this->ComponentFrame)	
     this->ComponentFrame = vtkKWFrameWithLabel::New();
  this->ComponentFrame->SetParent(this->MainFrame);
  this->ComponentFrame->Create();
  this->ComponentFrame->SetLabelText("Building Block");
  this->ComponentFrame->CollapseFrame( );
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
    this->ComponentFrame->GetWidgetName());
      
 // for selecting the building-block structure
  if(!this->ObjectListComboBox)
    this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();
  this->ObjectListComboBox->SetParent(this->ComponentFrame->GetFrame());
  this->ObjectListComboBox->Create();
  this->ObjectListComboBox->SetWidth(20);
  this->ObjectListComboBox->SetLabelText("Building Block : ");
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox->GetWidget()->SetCommand(this, "SelectionChangedCallback");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
    this->ObjectListComboBox->GetWidgetName());

  vtkKWIcon *colorIcon = vtkKWIcon::New();
  colorIcon->SetImage( image_mimxColorSeeds, 
                       image_mimxColorSeeds_width, 
                       image_mimxColorSeeds_height, 
                       image_mimxColorSeeds_pixel_size);
  if(!this->ShowMeshSeedCheckButton)  
    this->ShowMeshSeedCheckButton = vtkKWCheckButtonWithLabel::New();
  this->ShowMeshSeedCheckButton->SetParent(this->MainFrame);
  this->ShowMeshSeedCheckButton->Create();
  this->ShowMeshSeedCheckButton->GetWidget()->SetBorderWidth(2);
  //this->ShowMeshSeedCheckButton->SetLabelText("Color Code Mesh Seed ");
  this->ShowMeshSeedCheckButton->GetWidget()->SetImageToIcon(colorIcon);
  this->ShowMeshSeedCheckButton->GetWidget()->SetSelectImageToIcon(colorIcon);
  //this->ShowMeshSeedCheckButton->SetLabelWidth(20);
  this->ShowMeshSeedCheckButton->SetLabelPositionToLeft();
  this->ShowMeshSeedCheckButton->GetWidget()->SetCommand(this, "ShowMeshSeedCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor center -expand n -padx 2 -pady 6", 
	  this->ShowMeshSeedCheckButton->GetWidgetName());
	  
  if(!this->EditTypeSelection)
	  this->EditTypeSelection = vtkKWMenuButtonWithLabel::New();
  this->EditTypeSelection->SetParent(this->MainFrame);
  this->EditTypeSelection->Create();
  //this->EditTypeSelection->SetLabelPositionToLeft();
  //this->EditTypeSelection->SetLabelText("Seed Type: ");
  //this->EditTypeSelection->SetLabelWidth(20);
  this->EditTypeSelection->GetWidget()->SetWidth(20);
  this->EditTypeSelection->GetWidget()->GetMenu()->AddRadioButton(
	  "Element Length",this, "UserDefinedElementLengthCheckButtonCallback");
	this->EditTypeSelection->GetWidget()->GetMenu()->AddRadioButton(
	  "Number of Divisions",this, "NumberOfDivisionsCheckButtonCallback");
	this->EditTypeSelection->GetWidget()->SetValue("Element Length");  
  this->EditTypeSelection->GetWidget()->GetMenu()->SelectItem(0);
  this->GetApplication()->Script( "pack %s -side top -anchor n -padx 2 -pady 2", 
	  this->EditTypeSelection->GetWidgetName());
	  
	  


  // to select edit or create
/*
  this->MeshSeedTypeFrame->SetParent(this->MainFrame);
  this->MeshSeedTypeFrame->Create();
  this->MeshSeedTypeFrame->SetLabelText("Mesh Seed Operation");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
	  this->MeshSeedTypeFrame->GetWidgetName());

  this->MeshSeedTypeButton->SetParent(this->MeshSeedTypeFrame->GetFrame());
  this->MeshSeedTypeButton->Create();
  this->MeshSeedTypeButton->SetBorderWidth(0);
  this->MeshSeedTypeButton->SetReliefToGroove();
  this->MeshSeedTypeButton->SetMaximumNumberOfWidgetsInPackingDirection(1);
  for (int id = 0; id < 2; id++)	 
	  this->MeshSeedTypeButton->AddWidget(id);

  this->MeshSeedTypeButton->GetWidget(0)->SetCommand(this, "MeshSeedTypeCreateCallback");
  this->MeshSeedTypeButton->GetWidget(0)->SetText("Create");
  this->MeshSeedTypeButton->GetWidget(0)->SetBalloonHelpString(
	  "Create mesh seeds");
  this->MeshSeedTypeButton->GetWidget(0)->SetValue("Create");
  this->MeshSeedTypeButton->GetWidget(0)->SetCompoundModeToLeft();

  this->MeshSeedTypeButton->GetWidget(1)->SetText("Edit");
  this->MeshSeedTypeButton->GetWidget(1)->SetCommand(this, "MeshSeedTypeEditCallback");
  this->MeshSeedTypeButton->GetWidget(1)->SetBalloonHelpString(
	  "Edit applied mesh seeds");
  this->MeshSeedTypeButton->GetWidget(1)->SetVariableName(
	  this->MeshSeedTypeButton->GetWidget(0)->GetVariableName());
  this->MeshSeedTypeButton->GetWidget(1)->SetValue("Edit");
  this->MeshSeedTypeButton->GetWidget(1)->SetCompoundModeToLeft();

  this->GetApplication()->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
	  this->MeshSeedTypeButton->GetWidgetName());
*/
/*
  if (!this->CellSelectionFrame)
    this->CellSelectionFrame = vtkKWFrameWithLabel::New();
  this->CellSelectionFrame->SetParent(this->MainFrame);
  this->CellSelectionFrame->Create();
  this->CellSelectionFrame->SetLabelText("Cell Selection");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->CellSelectionFrame->GetWidgetName());
*/	    

      
  
  // check buttons for mesh seed types
  // check button for number of divisions
  

/*
  if(!this->EditButton)
	  this->EditButton = vtkKWRadioButton::New();
  this->EditButton->SetParent(this->CellSelectionFrame->GetFrame());
  this->EditButton->Create();
  this->EditButton->SetCommand(this, "EditBBMeshSeedPickCellCallback");
  this->EditButton->SetText("On");
  this->EditButton->SetImageToPredefinedIcon(vtkKWIcon::IconBoundingBox);
  this->EditButton->IndicatorVisibilityOff();
  this->EditButton->SetBalloonHelpString("Turn on pick cell for mesh seed editing");
  this->EditButton->SetValue("Edit");
  this->EditButton->SetCompoundModeToLeft();

  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand y -padx 6 -pady 2", 
	  this->EditButton->GetWidgetName());

  this->VtkInteractionButton->SetParent(this->CellSelectionFrame->GetFrame());
  this->VtkInteractionButton->Create();
  this->VtkInteractionButton->SetText("Off");
  this->VtkInteractionButton->SetCommand(this, "EditBBMeshSeedVtkInteractionCallback");
  this->VtkInteractionButton->SetImageToPredefinedIcon(vtkKWIcon::IconTransportStop);
  this->VtkInteractionButton->IndicatorVisibilityOff();
  this->VtkInteractionButton->SetBalloonHelpString("Turn off pick cell for mesh seed editing");
  this->VtkInteractionButton->SetVariableName(this->EditButton->GetVariableName());
  this->VtkInteractionButton->SetValue("VtkInteraction");
  this->VtkInteractionButton->SetCompoundModeToLeft();
  this->GetApplication()->Script(
	  "pack %s -side right -anchor ne -expand 1 -padx 6 -pady 2", 
	  this->VtkInteractionButton->GetWidgetName());
  this->VtkInteractionButton->SelectedStateOn();
*/
  //
//  this->NumDivCheckButton->SetParent(this->EditTypeChoiceFrame->GetFrame());
//  this->NumDivCheckButton->SetText("Number of Divisions");
//  this->NumDivCheckButton->Create();
//  //this->NumDivCheckButton->SetLabelWidth( 15 );
//  this->NumDivCheckButton->SetCommand(this, "NumberOfDivisionsCheckButtonCallback");
//  //this->NumDivCheckButton->SetVariableName(
//	 // this->NumDivCheckButton->GetVariableName());
//  this->NumDivCheckButton->SetValueAsInt(12);
//  this->GetApplication()->Script(
//	  "pack %s -side top -anchor nw -expand y -padx 6 -pady 2 -fill x", 
//	  this->NumDivCheckButton->GetWidgetName());
//// check button for element length
//  this->ElementLengthCheckButton->SetParent(this->EditTypeChoiceFrame->GetFrame());
//  this->ElementLengthCheckButton->SetText("Element Length");
//  this->ElementLengthCheckButton->Create();
//  //this->ElementLengthCheckButton->SetLabelWidth( 15 );
//  this->ElementLengthCheckButton->SetCommand(this, 
//	  "UserDefinedElementLengthCheckButtonCallback");
//  this->ElementLengthCheckButton->SetValueAsInt(13);
//  this->ElementLengthCheckButton->SetVariableName(
//	  this->NumDivCheckButton->GetVariableName());
//
//  this->GetApplication()->Script(
//	  "pack %s -side right -anchor nw -expand y -padx 6 -pady 2 -fill x", 
//	  this->ElementLengthCheckButton->GetWidgetName());
  
  if (!this->EditTypeChoiceFrame)
    this->EditTypeChoiceFrame = vtkKWFrame::New();
  this->EditTypeChoiceFrame->SetParent(this->MainFrame);
  this->EditTypeChoiceFrame->Create();
  //this->EditTypeChoiceFrame->SetLabelText("Seeding Type");
  //this->EditTypeChoiceFrame->SetEnabled(0);
  this->GetApplication()->Script(
      "pack %s -side top -anchor center -padx 2 -pady 6", 
      this->EditTypeChoiceFrame->GetWidgetName());
      
  if(!this->XMeshSeed)
  {
	  this->XMeshSeed = vtkKWEntryWithLabel::New();
	  this->YMeshSeed = vtkKWEntryWithLabel::New();
	  this->ZMeshSeed = vtkKWEntryWithLabel::New();
  }
  this->XMeshSeed->SetParent(this->EditTypeChoiceFrame);
  this->YMeshSeed->SetParent(this->EditTypeChoiceFrame);
  this->ZMeshSeed->SetParent(this->EditTypeChoiceFrame);
  
  /* Pull the initial Settings from the Application */
  vtkKWMimxApplication *app
    = vtkKWMimxApplication::SafeDownCast(this->GetMimxMainWindow()->GetApplication());
  double edgeLength = app->GetAverageElementLength();
  
  this->AverageEdgeLength[0] = edgeLength;
  this->AverageEdgeLength[1] = edgeLength;
  this->AverageEdgeLength[2] = edgeLength;
  
  this->XMeshSeed->Create();
  this->YMeshSeed->Create();
  this->ZMeshSeed->Create();

  this->XMeshSeed->SetLabelText("Rx :");
  this->YMeshSeed->SetLabelText("Gy :");
  this->ZMeshSeed->SetLabelText("Bz :");
  this->XMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->YMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->ZMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->XMeshSeed->GetWidget()->SetValueAsDouble( this->AverageEdgeLength[0] );
  this->YMeshSeed->GetWidget()->SetValueAsDouble( this->AverageEdgeLength[1] );
  this->ZMeshSeed->GetWidget()->SetValueAsDouble( this->AverageEdgeLength[2] );
  //this->XMeshSeed->SetEnabled(0);
  //this->YMeshSeed->SetEnabled(0);
  //this->ZMeshSeed->SetEnabled(0);
  
  this->XMeshSeed->GetWidget()->SetWidth(5);
  this->YMeshSeed->GetWidget()->SetWidth(5);
  this->ZMeshSeed->GetWidget()->SetWidth(5);
  
  /*
  this->XMeshSeed->GetWidget()->SetCommand(this, "EditBBMeshSeedApplyCallback");
  this->YMeshSeed->GetWidget()->SetCommand(this, "EditBBMeshSeedApplyCallback");
  this->ZMeshSeed->GetWidget()->SetCommand(this, "EditBBMeshSeedApplyCallback");
  */
  this->GetApplication()->Script(
      "pack %s -side left -anchor center -expand n -padx 5 -pady 6", 
      this->XMeshSeed->GetWidgetName());
  
  this->GetApplication()->Script(
      "pack %s -side left -anchor center -expand n -padx 5 -pady 6", 
      this->YMeshSeed->GetWidgetName());
      
  this->GetApplication()->Script(
      "pack %s -side left -anchor center -expand n -padx 5 -pady 6", 
      this->ZMeshSeed->GetWidgetName());
  
  
      
	//this->EditTypeSelection->GetWidget(1)->SelectedStateOn();		
/*
  this->AxisSelection->SetParent(this->EditTypeChoiceFrame->GetFrame());
  this->AxisSelection->Create();
  this->AxisSelection->SetBorderWidth(2);
  this->AxisSelection->SetReliefToGroove();
  


  vtkKWRadioButton *rb;
  rb = this->AxisSelection->AddWidget(0);
  rb->SetText(" X ");
  rb = this->AxisSelection->AddWidget(1);
  rb->SetText(" Y ");
  rb = this->AxisSelection->AddWidget(2);
  rb->SetText(" Z ");
  this->AxisSelection->SetPackHorizontally(1);

  this->AxisSelection->GetWidget(0)->SetSelectedState(1);

  this->UserDefinedElementLength->SetParent(this->EditTypeChoiceFrame->GetFrame());
  this->UserDefinedElementLength->Create();
  this->UserDefinedElementLength->SetLabelText("Element Length : ");
  this->UserDefinedElementLength->GetWidget()->SetRestrictValueToDouble();
  this->UserDefinedElementLength->GetWidget()->SetWidth(5);
*/

  if (!this->ButtonFrame)	
	  this->ButtonFrame = vtkKWFrame::New();
  this->ButtonFrame->SetParent(this->MainFrame);
  this->ButtonFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
    this->ButtonFrame->GetWidgetName());
    

  this->ApplyButton->SetParent(this->ButtonFrame);
  this->ApplyButton->Create();
  this->ApplyButton->SetReliefToFlat();
  this->ApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
  //this->ApplyButton->SetText("Apply");
  this->ApplyButton->SetCommand(this, "EditBBMeshSeedApplyCallback");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand n -padx 5 -pady 6", 
	  this->ApplyButton->GetWidgetName());


  this->CancelButton->SetParent(this->ButtonFrame);
  this->CancelButton->Create();
  this->CancelButton->SetReliefToFlat();
  this->CancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
  //this->CancelButton->SetText("Cancel");
  this->CancelButton->SetCommand(this, "EditBBMeshSeedCancelCallback");
  this->GetApplication()->Script(
    "pack %s -side right -anchor ne -expand n -padx 5 -pady 6", 
    this->CancelButton->GetWidgetName());

	  
  //this->EditTypeChoiceFrame->SetEnabled(0);
  //for (int i=0; i<this->EditTypeChoiceFrame->GetFrame()->GetNumberOfChildren(); i++)
  //{
	 // this->EditTypeChoiceFrame->GetFrame()->GetNthChild(i)->SetEnabled(0);
  //}
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxEditBBMeshSeedGroup::EditBBMeshSeedApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	callback->SetState(0);
	
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	  
 // if(!this->EditButton->GetSelectedState())
 // {
	//callback->ErrorMessage("Mesh seed button should be selected");
	//return 0;
 // }
  if(!strcmp(name,""))
  {
	  callback->ErrorMessage("Building Block selection required");
	  return 0;
  }
  /*
  if(this->MeshSeedTypeButton->GetWidget(0)->GetSelectedState())
  {
	  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	  //	callback->SetState(0);
	  if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	  {
		  callback->ErrorMessage("Building Block to be mesh seeded not selected");
		  return 0;
	  }

	  if(this->AvElementLength->GetWidget()->GetValueAsDouble() <= 0.0)
	  {
		  callback->ErrorMessage("Average element length cannot be <= 0.0");
		  return 0;
	  }

	  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	  const char *name = combobox->GetValue();

	  int num = combobox->GetValueIndex(name);
	  if(num < 0 || num > combobox->GetNumberOfValues()-1)
	  {
		  callback->ErrorMessage("Choose valid Building-block structure");
		  combobox->SetValue("");
		  return 0;
	  }

	  vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
		  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
	  vtkUnstructuredGrid *ugrid = ugridactor->GetDataSet();
	  if(!ugrid->GetCellData()->GetArray("Mesh_Seed"))
	  {
		  double elementLength = this->AvElementLength->GetWidget()->GetValueAsDouble();
		  ugridactor->MeshSeedFromAverageElementLength(elementLength, elementLength, elementLength);
	  }
	  else
	  {
		  vtkKWMessageDialog *Dialog = vtkKWMessageDialog::New();
		  Dialog->SetStyleToOkCancel();
		  Dialog->SetApplication(this->GetApplication());
		  Dialog->Create();
		  Dialog->SetTitle("Your attention please!");
		  Dialog->SetText("Mesh seeds already present, would you like to overwrite ?");
		  Dialog->Invoke();
		  if(Dialog->GetStatus() == vtkKWMessageDialog::StatusOK)
		  {
			  double elementLength = this->AvElementLength->GetWidget()->GetValueAsDouble();
			  ugridactor->MeshSeedFromAverageElementLength(elementLength, elementLength, elementLength);
		  }
		  Dialog->Delete();
	  }
	  callback->ErrorMessage("Mesh Seeding Done.");
	  return 1;
  }
  */
  bool modifyX, modifyY, modifyZ;
  int xSize, ySize, zSize;
  double xLength, yLength, zLength;
  int mode;
  
  xLength = yLength = zLength = 0.0;
  xSize = ySize = zSize = 0;
  modifyX = modifyY = modifyZ = false;
  
  if ( this->EditTypeSelection->GetWidget()->GetMenu()->GetItemSelectedState(1) )
  {
    //std::cout << "Setting Number of Divisions" << std::endl;
    const char *tmpValueX = this->XMeshSeed->GetWidget()->GetValue();
    if (strlen(tmpValueX) > 0)
    {
      xSize = this->XMeshSeed->GetWidget()->GetValueAsInt();
      if (xSize < 1)
  	  {
  		  callback->ErrorMessage("Number of X divisions must be greater than 0");
  		  return 0;
  	  }
      modifyX = true;
    }
    const char *tmpValueY = this->YMeshSeed->GetWidget()->GetValue();
    if (strlen(tmpValueY) > 0)
    {
      ySize = this->YMeshSeed->GetWidget()->GetValueAsInt();
      if (ySize < 1)
  	  {
  		  callback->ErrorMessage("Number of Y divisions must be greater than 0");
  		  return 0;
  	  }
      modifyY = true;
    }
    const char *tmpValueZ = this->ZMeshSeed->GetWidget()->GetValue();
    if (strlen(tmpValueZ) > 0)
    {
      zSize = this->ZMeshSeed->GetWidget()->GetValueAsInt();
      if (zSize < 1)
  	  {
  		  callback->ErrorMessage("Number of Z divisions must be greater than 0");
  		  return 0;
  	  }
      modifyZ = true;
    }
    mode = 1;
  }
  else
  {
    //std::cout << "Setting Element length" << std::endl;
    const char *tmpValueX = this->XMeshSeed->GetWidget()->GetValue();
    if (strlen(tmpValueX) > 0)
    {
      xLength = this->XMeshSeed->GetWidget()->GetValueAsDouble();
      if (xLength <= 0.0)
  	  {
  		  callback->ErrorMessage("Average X length must be greater than 0.0");
  		  return 0;
  	  }
      modifyX = true;
      this->AverageEdgeLength[0] = xLength;
    }
    const char *tmpValueY = this->YMeshSeed->GetWidget()->GetValue();
    if (strlen(tmpValueY) > 0)
    {
      yLength = this->YMeshSeed->GetWidget()->GetValueAsDouble();
      if (yLength <= 0.0)
  	  {
  		  callback->ErrorMessage("Average Y length must be greater than 0.0");
  		  return 0;
  	  }
      modifyY = true;
      this->AverageEdgeLength[1] = yLength;
    }
    const char *tmpValueZ = this->ZMeshSeed->GetWidget()->GetValue();
    if (strlen(tmpValueZ) > 0)
    {
      zLength = this->ZMeshSeed->GetWidget()->GetValueAsDouble();
      if (zLength < 0.0)
  	  {
  		  callback->ErrorMessage("Average Z length must be greater than 0.0");
  		  return 0;
  	  }
      modifyZ = true;
      this->AverageEdgeLength[2] = zLength;
    }   
    mode = 2;
  }
  
  int cellMin = 0;
  int cellMax = 0;
  if (this->LocalAxesWidget->GetEnabled())
	{
		if(this->LocalAxesWidget->GetPickedCell() < 0)
		{
			vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::
				  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
  		cellMin = 0;
  		cellMax = ugrid->GetNumberOfCells();
		}
		else
		{
		  cellMin = this->LocalAxesWidget->GetPickedCell();
		  cellMax = this->LocalAxesWidget->GetPickedCell()+1;
		}
	}
	else
	{
	  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::
				  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		cellMin = 0;
		cellMax = ugrid->GetNumberOfCells();
	}
	
	//std::cout << "Cell Min: " << cellMin << std::endl;
	//std::cout << "Cell Max: " << cellMax << std::endl;
	
	vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
				  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
				  
	if ( mode == 1 )
	{
	  for (int i=cellMin;i<cellMax;i++)
	  {
		  if (modifyX ) ugridactor->ChangeMeshSeed(i, 0, xSize+1);
		  if (modifyY ) ugridactor->ChangeMeshSeed(i, 1, ySize+1);	
		  if (modifyZ ) ugridactor->ChangeMeshSeed(i, 2, zSize+1);
		}
	}
	else
	{
	  for (int i=cellMin;i<cellMax;i++)
	  {
		  if (modifyX ) ugridactor->ChangeMeshSeedAvElLength(i, 0, xLength);
		  if (modifyY ) ugridactor->ChangeMeshSeedAvElLength(i, 1, yLength);	
		  if (modifyZ ) ugridactor->ChangeMeshSeedAvElLength(i, 2, zLength);
		}
	}	
	
	if(this->ShowMeshSeedCheckButton->GetWidget()->GetSelectedState())
	{
		this->ShowMeshSeedCheckButton->GetWidget()->SetSelectedState(0);
	}
	this->ShowMeshSeedCheckButton->GetWidget()->SetSelectedState(1);
 // if (this->ColorCodeMeshSeedActor)
	//{
	//	this->GetMimxMainWindow()->GetRenderWidget()->
	//			RemoveViewProp(this->ColorCodeMeshSeedActor->GetMeshSeedActor());
	//	this->GetMimxMainWindow()->GetRenderWidget()->
	//			RemoveViewProp(this->ColorCodeMeshSeedActor->GetScalarBarActor());
	//	this->ColorCodeMeshSeedActor->Delete();
	//	
	//	vtkUnstructuredGrid *ugrid = ugridactor->GetDataSet();
	//	this->ColorCodeMeshSeedActor = vtkMimxColorCodeMeshSeedActor::New();
	//	vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
	//	this->ColorCodeMeshSeedActor->SetLabelTextColor( app->GetTextColor() );
	//	this->ColorCodeMeshSeedActor->SetInput(ugrid);
	//	this->GetMimxMainWindow()->GetRenderWidget()->
	//		AddViewProp(this->ColorCodeMeshSeedActor->GetMeshSeedActor());
	//	this->GetMimxMainWindow()->GetRenderWidget()->
	//		AddViewProp(this->ColorCodeMeshSeedActor->GetScalarBarActor());
	//	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//}
	
	this->GetMimxMainWindow()->SetStatusText("Edited Building Block Mesh Seed");
		  
	/*
  if ( this->LocalAxesWidget )
  {
	  if(this->LocalAxesWidget->GetEnabled())
	  {
		  if(this->LocalAxesWidget->GetPickedCell() < 0)
		  {
			  callback->ErrorMessage("Invalid cell picked");
			  return 0;
		  }
		  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		  const char *name = combobox->GetValue();
		  int num = combobox->GetValueIndex(name);

		  if(num < 0 || num > combobox->GetNumberOfValues()-1)
		  {
			  callback->ErrorMessage("Choose valid Building-block structure");
			  combobox->SetValue("");
			  return 0;
		  }
		  
		  if(!this->EditTypeSelection->GetWidget(1)->GetSelectedState() && 
			  !this->EditTypeSelection->GetWidget(0)->GetSelectedState())
		  {
			  callback->ErrorMessage("Choose Mesh seed type");
			  combobox->SetValue("");
			  return 0;
		  }
		  if(this->EditTypeSelection->GetWidget(1)->GetSelectedState())
		  {
			  int x, y, z;
			  x = this->XMeshSeed->GetWidget()->GetValueAsInt();
			  y = this->YMeshSeed->GetWidget()->GetValueAsInt();
			  z = this->ZMeshSeed->GetWidget()->GetValueAsInt();
			  if(x < 1 || y < 1 || z < 1)
			  {
				  callback->ErrorMessage("Mesh seeds cannot be < 1 ");
				  return 0;
			  }
			  vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
				  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
			  ugridactor->ChangeMeshSeed(this->LocalAxesWidget->GetPickedCell(), 0, x+1);
			  ugridactor->ChangeMeshSeed(this->LocalAxesWidget->GetPickedCell(), 1, y+1);	
			  ugridactor->ChangeMeshSeed(this->LocalAxesWidget->GetPickedCell(), 2, z+1);
//			  this->VtkInteractionButton->SelectedStateOn();
			  return 1;
		  }
		  else
		  {
			  int Axis;
			  if(this->AxisSelection->GetWidget(0)->GetSelectedState())	Axis = 0;
			  if(this->AxisSelection->GetWidget(1)->GetSelectedState())	Axis = 1;
			  if(this->AxisSelection->GetWidget(2)->GetSelectedState())	Axis = 2;
			  vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
				  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
			  ugridactor->ChangeMeshSeedAvElLength(this->LocalAxesWidget->GetPickedCell(), 
				  Axis, this->UserDefinedElementLength->GetWidget()->GetValueAsDouble());
//			  this->VtkInteractionButton->SelectedStateOn();
			  return 1;

		  }
	  }
  }
//  this->VtkInteractionButton->SelectedStateOn();
*/
  return 1;	
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::EditBBMeshSeedCancelCallback()
{
	this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName());
	this->EditBBMeshSeedVtkInteractionCallback();
	this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
	if (this->ColorCodeMeshSeedActor)
	{
		this->GetMimxMainWindow()->GetRenderWidget()->
			RemoveViewProp(this->ColorCodeMeshSeedActor->GetMeshSeedActor());
		this->GetMimxMainWindow()->GetRenderWidget()->
			RemoveViewProp(this->ColorCodeMeshSeedActor->GetScalarBarActor());
		this->ColorCodeMeshSeedActor->Delete();
		this->ColorCodeMeshSeedActor = NULL;
		
		/* Return the original Actor */
		this->ReturnUnstructuredGridActor(this->ObjectListComboBox->GetWidget()->GetValue());
			
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}
	strcpy(this->PreviousSelection, "");
	this->GetMimxMainWindow()->GetViewProperties()->EnableViewPropertyList(1);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::EditBBMeshSeedPickCellCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Building Block selection required");
		return;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Building-block structure");
		combobox->SetValue("");
		return ;
	}
	int i;
	this->EditTypeChoiceFrame->SetEnabled(1);
	for (i=0; i<this->EditTypeChoiceFrame->GetNumberOfChildren(); i++)
	{
		this->EditTypeChoiceFrame->GetNthChild(i)->SetEnabled(1);
	}
	vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
		this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
	if(this->LocalAxesWidget)
	{
		if(this->LocalAxesWidget->GetEnabled())
		{
			this->LocalAxesWidget->SetEnabled(0);
		}
		this->LocalAxesWidget->Delete();
		this->LocalAxesWidget = NULL;
	}
	this->LocalAxesWidget = vtkMimxPlaceLocalAxesWidget::New();
	this->LocalAxesWidget->SetInteractor(this->GetMimxMainWindow()->
		GetRenderWidget()->GetRenderWindowInteractor());
	this->LocalAxesWidget->SetInput(ugrid);
	this->LocalAxesWidget->SetInputActor(this->BBoxList->GetItem(
		combobox->GetValueIndex(name))->GetActor());
	this->LocalAxesWidget->SetXMeshSeed(this->XMeshSeed);
	this->LocalAxesWidget->SetYMeshSeed(this->YMeshSeed);
	this->LocalAxesWidget->SetZMeshSeed(this->ZMeshSeed);
	/***VAM ***/
	this->LocalAxesWidget->SetSelectionTypeWidget(this->EditTypeSelection);
	/***VAM - Fix Average Element Length to update based on the Selected Cell */
	this->LocalAxesWidget->SetEnabled(1);
}
//----------------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::EditBBMeshSeedVtkInteractionCallback()
{
  if(this->LocalAxesWidget)
  {
	  if(this->LocalAxesWidget->GetEnabled())
	  {
		  this->LocalAxesWidget->SetEnabled(0);
	  }
  }
  /*
  if(this->XMeshSeed)
  {
	  this->XMeshSeed->SetEnabled(0);
	  this->YMeshSeed->SetEnabled(0);
	  this->ZMeshSeed->SetEnabled(0);
  }
  */
  //this->EditTypeChoiceFrame->SetEnabled(0);
 /* int i;
  for (i=0; i<this->EditTypeChoiceFrame->GetFrame()->GetNumberOfChildren(); i++)
  {
	  this->EditTypeChoiceFrame->GetFrame()->GetNthChild(i)->SetEnabled(0);
  }*/
}
//-----------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();

	int defaultItem = -1;
	for (int i = 0; i < this->BBoxList->GetNumberOfItems(); i++)
	{
		ObjectListComboBox->GetWidget()->AddValue(
			this->BBoxList->GetItem(i)->GetFileName());
		int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->
			GetRenderer()->HasViewProp(this->BBoxList->GetItem(i)->GetActor());
		if ( (defaultItem == -1) && ( viewedItem ) )
		{
			defaultItem = i;
		}
	}
  
  if ((this->BBoxList->GetNumberOfItems() > 0) && (defaultItem == -1))
    defaultItem = this->BBoxList->GetNumberOfItems()-1;
    
	if (defaultItem != -1)
	{
		ObjectListComboBox->GetWidget()->SetValue(
			this->BBoxList->GetItem(defaultItem)->GetFileName());
		strcpy(this->PreviousSelection, this->BBoxList->GetItem(defaultItem)->GetFileName());
		this->EditBBMeshSeedPickCellCallback();
	}
	else
	{
	  this->EditBBMeshSeedVtkInteractionCallback();
	}
  
  return;
	
}
//--------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::EditBBMeshSeedDoneCallback()
{
	if(this->EditBBMeshSeedApplyCallback())
		this->EditBBMeshSeedCancelCallback();
}
//---------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::NumberOfDivisionsCheckButtonCallback()
{
	
	this->EditTypeSelection->GetWidget()->SetValue("Number of Divisions"); 
	
	if ( this->LocalAxesWidget )
	  this->LocalAxesWidget->SetEnabled(1);
	  
	this->XMeshSeed->GetWidget()->SetRestrictValueToInteger();
  this->YMeshSeed->GetWidget()->SetRestrictValueToInteger();
  this->ZMeshSeed->GetWidget()->SetRestrictValueToInteger();
  
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);

  if ((num >= 0) && (num <= combobox->GetNumberOfValues()-1) &&
        this->LocalAxesWidget->GetEnabled() &&
      (this->LocalAxesWidget->GetPickedCell() >= 0) )
  {
    int numSeed = this->LocalAxesWidget->GetMeshSeedX();
    this->XMeshSeed->GetWidget()->SetValueAsInt(numSeed-1);
    numSeed = this->LocalAxesWidget->GetMeshSeedY();
    this->YMeshSeed->GetWidget()->SetValueAsInt(numSeed-1);
    numSeed = this->LocalAxesWidget->GetMeshSeedZ();
    this->ZMeshSeed->GetWidget()->SetValueAsInt(numSeed-1);
  }
  else
  {
    this->XMeshSeed->GetWidget()->SetValue("");
    this->YMeshSeed->GetWidget()->SetValue("");
    this->ZMeshSeed->GetWidget()->SetValue("");
  }

	//this->EditTypeSelection->GetMenu()->SetSelectedState(0);
	//this->UserDefinedElementLengthCheckButtonCallback();
	
  
}
//---------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::UserDefinedElementLengthCheckButtonCallback()
{
	this->EditTypeSelection->GetWidget()->SetValue("Element Length");
	
	if ( this->LocalAxesWidget )
	  this->LocalAxesWidget->SetEnabled(1);
	
	//this->EditTypeSelection->GetWidget(0)->SetSelectedState(0);
	
	this->XMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->YMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->ZMeshSeed->GetWidget()->SetRestrictValueToDouble();
  
  /***VAM - This should be queries from the Axes Widget */
  this->XMeshSeed->GetWidget()->SetValueAsDouble(this->AverageEdgeLength[0]);
  this->YMeshSeed->GetWidget()->SetValueAsDouble(this->AverageEdgeLength[1]);
  this->ZMeshSeed->GetWidget()->SetValueAsDouble(this->AverageEdgeLength[2]);
  
}
//----------------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::MeshSeedTypeCreateCallback()
{
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
		this->AvElementLength->GetWidgetName());

	this->GetApplication()->Script("pack forget %s", this->CellSelectionFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->EditTypeChoiceFrame->GetWidgetName());

}
//----------------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::MeshSeedTypeEditCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	callback->SetState(0);
	this->GetApplication()->Script("pack forget %s", this->AvElementLength->GetWidgetName());

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);

	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Building-block structure");
		combobox->SetValue("");
		return ;
	}
	
	vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
		SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
	vtkUnstructuredGrid *ugrid = ugridactor->GetDataSet();
	if(!ugrid->GetCellData()->GetArray("Mesh_Seed"))
	{
		callback->ErrorMessage("Mesh Seed data does not exist, You should create mesh seeds before editing them");
		return ;
	}

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
		this->CellSelectionFrame->GetWidgetName());

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
		this->EditTypeChoiceFrame->GetWidgetName());

}
//----------------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::ShowMeshSeedCallback(int State)
{
	if (State)
	{
		vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
		callback->SetState(0);

		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);

		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			this->ShowMeshSeedCheckButton->GetWidget()->SelectedStateOff();
			return ;
		}

		vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
			SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
		vtkUnstructuredGrid *ugrid = ugridactor->GetDataSet();
		if(!ugrid->GetCellData()->GetArray("Mesh_Seed"))
		{
			callback->ErrorMessage("Mesh Seed data does not exist, You should create mesh seeds before editing them");
			this->ShowMeshSeedCheckButton->SetState(0);
			return ;
		}
		if(this->ColorCodeMeshSeedActor)
		{
			this->GetMimxMainWindow()->GetRenderWidget()->
				RemoveViewProp(this->ColorCodeMeshSeedActor->GetMeshSeedActor());
			this->GetMimxMainWindow()->GetRenderWidget()->
				RemoveViewProp(this->ColorCodeMeshSeedActor->GetScalarBarActor());
			this->ColorCodeMeshSeedActor->Delete();
		}
		this->ColorCodeMeshSeedActor = vtkMimxColorCodeMeshSeedActor::New();
		vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
		this->ColorCodeMeshSeedActor->SetLabelTextColor( app->GetTextColor() );
		this->ColorCodeMeshSeedActor->SetLineWidth(
			ugridactor->GetActor()->GetProperty()->GetLineWidth()*2.0);
		this->ColorCodeMeshSeedActor->SetInput(ugrid);
		this->GetMimxMainWindow()->GetRenderWidget()->
			AddViewProp(this->ColorCodeMeshSeedActor->GetMeshSeedActor());
		this->GetMimxMainWindow()->GetRenderWidget()->
			AddViewProp(this->ColorCodeMeshSeedActor->GetScalarBarActor());
		//if(this->LocalAxesWidget)
		//{
		//	this->LocalAxesWidget->SetEnabled(0);
		//}
		this->GetMimxMainWindow()->GetRenderWidget()->
			RemoveViewProp(ugridactor->GetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		//this->GetApplication()->Script(
		//	"pack forget %s", this->MeshSeedTypeFrame->GetWidgetName());
		//int i;
		//this->MeshSeedTypeFrame->SetEnabled(0);
		//for (i=0; i<this->MeshSeedTypeFrame->GetFrame()->GetNumberOfChildren(); i++)
		//{
		//	this->MeshSeedTypeFrame->GetFrame()->GetNthChild(i)->SetEnabled(0);
		//}
		this->ShowMeshSeedCheckButton->GetWidget()->SetReliefToSunken();
		this->GetMimxMainWindow()->GetViewProperties()->EnableViewPropertyList(0);
	}	
	else
	{
		if(this->ColorCodeMeshSeedActor)
		{
			this->GetMimxMainWindow()->GetRenderWidget()->
				RemoveViewProp(this->ColorCodeMeshSeedActor->GetMeshSeedActor());
			this->GetMimxMainWindow()->GetRenderWidget()->
				RemoveViewProp(this->ColorCodeMeshSeedActor->GetScalarBarActor());
			this->ColorCodeMeshSeedActor->Delete();
			this->ColorCodeMeshSeedActor = NULL;
		}
		//if(this->LocalAxesWidget)
		//	this->LocalAxesWidget->SetEnabled(1);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		//this->GetApplication()->Script(
		//	"pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
		//	this->MeshSeedTypeFrame->GetWidgetName());
		//int i;
		//this->MeshSeedTypeFrame->SetEnabled(1);
		//for (i=0; i<this->MeshSeedTypeFrame->GetFrame()->GetNumberOfChildren(); i++)
		//{
		//	this->MeshSeedTypeFrame->GetFrame()->GetNthChild(i)->SetEnabled(1);
		//}
		this->ShowMeshSeedCheckButton->GetWidget()->SetReliefToRaised();
		this->GetMimxMainWindow()->GetViewProperties()->EnableViewPropertyList(1);
	}
}
//----------------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::InitializeState( )
{
  this->EditTypeSelection->GetWidget()->GetMenu()->SelectItem(0);
  this->XMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->YMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->ZMeshSeed->GetWidget()->SetRestrictValueToDouble();
  this->XMeshSeed->GetWidget()->SetValueAsDouble(this->AverageEdgeLength[0]);
  this->YMeshSeed->GetWidget()->SetValueAsDouble(this->AverageEdgeLength[1]);
  this->ZMeshSeed->GetWidget()->SetValueAsDouble(this->AverageEdgeLength[2]);
  this->ShowMeshSeedCheckButton->GetWidget()->SetSelectedState(0);
  
}
//----------------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBMeshSeedGroup::ReturnUnstructuredGridActor(const char *name)
{
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
    SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
  this->GetMimxMainWindow()->GetRenderWidget()->
  	AddViewProp(ugridactor->GetActor());
}
//-------------------------------------------------------------------------------------------------------
