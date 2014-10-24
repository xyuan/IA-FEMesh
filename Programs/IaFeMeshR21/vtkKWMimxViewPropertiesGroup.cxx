/*=========================================================================
Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewPropertiesGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/08/17 01:42:57 $
Version:   $Revision: 1.26.4.1 $

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

#include "vtkKWMimxViewPropertiesGroup.h"

#include "vtkLinkedListWrapper.h"
#include "vtkLinkedListWrapperTree.h"

#include "vtkKWApplication.h"
#include "vtkKWChangeColorButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWIcon.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkRenderer.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWThumbWheel.h"
#include "vtkKWTkUtilities.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkMimxMeshActor.h"
#include "vtkKWFrameWithScrollbar.h"
#include "vtkKWSpinBoxWithLabel.h"
#include "vtkKWSpinBox.h"
#include "vtkKWScale.h"
#include "vtkMeshQualityClass.h"
#include "vtkActor2D.h"
#include "vtkKWRange.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkPlane.h"
#include "vtkPlaneWidget.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkCommand.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRadioButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWDialog.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWSeparator.h"
#include "vtkStringArray.h"
#include "vtkKWIcon.h"

#include "vtkActor.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

#include "Resources/mimxClose.h"

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxViewPropertiesGroup);
vtkCxxRevisionMacro(vtkKWMimxViewPropertiesGroup, "$Revision: 1.26.4.1 $");

//----------------------------------------------------------------------------
class vtkPlaneWidgetEventCallback : public vtkCommand
{
public:
	static vtkPlaneWidgetEventCallback *New() 
	{ return new vtkPlaneWidgetEventCallback; }
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkPlaneWidget *planeWidget = reinterpret_cast<vtkPlaneWidget*>(caller);
		planeWidget->GetPlane(this->PlaneInstance);

		// can't drag the plane and have the mesh highlight at the same time, so clear this mode
	//	if (((vtkKWMimxViewPropertiesGroup*)myInterface)->MeshQualityClass->GetElementsHighlighted())
	//	{
	//		((vtkKWMimxViewPropertiesGroup*)myInterface)->MeshQualityClass->CancelElementHighlight();
	////		((vtkKWMimxViewPropertiesGroup*)myInterface)->HighlightCellsButton->GetWidget()->SelectedStateOff();
	//	}
		// if real-time updates are selected, then redraw the interface after
		// each movement of the plane. Could be slow for large meshes, so it is
		// gated by the RealTimeWarningUpdate status variable, set through the GUI  
	/*	if (((vtkKWMimxViewPropertiesGroup*)myInterface)->RealTimeWarningUpdate)
		{
			((vtkKWMimxViewPropertiesGroup*)myInterface)->UpdateRenderingState();
		}*/
	}

	void SetUserInterfacePointer (vtkKWWindow *userInterface)
	{ myInterface = userInterface;}

	vtkPlaneWidgetEventCallback():PlaneInstance(0) {}
	vtkPlane *PlaneInstance;
	vtkKWWindow *myInterface;

};
//----------------------------------------------------------------------------
vtkKWMimxViewPropertiesGroup::vtkKWMimxViewPropertiesGroup()
{
  this->ObjectList = NULL;
  this->MimxMainWindow = NULL;
  this->MainFrame = NULL;
  this->ActorColorButton = NULL;
  this->ElementDataColorButton = NULL;
  this->DisplayStyleMenuButton = NULL;
  this->LinewidthThumbwheel = NULL;
  this->OpacityThumbwheel = NULL;
  this->NodeDataMenuButton = NULL;
  this->ElementDataMenuButton = NULL;
  this->DoUndoTree = NULL;
  this->PropertyFrame = NULL;
  this->ColorFrame = NULL;
  this->StyleFrame = NULL;
  this->NodeDataFrame = NULL;
  this->ElementDataFrame = NULL;
  this->MultiColumnList = NULL;
  this->ElementSizeFrame = NULL;
  this->ElementSizeScale = NULL;
  this->MeshQualityClass = NULL;
  this->OverrideColorButton = NULL;
  this->ElementColorRange = NULL;
  this->PlaneControlFrame = NULL;
  this->EnablePlaneButton = NULL;
  this->InvertSelector = NULL;
  this->PlaneMoveCallback = NULL;
  this->ViewPropertiesDialog = NULL;
  this->CloseButton = NULL;
  this->OutlineButton = NULL;
  this->OutlineColorButton = NULL;
  this->ColorMenuButton = NULL;
  this->ElementSetList = NULL;
  this->DisplayStyleMenuButton = NULL;
  this->DisplayModeButton = NULL;
  this->ObjectNameLabel = NULL;
  //this->MeshQualityClass = vtkMeshQualityClass::New();
  this->InInput = vtkUnstructuredGrid::New();
  this->SavedImplicitPlane = vtkPlane::New();
  this->SavedPlaneWidget = vtkPlaneWidget::New();
  this->SavedPlaneWidget->SetInput(this->InInput);
  this->SavedPlaneWidget->SetRepresentationToSurface();
  this->SavedPlaneWidget->GetPlaneProperty()->SetColor(0.5,0.5,0.5);
  this->SavedPlaneWidget->GetPlaneProperty()->SetOpacity(0.2);
  //this->MeshQualityClass->InitializeFromExternalMesh(this->InInput);
  this->SelectionRow = -1;
  strcpy(this->ItemName, "");
  strcpy(this->ItemId, "");
  this->inializedElementSetList = false;
}

//----------------------------------------------------------------------------
vtkKWMimxViewPropertiesGroup::~vtkKWMimxViewPropertiesGroup()
{
  if(this->MainFrame)
	  this->MainFrame->Delete();
	if (this->PropertyFrame)
	  this->PropertyFrame->Delete();
  if(this->ActorColorButton)
	  this->ActorColorButton->Delete();
  if(this->DisplayStyleMenuButton)
	  this->DisplayStyleMenuButton->Delete();
  if(this->LinewidthThumbwheel)
	  this->LinewidthThumbwheel->Delete();
  if(this->OpacityThumbwheel)
	  this->OpacityThumbwheel->Delete();
  if(this->NodeDataMenuButton)
	  this->NodeDataMenuButton->Delete();
  if(this->ElementDataMenuButton)
	  this->ElementDataMenuButton->Delete();
	if(this->ColorFrame)
	  this->ColorFrame->Delete();
	if(this->StyleFrame)
	  this->StyleFrame->Delete();
	if(this->NodeDataFrame)
		this->NodeDataFrame->Delete();
	if(this->ElementDataFrame)
		this->ElementDataFrame->Delete();
	if (this->ElementDataColorButton)
		this->ElementDataColorButton->Delete();
	this->ElementSizeFrame->Delete();
	if(this->ElementSizeScale)
		this->ElementSizeScale->Delete();
	if(this->MeshQualityClass)
		this->MeshQualityClass->Delete();
	if(this->OverrideColorButton)
		this->OverrideColorButton->Delete();
	if(this->ElementColorRange)
		this->ElementColorRange->Delete();
	if (this->SavedImplicitPlane)
	  this->SavedImplicitPlane->Delete();
	if (this->SavedPlaneWidget)
	  this->SavedPlaneWidget->Delete();
	if(this->PlaneControlFrame)
		this->PlaneControlFrame->Delete();
	if(this->EnablePlaneButton)
		this->EnablePlaneButton->Delete();
	if(this->InvertSelector)
		this->InvertSelector->Delete();
	if(this->PlaneMoveCallback)
		this->PlaneMoveCallback->Delete();
	if (this->InInput)
	  this->InInput->Delete();
	if (this->ViewPropertiesDialog)
	  this->ViewPropertiesDialog->Delete();
	if (this->CloseButton)
	  this->CloseButton->Delete();
  if (this->OutlineButton)
	  this->OutlineButton->Delete();
  if (this->OutlineColorButton)
	  this->OutlineColorButton->Delete();
  if (this->ColorMenuButton)
	  this->ColorMenuButton->Delete();
	if (this->ElementSetList)
	  this->ElementSetList->Delete();
	if (this->DisplayModeButton)
	  this->DisplayModeButton->Delete();
	if (this->ObjectNameLabel)
	  this->ObjectNameLabel->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::CreateWidget()
{

	if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }
  
  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  this->SetResizable(1, 1);
  this->SetMinimumSize(250, 390);
  this->SetSize(250, 390);
  this->ModalOff( );
  this->SetMasterWindow(this->GetMimxMainWindow());
  this->SetTitle("Display Properties");
      
  if(!this->DisplayModeButton)
	  this->DisplayModeButton = vtkKWMenuButtonWithLabel::New();	  
  this->DisplayModeButton->SetParent(this);
  this->DisplayModeButton->Create();
  this->DisplayModeButton->SetLabelPositionToLeft();
  this->DisplayModeButton->SetLabelText("Mode: ");
  this->DisplayModeButton->SetLabelWidth(6);
  this->DisplayModeButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Mesh",this, "DisplayModeCallback 1");
  this->DisplayModeButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Element Sets",this, "DisplayModeCallback 2");
  this->DisplayModeButton->GetWidget()->SetValue("Mesh");
  this->GetApplication()->Script(
    "pack %s -side top -anchor n -expand n -padx 10 -pady 6 -fill x", 
    this->DisplayModeButton->GetWidgetName());  
  
  if (!this->ElementSetList)
    this->ElementSetList = vtkKWMultiColumnListWithScrollbars::New();

  this->ElementSetList->SetParent( this );
  this->ElementSetList->Create();
  this->ElementSetList->GetWidget()->ClearStripeBackgroundColor();
  this->ElementSetList->SetHorizontalScrollbarVisibility(0);
  //this->ElementSetList->GetWidget()->SetSelectionBackgroundColor(1.0,1.0,1.0);
  this->ElementSetList->GetWidget()->SetSortArrowVisibility(0);
  this->ElementSetList->GetWidget()->ColumnSeparatorsVisibilityOff();
  this->ElementSetList->GetWidget()->SetHeight( 4 );
  
  
  // Add the columns 
  int col_index;
  col_index = this->ElementSetList->GetWidget()->AddColumn(NULL);
  this->ElementSetList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  this->ElementSetList->GetWidget()->SetColumnLabelImageToPredefinedIcon(
	  col_index, vtkKWIcon::IconEye);
	this->ElementSetList->GetWidget()->SetColumnWidth( col_index, 2);
	
	col_index = this->ElementSetList->GetWidget()->AddColumn("Element Set Name");
  this->ElementSetList->GetWidget()->SetColumnWidth( col_index, 30);
  //this->ElementSetList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
    this->ElementSetList->GetWidgetName());
  
      
  // Style Frame
  if (!this->StyleFrame)
  {
    this->StyleFrame = vtkKWFrameWithLabel::New();
  }
  this->StyleFrame->SetParent(this);
  this->StyleFrame->Create();
  this->StyleFrame->GetFrame()->SetReliefToGroove();
  this->StyleFrame->SetLabelText("Style");
  this->StyleFrame->AllowFrameToCollapseOn();
  //this->StyleFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->StyleFrame->GetWidgetName());  
      
  if(!this->DisplayStyleMenuButton)
  {
	  this->DisplayStyleMenuButton = vtkKWMenuButtonWithLabel::New();
  }
  this->DisplayStyleMenuButton->SetParent(this->StyleFrame->GetFrame());
  this->DisplayStyleMenuButton->Create();
  this->DisplayStyleMenuButton->SetLabelPositionToLeft();
  this->DisplayStyleMenuButton->SetLabelText("Representation: ");
  this->DisplayStyleMenuButton->SetLabelWidth(15);
  this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Solid + Wireframe",this, "RepresentationCallback 3");
	this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Wireframe",this, "RepresentationCallback 2");
  this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	  "Solid",this, "RepresentationCallback 1");
  this->DisplayStyleMenuButton->GetWidget()->SetValue("Solid + Wireframe");
	this->DisplayStyleMenuButton->SetEnabled(0);

  this->GetApplication()->Script( 
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->DisplayStyleMenuButton->GetWidgetName());
	
	if ( !this->ActorColorButton )
	  this->ActorColorButton = vtkKWChangeColorButton::New();
  this->ActorColorButton->SetParent(this->StyleFrame->GetFrame());
  this->ActorColorButton->Create();
  this->ActorColorButton->SetColor(0.5,0.5,0.5);
  this->ActorColorButton->SetLabelPositionToLeft();
  this->ActorColorButton->SetLabelText("Fill Color");
  this->ActorColorButton->SetCommand(this, "SetActorColor");
  this->ActorColorButton->SetEnabled(0);
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->ActorColorButton->GetWidgetName());
	
	if (!this->ElementSizeScale)
    this->ElementSizeScale = vtkKWSpinBoxWithLabel::New();
    
  this->ElementSizeScale->SetParent(this->StyleFrame->GetFrame());
  this->ElementSizeScale->Create();
  this->ElementSizeScale->GetWidget()->SetRange(0.0, 100.0);
  this->ElementSizeScale->GetWidget()->SetIncrement(1.0);
  this->ElementSizeScale->GetWidget()->SetValue(100.0);
  this->ElementSizeScale->GetWidget()->WrapOff();
  this->ElementSizeScale->SetLabelWidth(15);
  this->ElementSizeScale->GetLabel()->SetText("Element Size (%): ");
  this->ElementSizeScale->GetWidget()->SetCommand(this, "SetElementSizeFromScaleCallback");
  this->GetApplication()->Script( 
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->ElementSizeScale->GetWidgetName());
  this->ElementSizeScale->SetEnabled(0);
	
  if(!this->OpacityThumbwheel)
	  this->OpacityThumbwheel = vtkKWSpinBoxWithLabel::New();
  this->OpacityThumbwheel->SetParent(this->StyleFrame->GetFrame());
  this->OpacityThumbwheel->Create();
  this->OpacityThumbwheel->GetWidget()->SetRange(0.0, 1.0);
  this->OpacityThumbwheel->GetWidget()->SetIncrement(0.05);
  this->OpacityThumbwheel->GetWidget()->SetValueFormat("%3.2f");
  this->OpacityThumbwheel->GetWidget()->WrapOff();
  this->OpacityThumbwheel->SetLabelWidth(15);
  this->OpacityThumbwheel->GetLabel()->SetText("Opacity :");
  this->OpacityThumbwheel->GetWidget()->SetCommand(this, "OpacityCallback");
  this->GetApplication()->Script( 
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->OpacityThumbwheel->GetWidgetName());
  this->OpacityThumbwheel->SetEnabled(0);
  
	vtkKWSeparator *seperator = vtkKWSeparator::New();
	seperator->SetParent(this->StyleFrame->GetFrame());
  seperator->Create();
  seperator->SetOrientationToHorizontal();
  seperator->SetOrientationToHorizontal();
  this->GetApplication()->Script( 
    "pack %s -side top -anchor nw -expand n -padx 5 -pady 2 -fill x", 
	  seperator->GetWidgetName());
	  
	if (!this->OutlineColorButton)
    this->OutlineColorButton = vtkKWChangeColorButton::New();
  this->OutlineColorButton->SetParent(this->StyleFrame->GetFrame());
  this->OutlineColorButton->Create();
  this->OutlineColorButton->SetColor(0.5,0.5,0.5);
  this->OutlineColorButton->SetLabelPositionToLeft();
  this->OutlineColorButton->SetLabelText("Outline Color");
  this->OutlineColorButton->SetCommand(this, "SetOutlineColor");
  this->OutlineColorButton->SetEnabled(0);
  this->GetApplication()->Script( 
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
	  this->OutlineColorButton->GetWidgetName());
	    
	  
  if(!this->LinewidthThumbwheel)
	  this->LinewidthThumbwheel = vtkKWSpinBoxWithLabel::New();
  this->LinewidthThumbwheel->SetParent(this->StyleFrame->GetFrame());
  this->LinewidthThumbwheel->Create();
  this->LinewidthThumbwheel->GetWidget()->SetRange(0, 100);
  this->LinewidthThumbwheel->GetWidget()->SetIncrement(1);
  this->LinewidthThumbwheel->GetWidget()->SetValue(1);
 // this->LinewidthThumbwheel->GetWidget()->SetValueFormat("%3.2f");
  this->LinewidthThumbwheel->GetWidget()->WrapOff();
  this->LinewidthThumbwheel->GetLabel()->SetText("Line width:");
  this->LinewidthThumbwheel->SetLabelWidth(15);
  this->LinewidthThumbwheel->GetWidget()->SetCommand(this, "LineWidthCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->LinewidthThumbwheel->GetWidgetName());
  this->LinewidthThumbwheel->SetEnabled(0);

  /*
  if (!this->OverrideColorButton)
  {
	  this->OverrideColorButton = vtkKWCheckButtonWithLabel::New();
  }  
  this->OverrideColorButton->SetParent(this->ElementSizeFrame->GetFrame());
  this->OverrideColorButton->Create();
  this->OverrideColorButton->GetWidget()->SetCommand(this, "SetOverrideColorRangeFromButton");
  this->OverrideColorButton->GetWidget()->SetText("Manually Set Color Range");
  this->GetApplication()->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
	  this->OverrideColorButton->GetWidgetName());
  this->OverrideColorButton->SetEnabled(0);

  if (!this->ElementColorRange)
  {   
	  this->ElementColorRange  = vtkKWRange::New();
	  this->ElementColorRange->SetParent(this->ElementSizeFrame->GetFrame());
	  this->ElementColorRange->Create();
	  this->ElementColorRange->SetEnabled( 0 );
	  this->ElementColorRange->SetLabelText("A range:");
	  this->ElementColorRange->SetWholeRange(-10.0, 40.0);
	  this->ElementColorRange->SetRange(0.0, 10.0);
	  this->ElementColorRange->SetReliefToGroove();
	  this->ElementColorRange->SetBorderWidth(2);
	  this->ElementColorRange->SetPadX(2);
	  this->ElementColorRange->SetPadY(2);
	  this->ElementColorRange->SetCommand(this, "SetColorRangeCallback");
	  this->ElementColorRange->SetBalloonHelpString(
		  "Specify a subrange of element values here to control how elements are colored. ");
	  this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
		  this->ElementColorRange->GetWidgetName());
	  this->ElementColorRange->SetEnabled(0);
  }
  this->ElementSizeFrame->CollapseFrame();
  
  this->PlaneControlFrame = vtkKWFrameWithLabel::New();
  this->PlaneControlFrame->SetParent(this->ElementSizeFrame->GetFrame());
  this->PlaneControlFrame->Create();
  this->PlaneControlFrame->SetLabelText("Selection Plane Controls");
  this->GetApplication()->Script("pack %s -side top -anchor n -expand n -padx 2 -pady 2 -fill x",
	  this->PlaneControlFrame->GetWidgetName());  
 */
/*
  this->SelectionControls = vtkKWRadioButtonSet::New();
  this->SelectionControls->SetParent(PlaneControlFrame->GetFrame());
  this->SelectionControls->Create();
  this->SelectionControls->SetPadX(2);
  this->SelectionControls->SetPadY(2);
  this->SelectionControls->SetBorderWidth(2);
  this->SelectionControls->SetReliefToGroove();
  this->SelectionControls->ExpandWidgetsOn();
  this->SelectionControls->SetWidgetsPadY(1);
  this->SelectionControls->SetPackHorizontally(1);
  this->GetApplication()->Script("pack %s -expand n -fill x -padx 2 -pady 2", 
	  SelectionControls->GetWidgetName());

   if (!this->EnablePlaneButton)
  {
	  this->EnablePlaneButton = vtkKWCheckButton::New();
  }  
  this->EnablePlaneButton->SetParent(this->PlaneControlFrame->GetFrame());
  this->EnablePlaneButton->Create();
  this->EnablePlaneButton->SetText("Enable");
  this->EnablePlaneButton->SetCommand(this, "EnableSelectionPlaneCallback");
  this->EnablePlaneButton->SelectedStateOff();
  this->Script("pack %s -side left -expand n -padx 5 -pady 2",
	  this->EnablePlaneButton->GetWidgetName());
	*/  
  
  //pb = SelectionControls->AddWidget(2);
  //pb->SetText("Save Current Selection as New Mesh");
  //pb->SetCommand(this, "SaveSelectionMeshCallback");

  //pb = SelectionControls->AddWidget(2);
  //pb->SetText("Disable");
  //pb->SetCommand(this, "ClearSelectionPlaneCallback");
  //pb->SelectedStateOn();
/*
  if (!this->InvertSelector)
  {
	  this->InvertSelector = vtkKWCheckButton::New();
  }  
  this->InvertSelector->SetParent(this->PlaneControlFrame->GetFrame());
  this->InvertSelector->Create();
  this->InvertSelector->SetText("Invert Selection");
  this->InvertSelector->SetCommand(this, "SetInvertSelectionFromButton");
  this->Script("pack %s -side right -expand n -padx 5 -pady 2",
	  this->InvertSelector->GetWidgetName());
	  
  // cutting plane callbacks
  this->SavedPlaneWidget->SetInteractor(
	  this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
*/
  // register a callback that is invoked when the plane is moved or dragged, so the mesh quality
  // class can get the new positions and update the rendered actors appropriately.  A vtkPlane
  // instance is updated with the information from the plane widget.  Then the vtkPlane instance
  // can be passed to the quality filter so the implicit function can enable/disable element display
  // accordingly. 

/*
  this->PlaneMoveCallback = vtkPlaneWidgetEventCallback::New();
  this->PlaneMoveCallback->SetUserInterfacePointer(this->GetMimxMainWindow());
  this->PlaneMoveCallback->PlaneInstance = this->SavedImplicitPlane;
  this->SavedPlaneWidget->AddObserver(vtkCommand::InteractionEvent,PlaneMoveCallback);
  this->MeshQualityClass->SetCuttingPlaneFunction(this->SavedImplicitPlane);
  
  this->PlaneControlFrame->CollapseFrame();
*/  
  vtkKWIcon *closeIcon = vtkKWIcon::New();
  closeIcon->SetImage( image_mimxClose, 
                       image_mimxClose_width, 
                       image_mimxClose_height, 
                       image_mimxClose_pixel_size);
                             
  if(!this->CloseButton)
  {
    this->CloseButton = vtkKWPushButton::New();
  }
  this->CloseButton->SetParent(this);
  this->CloseButton->Create();
  //this->CloseButton->SetText("Close");
  this->CloseButton->SetImageToIcon( closeIcon );
  this->CloseButton->SetReliefToFlat( );
  this->CloseButton->SetCommand(this, "Withdraw");
  this->GetApplication()->Script(
	  "pack %s -side right -anchor ne -expand y -padx 2 -pady 6", 
	  this->CloseButton->GetWidgetName());

  //this->AddBinding("<Return>", this, "Withdraw");
  this->AddBinding("<Escape>", this, "Withdraw");
}
//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetActorColor(double color[3])
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
	{
  	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
      this->ObjectList->GetItem(row));
    if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
    {
      meshActor->SetMeshColor(this->ActorColorButton->GetColor());
    }
    else
    {
      vtkUnstructuredGrid *ugrid = meshActor->GetDataSet();
      vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
          ugrid->GetFieldData()->GetAbstractArray("Element_Set_Names"));
    	int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
    	if ( stringarray && (elementIndex >= 0) )
    	{
    	  std::string setName = stringarray->GetValue( elementIndex );
        meshActor->SetElementSetColor(setName, this->ActorColorButton->GetColor());
      }
    }
	}
	else if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_BUILDING_BLOCK)
	{
  	vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
      this->ObjectList->GetItem(row));   
  	ugridActor->SetFillColor( this->ActorColorButton->GetColor() );
	}
	else
	{
  	vtkMimxSurfacePolyDataActor *polyDataActor = vtkMimxSurfacePolyDataActor::SafeDownCast(
      this->ObjectList->GetItem(row));   
  	polyDataActor->SetFillColor( this->ActorColorButton->GetColor() );
	}
    	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetOutlineColor(double color[3])
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
	{
  	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
      this->ObjectList->GetItem(row));
    if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
    {
      meshActor->SetMeshOutlineColor(this->OutlineColorButton->GetColor());
    }
    else
    {
      vtkUnstructuredGrid *ugrid = meshActor->GetDataSet();
      vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
          ugrid->GetFieldData()->GetAbstractArray("Element_Set_Names"));
    	int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
    	//std::cout << "Set Index: " << elementIndex << std::endl;
    	if ( stringarray && (elementIndex >= 0) )
    	{
    	  std::string setName = stringarray->GetValue( elementIndex );
    	  //std::cout << "Set Name: " << setName << std::endl;
        meshActor->SetElementSetOutlineColor(setName, this->OutlineColorButton->GetColor());
      }
    }
	}
	else if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_BUILDING_BLOCK)
	{
  	vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
      this->ObjectList->GetItem(row));   
  	ugridActor->SetOutlineColor( this->OutlineColorButton->GetColor() );
	}
	else
	{
  	vtkMimxSurfacePolyDataActor *polyDataActor = vtkMimxSurfacePolyDataActor::SafeDownCast(
      this->ObjectList->GetItem(row));   
  	polyDataActor->SetOutlineColor( this->OutlineColorButton->GetColor() );
	}
    	
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetViewProperties()
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	this->SetTitle(this->ItemName);
	
  if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
  {
    /*
    this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s", 
		this->MirrorFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());
    */
    this->DisplayStyleMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
    this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	    "Solid + Wireframe",this, "RepresentationCallback 3");
	  this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	    "Wireframe",this, "RepresentationCallback 2");
    this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	    "Solid",this, "RepresentationCallback 1");
    vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(row));
    vtkUnstructuredGrid *ugrid = meshActor->GetDataSet();
    if (meshActor->GetNumberOfElementSets() == 0)
    {
      this->DisplayModeButton->SetEnabled( 0 );  
    }
    else
    {
      this->DisplayModeButton->SetEnabled( 1 );  
    }
		if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)	
		{
      this->ElementSetList->SetEnabled( 0 );
      this->DisplayModeButton->GetWidget()->SetValue("Mesh");
      this->SetMeshDisplayPropertyCallback();
    }
    else
    {
      this->ElementSetList->SetEnabled( 1 );
      this->DisplayModeButton->GetWidget()->SetValue("Element Sets");
      this->ElementSetDisplayPropertyCallback();
    }
		this->SetElementSetList();
		//this->MeshQualityClass->InitializeFromExternalMesh(ugrid);
		//this->UpdateNodeAndElementData();
		this->ElementSizeScale->SetEnabled(1);
  }
  else if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_BUILDING_BLOCK)
	{
  	this->ElementSetList->GetWidget()->DeleteAllRows();
    
    this->DisplayModeButton->SetEnabled( 0 );  
    this->ElementSetList->SetEnabled( 0 );
    
    vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
      this->ObjectList->GetItem(row));   
    double color[3];
    ugridActor->GetFillColor( color );
    this->ActorColorButton->SetColor(color);
    ugridActor->GetOutlineColor( color );
    this->OutlineColorButton->SetColor(color);

    this->DisplayStyleMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	  this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	    "Wireframe",this, "RepresentationCallback 2");
    this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	    "Surface",this, "RepresentationCallback 1");
	    
    int representation = ugridActor->GetDisplayType();
		if(representation == vtkMimxUnstructuredGridActor::DisplayOutline)
		{
			this->DisplayStyleMenuButton->GetWidget()->SetValue("Wireframe");
			this->LinewidthThumbwheel->GetWidget()->SetValue(this->ObjectList->GetItem(row)
  			->GetActor()->GetProperty()->GetLineWidth());
    	this->LinewidthThumbwheel->SetEnabled(1);
    	this->OutlineColorButton->SetEnabled(1);
    	this->ActorColorButton->SetEnabled(0); 
		}
		else
		{
			this->DisplayStyleMenuButton->GetWidget()->SetValue("Surface");
			this->LinewidthThumbwheel->SetEnabled(0);
			this->ActorColorButton->SetEnabled(1); 
			this->OutlineColorButton->SetEnabled(0);
		}
		this->DisplayStyleMenuButton->SetEnabled(1);
	    
		this->OpacityThumbwheel->GetWidget()->SetValue(this->ObjectList->GetItem(row)
			->GetActor()->GetProperty()->GetOpacity());
		this->OpacityThumbwheel->SetEnabled(1);
		this->ElementSizeScale->SetEnabled(0);
	}
  else
  {
    this->ElementSetList->GetWidget()->DeleteAllRows();
    
    this->DisplayModeButton->SetEnabled( 0 );  
    this->ElementSetList->SetEnabled( 0 );
    
    vtkMimxSurfacePolyDataActor *polyDataActor = vtkMimxSurfacePolyDataActor::SafeDownCast(
          this->ObjectList->GetItem(row));   
    double color[3];
    polyDataActor->GetFillColor( color );
    this->ActorColorButton->SetColor(color);
    polyDataActor->GetOutlineColor( color );
    this->OutlineColorButton->SetColor(color);
    
    
    this->DisplayStyleMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
	  this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	    "Wireframe",this, "RepresentationCallback 2");
    this->DisplayStyleMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	    "Surface",this, "RepresentationCallback 1");
	    
    int representation = polyDataActor->GetDisplayType();
		if (representation == vtkMimxSurfacePolyDataActor::DisplayOutline)
		{
			this->DisplayStyleMenuButton->GetWidget()->SetValue("Wireframe");
			this->LinewidthThumbwheel->GetWidget()->SetValue(this->ObjectList->GetItem(row)
  			->GetActor()->GetProperty()->GetLineWidth());
    	this->LinewidthThumbwheel->SetEnabled(1);
    	this->OutlineColorButton->SetEnabled(1);
    	this->ActorColorButton->SetEnabled(0);
		}
		else
		{
			this->DisplayStyleMenuButton->GetWidget()->SetValue("Surface");
			this->LinewidthThumbwheel->SetEnabled(0);
			this->ActorColorButton->SetEnabled(1);
			this->OutlineColorButton->SetEnabled(0);
		}
		this->DisplayStyleMenuButton->SetEnabled(1);
		this->OpacityThumbwheel->GetWidget()->SetValue(this->ObjectList->GetItem(row)
			->GetActor()->GetProperty()->GetOpacity());
		this->OpacityThumbwheel->SetEnabled(1);
		this->ElementSizeScale->SetEnabled(0);
	}
	
}

//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::RepresentationCallback(int type)
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	switch (type)
	{
	  case 1:
	    if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
      	this->DisplayStyleMenuButton->GetWidget()->SetValue("Solid");
    	else
    	  this->DisplayStyleMenuButton->GetWidget()->SetValue("Surface");
      this->OutlineColorButton->EnabledOff();
      this->ActorColorButton->EnabledOn();
      this->LinewidthThumbwheel->EnabledOff();
      this->OpacityThumbwheel->EnabledOn();
	    if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
    	{
      	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
          this->ObjectList->GetItem(row));
        if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
        {
          meshActor->SetMeshDisplayType(vtkMimxMeshActor::DisplaySurface);
        }
        else
        {
        	int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
        	if ( elementIndex >= 0 )
        	{
        	  std::string setName = this->ElementSetList->GetWidget()->GetCellText(elementIndex,1);
            meshActor->SetElementSetDisplayType(setName, vtkMimxMeshActor::DisplaySurface);
          }
        }
        this->ElementSizeScale->EnabledOn();
    	}
    	else if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_BUILDING_BLOCK)
    	{
      	vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
          this->ObjectList->GetItem(row));   
      	ugridActor->SetDisplayType( vtkMimxUnstructuredGridActor::DisplaySurface );
    	}
    	else
    	{
      	vtkMimxSurfacePolyDataActor *polyDataActor = vtkMimxSurfacePolyDataActor::SafeDownCast(
          this->ObjectList->GetItem(row));   
      	polyDataActor->SetDisplayType( vtkMimxSurfacePolyDataActor::DisplaySurface );
    	}
    	break;
	  case 2:
	    this->DisplayStyleMenuButton->GetWidget()->SetValue("Wireframe");
      this->OutlineColorButton->EnabledOn();
      this->ActorColorButton->EnabledOff();
      this->LinewidthThumbwheel->EnabledOn();
      this->OpacityThumbwheel->EnabledOff();
      this->ElementSizeScale->EnabledOff();
	    if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
    	{
      	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
          this->ObjectList->GetItem(row));
        if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
        {
          meshActor->SetMeshDisplayType(vtkMimxMeshActor::DisplayOutline);
        }
        else
        {
          int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
        	if ( elementIndex >= 0 )
        	{
        	  std::string setName = this->ElementSetList->GetWidget()->GetCellText(elementIndex,1);
            meshActor->SetElementSetDisplayType(setName, vtkMimxMeshActor::DisplayOutline);
          }
        }      
    	}
    	else if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_BUILDING_BLOCK)
    	{
      	vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
          this->ObjectList->GetItem(row));   
      	ugridActor->SetDisplayType( vtkMimxUnstructuredGridActor::DisplayOutline );
    	}
    	else
    	{
      	vtkMimxSurfacePolyDataActor *polyDataActor = vtkMimxSurfacePolyDataActor::SafeDownCast(
          this->ObjectList->GetItem(row));   
      	polyDataActor->SetDisplayType( vtkMimxSurfacePolyDataActor::DisplayOutline );
    	}
    	break;
	  case 3:
	    if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
  	    this->DisplayStyleMenuButton->GetWidget()->SetValue("Solid + Wireframe");
      else
        this->DisplayStyleMenuButton->GetWidget()->SetValue("Surface + Wireframe");
      this->OutlineColorButton->EnabledOn();
      this->ActorColorButton->EnabledOn();
      this->LinewidthThumbwheel->EnabledOn();
      this->OpacityThumbwheel->EnabledOn();
      
	    if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
    	{
      	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
          this->ObjectList->GetItem(row));
        if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
        {
          meshActor->SetMeshDisplayType(vtkMimxMeshActor::DisplaySurfaceAndOutline);
        }
        else
        {
          int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
        	if ( elementIndex >= 0 )
        	{
        	  std::string setName = this->ElementSetList->GetWidget()->GetCellText(elementIndex,1);
            meshActor->SetElementSetDisplayType(setName, vtkMimxMeshActor::DisplaySurfaceAndOutline);
          }
        } 
        this->ElementSizeScale->EnabledOn();       
    	}
    	break;
	}
	
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::LineWidthCallback(double width)
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
  if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
  {
    vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
      this->ObjectList->GetItem(row));
    if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
    {
      meshActor->SetMeshOutlineRadius( width );
    }
    else
    {
      int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
    	if ( elementIndex >= 0 )
    	{
    	  std::string setName = this->ElementSetList->GetWidget()->GetCellText(elementIndex,1);
        meshActor->SetElementSetOutlineRadius(setName, width);
      }
    }
    
  }
  else
  {
    this->ObjectList->GetItem(row)->GetActor()->GetProperty()->SetLineWidth(width);
	this->ObjectList->GetItem(row)->GetActor()->Modified();
  }
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::OpacityCallback(double opacity)
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
  {
    vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
      this->ObjectList->GetItem(row));
    if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
    {
      meshActor->SetMeshOpacity( opacity );
    }
    else
    {
      int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
    	if ( elementIndex >= 0 )
    	{
    	  std::string setName = this->ElementSetList->GetWidget()->GetCellText(elementIndex,1);
        meshActor->SetElementSetOpacity(setName, opacity);
      }
    }
  }
  else
  {
    this->ObjectList->GetItem(row)->GetActor()->GetProperty()->SetOpacity(opacity);
  }
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
/*
void vtkKWMimxViewPropertiesGroup::UpdateNodeAndElementData()
{
	int row = this->SelectionRow;
	
	
	vtkUnstructuredGrid *ugrid;
	if(this->ObjectList->GetItem(rows[0])->GetDataType() == ACTOR_FE_MESH)
	{
		ugrid = vtkMimxMeshActor::SafeDownCast(
			this->ObjectList->GetItem(rows[0]))->GetDataSet();
	}
	
	int numarrays = ugrid->GetCellData()->GetNumberOfArrays();
	if(numarrays)
	{
		for(i=0; i<numarrays; i++)
		{
			const char * arrayname = ugrid->GetCellData()->GetArrayName(i);
			if(!strcmp(arrayname,"Volume"))
			{
				this->ElementDataMenuButton->GetWidget()->GetMenu()->AddRadioButton(
					"Volume",this, "VolumePlotCallback");
				this->ElementDataMenuButton->SetEnabled(1);
			}
		}
	}
	else
	{
		this->ElementDataMenuButton->SetEnabled(0);
	}
	
}
*/
//-------------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetElementSetColor(double color[3])
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	this->ObjectList->GetItem(row)->GetActor()->GetProperty()
		->SetColor(this->ActorColorButton->GetColor());
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//----------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetElementSizeFromScaleCallback(double value)
{
	double shrinkFactor = value / 100.0;
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
	{
		vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
      this->ObjectList->GetItem(row));
    if (meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayMesh)
    {
      meshActor->SetMeshShrinkFactor( shrinkFactor );
    }
    else
    {
      int elementIndex = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
    	if ( elementIndex >= 0 )
    	{
    	  std::string setName = this->ElementSetList->GetWidget()->GetCellText(elementIndex,1);
        meshActor->SetElementSetShrinkFactor(setName, shrinkFactor);
      }
    }  
	}
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}


//----------------------------------------------------------------------------
/*
void vtkKWMimxViewPropertiesGroup::SetOverrideColorRangeFromButton(int state)
{
	double range[2];

	this->MeshQualityClass->SetOverrideMeshColorRange(state);

	this->ElementColorRange->SetEnabled( state );

	if (state)
	{
		// user enabling manual control.  Read the range and update
		// the interface immediately
		this->ElementColorRange->GetRange(range);
		this->SetColorRangeCallback(range[0],range[1]);
	}
	else 
	{
		// called to update the pipeline back to the default colors
		this->MeshQualityClass->UpdatePipeline();
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}
}
//--------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetColorRangeCallback(double low, double high)
{
	this->MeshQualityClass->SetMeshColorRange(low,high);
	this->MeshQualityClass->UpdatePipeline();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//---------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::EnableSelectionPlaneCallback(int state)
{
	if ( state )
	{
	  this->SavedPlaneWidget->SetInput(this->MeshQualityClass->GetInitialMesh());
	  this->SavedPlaneWidget->PlaceWidget();
	  this->SavedPlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
		  ->GetRenderWindowInteractor());
	  this->SavedPlaneWidget->SetEnabled(1);
	  this->MeshQualityClass->EnableCuttingPlane();

	  this->MeshQualityClass->UpdatePipeline();
	  this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}
	else
	{
	  this->SavedPlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
		  ->GetRenderWindowInteractor());
	  this->SavedPlaneWidget->SetEnabled(0);
	  this->MeshQualityClass->DisableCuttingPlane();
	  this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}
}
//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::ClearSelectionPlaneCallback()
{
	this->SavedPlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
		->GetRenderWindowInteractor());
	this->SavedPlaneWidget->SetEnabled(0);
	this->MeshQualityClass->DisableCuttingPlane();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//-----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetInvertSelectionFromButton(int state)
{
	//cout << "invert button callback " << endl;
	this->MeshQualityClass->SetInvertCuttingPlaneSelection(state);
	this->MeshQualityClass->UpdatePipeline();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
*/
//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::DisplayModeCallback(int mode)
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
        this->ObjectList->GetItem(row));
	switch ( mode )
	{
	  case 1:
	    this->DisplayModeButton->GetWidget()->SetValue("Mesh");
	    this->ElementSetList->EnabledOff();
  		this->SetElementSetList();
	    meshActor->SetDisplayMode(vtkMimxMeshActor::DisplayMesh);
	    this->ElementSetList->GetWidget()->SetSelectionCommand(NULL, NULL);
	    this->SetMeshDisplayPropertyCallback( );
	    break;
	  case 2:
	    this->DisplayModeButton->GetWidget()->SetValue("Element Sets");
	    this->ElementSetList->EnabledOn();
	    this->SetElementSetList();  
      meshActor->SetDisplayMode(vtkMimxMeshActor::DisplayElementSets);
      this->ElementSetList->GetWidget()->SetSelectionCommand(this, "ElementSetDisplayPropertyCallback");
      this->ElementSetDisplayPropertyCallback();
	    break;
	}
	
}
//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::ElementSetVisibilityCallback(int index, int flag)
{
	int row = this->GetItemRow(); /*this->SelectionRow;*/
	
	if (this->ObjectList->GetItem(row)->GetDataType() == ACTOR_FE_MESH)
	{
  	if ( index >= 0 )
  	{
  	  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
        this->ObjectList->GetItem(row));
                
  	  std::string setName = this->ElementSetList->GetWidget()->GetCellText(index,1);
      if ( flag ) 
        meshActor->ShowElementSet( setName );
  	  else 
  	    meshActor->HideElementSet( setName );
    }
	}
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::ElementSetDisplayPropertyCallback( )
{
	int row = this->ElementSetList->GetWidget()->GetIndexOfFirstSelectedRow();
	if ( row < 0 )
	  return;
	
	std::string setName = this->ElementSetList->GetWidget()->GetCellText(row, 1);
	
	int meshIndex = this->GetItemRow(); /*this->SelectionRow;*/
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(meshIndex));
	int representation = meshActor->GetElementSetDisplayType( setName );
  if (representation == vtkMimxMeshActor::DisplaySurface)
  {
    this->DisplayStyleMenuButton->GetWidget()->SetValue("Solid");
    this->OutlineColorButton->EnabledOff();
    this->ActorColorButton->EnabledOn();
    this->LinewidthThumbwheel->EnabledOff();
    this->OpacityThumbwheel->EnabledOn();
    this->ElementSizeScale->EnabledOn();
  }
  else if (representation == vtkMimxMeshActor::DisplayOutline)
  {
    this->DisplayStyleMenuButton->GetWidget()->SetValue("Wireframe");
    this->OutlineColorButton->EnabledOn();
    this->ActorColorButton->EnabledOff();
    this->LinewidthThumbwheel->EnabledOn();
    this->OpacityThumbwheel->EnabledOff();
    this->ElementSizeScale->EnabledOff();
  }
  else
  {
    this->DisplayStyleMenuButton->GetWidget()->SetValue("Solid + Wireframe");
    this->OutlineColorButton->EnabledOn();
    this->ActorColorButton->EnabledOn();
    this->LinewidthThumbwheel->EnabledOn();
    this->OpacityThumbwheel->EnabledOn();
    this->ElementSizeScale->EnabledOn();
  }
  
  this->OpacityThumbwheel->GetWidget()->SetValue(meshActor->GetElementSetOpacity( setName ));
	this->ElementSizeScale->GetWidget()->SetValue(meshActor->GetElementSetShrinkFactor( setName )*100.0);
	
  this->LinewidthThumbwheel->GetWidget()->SetValue(meshActor->GetElementSetOutlineRadius( setName )*100.0);
	
	double rgb[3];
	meshActor->GetElementSetColor( setName, rgb );
	this->ActorColorButton->SetColor(rgb);
	meshActor->GetElementSetOutlineColor( setName, rgb );
	this->OutlineColorButton->SetColor(rgb);	
}

//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetMeshDisplayPropertyCallback( )
{
  int row = this->GetItemRow(); /*this->SelectionRow;*/
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(row));
	
  if (this->inializedElementSetList == false)
  {
    this->SetElementSetList();
    this->SetElementSetList();
  }
  
  this->DisplayStyleMenuButton->EnabledOn();
  this->ElementSetList->EnabledOff();
  
  int representation = meshActor->GetMeshDisplayType();  
  if (representation == vtkMimxMeshActor::DisplaySurface)
  {
    this->DisplayStyleMenuButton->GetWidget()->SetValue("Solid");
    this->OutlineColorButton->EnabledOff();
    this->ActorColorButton->EnabledOn();
    this->LinewidthThumbwheel->EnabledOff();
    this->OpacityThumbwheel->EnabledOn();
    this->ElementSizeScale->EnabledOn();
  }
  else if (representation == vtkMimxMeshActor::DisplayOutline)
  {
    this->DisplayStyleMenuButton->GetWidget()->SetValue("Wireframe");
    this->OutlineColorButton->EnabledOn();
    this->ActorColorButton->EnabledOff();
    this->LinewidthThumbwheel->EnabledOn();
    this->OpacityThumbwheel->EnabledOff();
    this->ElementSizeScale->EnabledOff();
  }
  else
  {
    this->DisplayStyleMenuButton->GetWidget()->SetValue("Solid + Wireframe");
    this->OutlineColorButton->EnabledOn();
    this->ActorColorButton->EnabledOn();
    this->LinewidthThumbwheel->EnabledOn();
    this->OpacityThumbwheel->EnabledOn();
    this->ElementSizeScale->EnabledOn();
  }
  
  this->OpacityThumbwheel->GetWidget()->SetValue(meshActor->GetMeshOpacity());
  this->ElementSizeScale->GetWidget()->SetValue(meshActor->GetMeshShrinkFactor()*100.0);
	this->LinewidthThumbwheel->GetWidget()->SetValue(meshActor->GetMeshOutlineRadius()*100.0);
	
	double rgb[3];
	meshActor->GetMeshColor( rgb );
	this->ActorColorButton->SetColor(rgb);
	meshActor->GetMeshOutlineColor( rgb );
	this->OutlineColorButton->SetColor(rgb);
}

//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetElementSetList( )
{
  int row = this->GetItemRow(); /*this->SelectionRow;*/
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->ObjectList->GetItem(row));
  vtkUnstructuredGrid *ugrid = meshActor->GetDataSet();
  vtkStringArray *stringarray = vtkStringArray::SafeDownCast(
		ugrid->GetFieldData()->GetAbstractArray("Element_Set_Names"));
	
	this->ElementSetList->GetWidget()->DeleteAllRows();
	if ( stringarray )
	{
	  for (int i=0;i<stringarray->GetNumberOfTuples();i++)
	  {
	    std::string setName = stringarray->GetValue(i);
	    int isVisible = meshActor->GetElementSetVisibility( setName );
	    this->ElementSetList->GetWidget()->InsertCellTextAsInt(i, 0, isVisible);
      this->ElementSetList->GetWidget()->SetCellWindowCommandToCheckButton( i, 0);
      char command[64];
      sprintf(command, "ElementSetVisibilityCallback %d", i);
    	this->ElementSetList->GetWidget()->GetCellWindowAsCheckButton(i, 0)->SetCommand(this,command);
    	this->ElementSetList->GetWidget()->InsertCellText(i, 1, setName.c_str());
      this->ElementSetList->GetWidget()->GetCellWindowAsCheckButton(i, 0)->SetCommand(this,command);
	  }
	  if (stringarray->GetNumberOfTuples() > 0 )
	  {
	    this->ElementSetList->GetWidget()->SelectRow( 0 );
	    this->DisplayModeButton->SetEnabled( 1 );  
	    //this->ElementSetList->GetWidget()->SetCellTextAsInt(0, 0, 1);
	  }
	}
	this->inializedElementSetList = true;
}  
//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetItemName( const char *item )
{
  strcpy(this->ItemName, item);
}  	

//----------------------------------------------------------------------------------------------
void vtkKWMimxViewPropertiesGroup::SetItemId( const char *id )
{
  strcpy(this->ItemId, id);
}  

//----------------------------------------------------------------------------------------------
int vtkKWMimxViewPropertiesGroup::GetItemRow( )
{
  for (int i=0;i<this->ObjectList->GetNumberOfItems(); i++)
	{
		if ( strcmp(this->ItemId, this->ObjectList->GetItem(i)->GetUniqueId()) == 0 )
		{
			return i;
		}
	}
	return 0;
}  			
//-----------------------------------------------------------------------------------------------
