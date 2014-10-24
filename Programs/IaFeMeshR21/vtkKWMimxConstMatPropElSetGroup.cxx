/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxConstMatPropElSetGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/08/14 05:01:52 $
Version:   $Revision: 1.35 $

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

#include "vtkKWMimxConstMatPropElSetGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxDefineElSetGroup.h"

#include "vtkLinkedListWrapper.h"

#include "vtkActor.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkMimxMeshActor.h"
#include "vtkUnstructuredGrid.h"

#include "vtkKWApplication.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWOptions.h"
#include "vtkKWPushButton.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkObjectFactory.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWComboBox.h"
#include "vtkKWPushButton.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkFieldData.h"
#include "vtkFloatArray.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWCheckButton.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkRenderer.h"
#include "vtkKWIcon.h"
#include "vtkKWMimxViewPropertiesOptionGroup.h"


#include "Resources/mimxElementSetDfns.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxConstMatPropElSetGroup);
vtkCxxRevisionMacro(vtkKWMimxConstMatPropElSetGroup, "$Revision: 1.35 $");

//----------------------------------------------------------------------------
vtkKWMimxConstMatPropElSetGroup::vtkKWMimxConstMatPropElSetGroup()
{
  this->ObjectListComboBox = NULL;
  this->ElementSetComboBox = NULL;
  this->YoungsModulusEntry = NULL;
  this->PoissonsRatioEntry = NULL;
  //this->ViewFrame = NULL;
  //this->ViewPropertyButton = NULL;
  //this->ViewLegendButton = NULL;
  this->ComponentFrame = NULL;
  //this->ClippingPlaneMenuButton = NULL;
  //this->ScalarBarRangeComputationButton = NULL;
  //this->ViewElementSetMenuButton = NULL;
  this->DefineElSetButton = NULL;
  this->DefineElementSetDialog = NULL;
  this->ViewOptionsButton = NULL;
  this->ViewOptionsGroup = NULL;
  strcpy(this->elementSetSelectionPrevious,"");
}

//----------------------------------------------------------------------------
vtkKWMimxConstMatPropElSetGroup::~vtkKWMimxConstMatPropElSetGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->ElementSetComboBox)
	  this->ElementSetComboBox->Delete();
  if(this->YoungsModulusEntry)
	  this->YoungsModulusEntry->Delete();
  if(this->PoissonsRatioEntry)
	  this->PoissonsRatioEntry->Delete();
	//if(this->ViewFrame)
	//  this->ViewFrame->Delete();  
	//if(this->ViewPropertyButton)
	//  this->ViewPropertyButton->Delete();
  //if(this->ViewLegendButton)
	//  this->ViewLegendButton->Delete();
  //if(this->ClippingPlaneMenuButton)
	//  this->ClippingPlaneMenuButton->Delete();
  //if(this->ScalarBarRangeComputationButton)
	//  this->ScalarBarRangeComputationButton->Delete();
  //if(this->ViewElementSetMenuButton)
	//  this->ViewElementSetMenuButton->Delete();
	if (this->ComponentFrame)
	  this->ComponentFrame->Delete();
	if (this->DefineElSetButton)
	  this->DefineElSetButton->Delete();
	if (this->DefineElementSetDialog)
	  this->DefineElementSetDialog->Delete();
	if(this->ViewOptionsButton)
		this->ViewOptionsButton->Delete();
	if(this->ViewOptionsGroup)
		this->ViewOptionsGroup->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::CreateWidget()
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
  //this->MainFrame->SetLabelText("Constant Material Properties");

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 0 -fill x", 
    this->MainFrame->GetWidgetName());

  if ( !this->ComponentFrame )
    this->ComponentFrame = vtkKWFrameWithLabel::New();
  this->ComponentFrame->SetParent( this->MainFrame );
  this->ComponentFrame->Create();
  this->ComponentFrame->SetLabelText("Mesh");
  this->ComponentFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ComponentFrame->GetWidgetName());
    
  ObjectListComboBox->SetParent(this->ComponentFrame->GetFrame());
  ObjectListComboBox->Create();
  ObjectListComboBox->SetLabelText("Mesh : ");
  ObjectListComboBox->SetLabelWidth(15);
  ObjectListComboBox->GetWidget()->ReadOnlyOn();
  ObjectListComboBox->GetWidget()->SetCommand(this, "SelectionChangedCallback");
  ObjectListComboBox->GetWidget()->SetBalloonHelpString(
	  "List of Finite element meshes containing material property information");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ObjectListComboBox->GetWidgetName());

  vtkKWIcon *defineElSetIcon = vtkKWIcon::New();
  defineElSetIcon->SetImage(    image_mimxElementSetDfns, 
                                image_mimxElementSetDfns_width, 
                                image_mimxElementSetDfns_height, 
                                image_mimxElementSetDfns_pixel_size); 
                          
  if ( !this->DefineElSetButton )
    this->DefineElSetButton = vtkKWPushButton::New();
  this->DefineElSetButton->SetParent(this->MainFrame);
  this->DefineElSetButton->Create();
  this->DefineElSetButton->SetCommand(this, "DefineElementSetCallback");
  this->DefineElSetButton->SetImageToIcon( defineElSetIcon );
  this->DefineElSetButton->SetReliefToFlat( );
  this->GetApplication()->Script(
        "pack %s -side top -anchor n -padx 2 -pady 6", 
        this->DefineElSetButton->GetWidgetName());
/*        
  if ( !this->ViewOptionsButton )
	  this->ViewOptionsButton = vtkKWPushButton::New();
  this->ViewOptionsButton->SetParent(this->MainFrame);
  this->ViewOptionsButton->Create();
  this->ViewOptionsButton->SetCommand(this, "ViewOptionsCallback");
  this->ViewOptionsButton->SetText("View Options");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor n -padx 2 -pady 6", 
	  this->ViewOptionsButton->GetWidgetName());
*/
  // for element set listing
  if ( !this->ElementSetComboBox )	
	  this->ElementSetComboBox = vtkKWComboBoxWithLabel::New();
  ElementSetComboBox->SetParent(this->MainFrame);
  ElementSetComboBox->Create();
  ElementSetComboBox->SetLabelText("Element Set : ");
  ElementSetComboBox->SetLabelWidth(15);
  ElementSetComboBox->GetWidget()->ReadOnlyOn();
  this->ElementSetComboBox->GetWidget()->SetCommand(this, "ElementSetChangedCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  ElementSetComboBox->GetWidgetName());

  //Young's modulus
  if (!this->YoungsModulusEntry)
	  this->YoungsModulusEntry = vtkKWEntryWithLabel::New();
  this->YoungsModulusEntry->SetParent(this->MainFrame);
  this->YoungsModulusEntry->Create();
  this->YoungsModulusEntry->SetLabelWidth(15);
  this->YoungsModulusEntry->SetLabelText("Modulus : ");
  this->YoungsModulusEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->YoungsModulusEntry->GetWidgetName());

  // Poisson's ratio
  if (!this->PoissonsRatioEntry)
	  this->PoissonsRatioEntry = vtkKWEntryWithLabel::New();
  this->PoissonsRatioEntry->SetParent(this->MainFrame);
  this->PoissonsRatioEntry->Create();
  this->PoissonsRatioEntry->SetLabelWidth(15);
  this->PoissonsRatioEntry->SetLabelText("Poisson's Ratio : ");
  this->PoissonsRatioEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->PoissonsRatioEntry->GetWidgetName());

/*
  if (!this->ViewFrame)
    this->ViewFrame = vtkKWFrameWithLabel::New();
  this->ViewFrame->SetParent( this->MainFrame->GetFrame() );
  this->ViewFrame->Create();
  this->ViewFrame->SetLabelText("View");
  this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
              this->ViewFrame->GetWidgetName() );    
  this->ViewFrame->CollapseFrame();
*/  
  this->ApplyButton->SetParent(this->MainFrame);
  this->ApplyButton->Create();
  this->ApplyButton->SetReliefToFlat();
  this->ApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
  //this->ApplyButton->SetText("Apply");
  this->ApplyButton->SetCommand(this, "ConstMatPropElSetApplyCallback");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand y -padx 5 -pady 2", 
	  this->ApplyButton->GetWidgetName());

  this->CancelButton->SetParent(this->MainFrame);
  this->CancelButton->Create();
  this->CancelButton->SetReliefToFlat();
  this->CancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
  //this->CancelButton->SetText("Cancel");
  this->CancelButton->SetCommand(this, "ConstMatPropElSetCancelCallback");
  this->GetApplication()->Script(
    "pack %s -side right -anchor ne -expand y -padx 5 -pady 2", 
    this->CancelButton->GetWidgetName());

	  
/*    
  if (!this->ViewPropertyButton)
    this->ViewPropertyButton = vtkKWCheckButtonWithLabel::New();
  this->ViewPropertyButton->SetParent(this->ViewFrame->GetFrame());
  this->ViewPropertyButton->Create();
  this->ViewPropertyButton->GetWidget()->SetCommand(this, "ViewMaterialPropertyCallback");
  this->ViewPropertyButton->GetWidget()->SetText("View Properties");
 // this->ViewPropertyButton->GetWidget()->SetEnabled( 0 );
  this->GetApplication()->Script(
        "grid %s -row 0 -column 0 -sticky nw -padx 2 -pady 2", 
        this->ViewPropertyButton->GetWidgetName());

  if (!this->ViewLegendButton)
    this->ViewLegendButton = vtkKWCheckButtonWithLabel::New();
  this->ViewLegendButton->SetParent(this->ViewFrame->GetFrame());
  this->ViewLegendButton->Create();
  this->ViewLegendButton->GetWidget()->SetCommand(this, "ViewPropertyLegendCallback");
  this->ViewLegendButton->GetWidget()->SetText("View Legend");
//  this->ViewLegendButton->GetWidget()->SetEnabled( 0 );
  this->GetApplication()->Script(
        "grid %s -row 0 -column 1 -sticky nw -padx 2 -pady 2", 
        this->ViewLegendButton->GetWidgetName());

  if (!this->ViewElementSetMenuButton)
	  this->ViewElementSetMenuButton = vtkKWCheckButtonWithLabel::New();
  this->ViewElementSetMenuButton->SetParent(this->ViewFrame->GetFrame());
  this->ViewElementSetMenuButton->Create();
  this->ViewElementSetMenuButton->GetWidget()->SetCommand(this, "ViewElementSetCallback");
  this->ViewElementSetMenuButton->GetWidget()->SetText("Display Element Set");
  this->ViewElementSetMenuButton->
	  GetWidget()->SetBalloonHelpString("View either a selected element set of the Complete mesh");
  //  this->ViewLegendButton->GetWidget()->SetEnabled( 0 );
  this->GetApplication()->Script(
	  "grid %s -row 1 -column 0 -sticky nw -padx 2 -pady 2", 
	  this->ViewElementSetMenuButton->GetWidgetName());

  if (!this->ScalarBarRangeComputationButton)
    this->ScalarBarRangeComputationButton = vtkKWCheckButtonWithLabel::New();
  this->ScalarBarRangeComputationButton->SetParent(this->ViewFrame->GetFrame());
  this->ScalarBarRangeComputationButton->Create();
  this->ScalarBarRangeComputationButton->GetWidget()->SetCommand(this, "ScalarBarRangeComputationCallback");
  this->ScalarBarRangeComputationButton->GetWidget()->SetText("Element Set Range");
  this->ScalarBarRangeComputationButton->
	  GetWidget()->SetBalloonHelpString("If selected Element set range is used for display else range based on Complete mesh is used");
  this->ScalarBarRangeComputationButton->GetWidget()->SetStateToDisabled();
  this->GetApplication()->Script(
        "grid %s -row 1 -column 1 -sticky nw -padx 2 -pady 2", 
        this->ScalarBarRangeComputationButton->GetWidgetName());

  if(!this->ClippingPlaneMenuButton)	
		this->ClippingPlaneMenuButton = vtkKWMenuButtonWithLabel::New();
	this->ClippingPlaneMenuButton->SetParent(this->ViewFrame->GetFrame());
	this->ClippingPlaneMenuButton->Create();
	this->ClippingPlaneMenuButton->SetBorderWidth(0);
	this->ClippingPlaneMenuButton->SetReliefToGroove();
//	this->ClippingPlaneMenuButton->GetWidget()->SetEnabled( 0 );
	this->ClippingPlaneMenuButton->SetLabelText("Clipping Plane :");
	this->GetApplication()->Script(
	  "grid %s -row 2 -column 0 -sticky nw -padx 2 -pady 2", 
		this->ClippingPlaneMenuButton->GetWidgetName());
	this->ClippingPlaneMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Off",this, "ClippingPlaneCallback 1");
	this->ClippingPlaneMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"On",this, "ClippingPlaneCallback 2");
  this->ClippingPlaneMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Invert",this, "ClippingPlaneCallback 3");
	this->ClippingPlaneMenuButton->GetWidget()->SetValue("Off");
*/	  

}
//----------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxConstMatPropElSetGroup::ConstMatPropElSetApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("FE Mesh selection required");
		return 0;
	}

  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  strcpy(this->meshName, name);
  
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid FE Mesh");
		combobox->SetValue("");
		return 0;
	}

	float youngsmodulus = this->YoungsModulusEntry->GetWidget()->GetValueAsDouble();
	float poissonsratio = this->PoissonsRatioEntry->GetWidget()->GetValueAsDouble();

	if(youngsmodulus < 0)
	{
		callback->ErrorMessage("Young's Modulus cannot be < 0");
		return 0;
	}

	if(poissonsratio < -1.0)
	{
		callback->ErrorMessage("poissons ratio value should be >= -1.0");
		return 0;
	}
	
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex(name)));
  vtkUnstructuredGrid *ugrid = vtkMimxMeshActor::SafeDownCast(
	  this->FEMeshList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
	
  const char *elementsetname = this->ElementSetComboBox->GetWidget()->GetValue();
  strcpy(this->elementSetName, elementsetname);
  
  if(!strcmp(elementsetname,""))
  {
	  callback->ErrorMessage("Choose valid element set name");
	  return 0;
  }
  
  /* If both Fields are empty then clear out the values */
  std::string youngStr = this->YoungsModulusEntry->GetWidget()->GetValue();
  std::string poissonStr = this->PoissonsRatioEntry->GetWidget()->GetValue();
  if ((youngStr == "") && (poissonStr == ""))
  {
	  std::string propName = this->elementSetName;
	  propName += "_Constant_Youngs_Modulus";
	  ugrid->GetFieldData()->RemoveArray( propName.c_str() );

	  vtkIntArray *cellArray = vtkIntArray::SafeDownCast(ugrid->GetCellData()->GetArray(this->elementSetName));
	  vtkDoubleArray *youngsmodulus = vtkDoubleArray::SafeDownCast(ugrid->GetCellData()->GetArray("Youngs_Modulus"));	
	  if (cellArray && youngsmodulus)
	  {
		  int numCells = ugrid->GetNumberOfCells();
		  for (int i=0; i< numCells; i++)
		  {
			if (cellArray->GetValue(i))
			{
			  youngsmodulus->SetValue(i, -9999);
			}
		  }
	  }

	  propName = this->elementSetName;
	  propName += "_Constant_Poissons_Ratio";
	  ugrid->GetFieldData()->RemoveArray( propName.c_str() );

	  this->YoungsModulusEntry->SetEnabled( 0 );
	  this->PoissonsRatioEntry->SetEnabled( 0 );
	  this->GetMimxMainWindow()->SetStatusText("Cleared user defined material properties");

	  return 1;
  }

  std::string checkPropName;
  checkPropName = this->elementSetName;
  checkPropName += "_Image_Based_Material_Property";
  
  vtkDoubleArray *matarray = vtkDoubleArray::SafeDownCast( ugrid->GetCellData()->GetArray(checkPropName.c_str()) );
	if( matarray )
	{
		vtkKWMessageDialog *Dialog = vtkKWMessageDialog::New();
		Dialog->SetStyleToYesNo();
		Dialog->SetApplication(this->GetApplication());
		Dialog->Create();
		Dialog->SetTitle("Your Attention Please!");
		Dialog->SetText("An image-based material property has already been assigned to this element set. Would you like to overwrite?");
		Dialog->Invoke();
		if(Dialog->GetStatus() == vtkKWMessageDialog::StatusCanceled)
		{
			Dialog->Delete();
			return 1;
		}
		Dialog->Delete();
		ugrid->GetCellData()->RemoveArray( checkPropName.c_str() );
	}
	

//  meshActor->GenerateElementSetMapperLookUpTableFromMeshRange(elementsetname, "Youngs_Modulus");
  //char imagebased[256];
  //strcpy(imagebased, elementsetname);
  //strcat(imagebased, "_Image_Based_Material_Property");

  //vtkDoubleArray *matarray = vtkDoubleArray::SafeDownCast(
	 // ugrid->GetCellData()->GetArray(imagebased));
  //if(matarray)
  //{
	 // vtkKWMessageDialog *Dialog = vtkKWMessageDialog::New();
	 // Dialog->SetStyleToYesNo();
	 // Dialog->SetApplication(this->GetApplication());
	 // Dialog->Create();
	 // Dialog->SetTitle("Your attention please!");
	 // Dialog->SetText("Image based material property already exists. Would you like to overwrite ?");
	 // Dialog->Invoke();
	 // if(Dialog->GetStatus() == vtkKWMessageDialog::StatusCanceled)
	 // {
		//  meshActor->StoreConstantPoissonsRatio(elementsetname, poissonsratio);
		//  Dialog->Delete();
		//  this->GetMimxMainWindow()->SetStatusText("Assigned Poissons Ratio");	  
		//  return 1;
	 // }
	 // else
	 // {
		//  Dialog->Delete();
	 // }
  //}
  //
  meshActor->StoreConstantMaterialProperty(elementsetname, youngsmodulus);
  meshActor->StoreConstantPoissonsRatio(elementsetname, poissonsratio);
  //int viewState = this->ViewPropertyButton->GetWidget()->GetSelectedState();
  //if (viewState)	this->ViewPropertyButton->GetWidget()->SetSelectedState(0);
  //this->ViewPropertyButton->GetWidget()->SetSelectedState(1);
  //this->MimxMainWindow->GetRenderWidget()->Render();
  this->YoungsModulusEntry->SetEnabled( 0 );
  this->PoissonsRatioEntry->SetEnabled( 0 );
  this->GetMimxMainWindow()->SetStatusText("Assigned user defined material properties");	  

  return 1;
}
//----------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::ConstMatPropElSetCancelCallback()
{
	this->CancelStatus = 1;
	if(this->ViewOptionsGroup)
	{
		this->ViewOptionsGroup->DeselectAllButtons();
		this->ViewOptionsGroup->Withdraw();
	}
    this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName());
    this->MenuGroup->SetMenuButtonsEnabled(1);
    this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
	//this->ViewPropertyButton->GetWidget()->SetSelectedState(0);
	//this->ViewLegendButton->GetWidget()->SetSelectedState(0);
	//this->ClippingPlaneMenuButton->GetWidget()->SetValue("Off");
	//this->ClippingPlaneCallback(1);
	this->CancelStatus = 0;
	strcpy(this->objectSelectionPrevious, "");
	strcpy(this->elementSetSelectionPrevious, "");
}
//-----------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->FEMeshList->GetNumberOfItems(); i++)
	{
		vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->FEMeshList->GetItem(i));  
		//vtkDoubleArray *materialproparray = vtkDoubleArray::SafeDownCast(
		//	meshActor->GetDataSet()->GetCellData()->GetArray("Youngs_Modulus"));
		//if(materialproparray)
		//{
			ObjectListComboBox->GetWidget()->AddValue(
				this->FEMeshList->GetItem(i)->GetFileName());
		  
			bool viewedItem = meshActor->GetMeshVisibility();
	    
			if ((viewedItem) && (defaultItem == -1))
			{
			  defaultItem = i;
			}		  		
  	//}
  	/*
  	else
  	{
  	  std::cout << "Failed to find Youngs_Modulus Array" << std::endl;
  	}
  	*/
	}
	if ((defaultItem == -1) && (this->FEMeshList->GetNumberOfItems() > 0))
    defaultItem = this->FEMeshList->GetNumberOfItems()-1;
    
	if (defaultItem != -1)
  {
    ObjectListComboBox->GetWidget()->SetValue(
          this->FEMeshList->GetItem(defaultItem)->GetFileName());
  }
	else
	{
    ObjectListComboBox->GetWidget()->SetValue("");
	}
	this->SelectionChangedCallback(ObjectListComboBox->GetWidget()->GetValue());
}
//--------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::SelectionChangedCallback(const char *Selection)
{
	if(this->CancelStatus)	return;
	if(!strcmp(Selection,""))
	{
		this->DefineElSetButton->SetStateToDisabled();
		return;
	}
	if(!strcmp(this->objectSelectionPrevious, Selection))
	{
		return;
	}
	//this->RemovePreviousSelectionDisplay();
	strcpy(this->objectSelectionPrevious,Selection);

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex(Selection)));
	vtkUnstructuredGrid *ugrid = meshActor->GetDataSet();

	// populate the element set list
	this->ElementSetComboBox->GetWidget()->DeleteAllValues();
	int i;
	vtkStringArray *strarray = vtkStringArray::SafeDownCast(
		ugrid->GetFieldData()->GetAbstractArray("Element_Set_Names"));

	int numarrrays = strarray->GetNumberOfValues();

	for (i=0; i<numarrrays; i++)
	{
		this->ElementSetComboBox->GetWidget()->AddValue(
			strarray->GetValue(i));
	}
	this->ElementSetComboBox->GetWidget()->SetValue( strarray->GetValue(0) );
	strcpy(this->elementSetSelectionPrevious, "");
	if(this->ViewOptionsGroup)
	{
		this->ViewOptionsGroup->SetMeshActor(meshActor);
	}
	if(this->DefineElementSetDialog)
	{
		this->DefineElementSetDialog->SetMeshActor(meshActor);
		this->DefineElementSetDialog->ResetState();
	}
	this->ElementSetChangedCallback(this->ElementSetComboBox->GetWidget()->GetValue());
}
//-------------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::ElementSetChangedCallback(const char *Selection)
{
	if(this->CancelStatus)	return;
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(Selection,""))
	{
		callback->ErrorMessage("Element Set Selection Required");
		return;
	}

	//if(!strcmp(this->elementSetSelectionPrevious, Selection))
	//{
	//	return;
	//}
	//strcpy(this->elementSetSelectionPrevious,Selection);
    this->DefineElSetButton->SetStateToNormal();
  
	// get the femesh
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *elSetName = this->ElementSetComboBox->GetWidget()->GetValue();
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex(
		ObjectListComboBox->GetWidget()->GetValue())));
	vtkUnstructuredGrid *ugrid = meshActor->GetDataSet();
	
//	meshActor->HideMesh();
	char young[256];
	strcpy(young, Selection);
	strcat(young, "_Constant_Youngs_Modulus");

	char poisson[256];
	strcpy(poisson, Selection);
	strcat(poisson, "_Constant_Poissons_Ratio");

	// check if the field data exists.
	vtkDoubleArray *Earray = vtkDoubleArray::SafeDownCast(
		ugrid->GetFieldData()->GetArray(young));	

	vtkFloatArray *Nuarray = vtkFloatArray::SafeDownCast(
		ugrid->GetFieldData()->GetArray(poisson));	

	float youngsmodulus;
	float poissonsratio;
	if(Earray)
	{
		youngsmodulus = Earray->GetValue(0);
		this->YoungsModulusEntry->GetWidget()->SetValueAsDouble(youngsmodulus);
	}
	else
	{
		this->YoungsModulusEntry->GetWidget()->SetValue("");
	}
	if(Nuarray)
	{
		poissonsratio = Nuarray->GetValue(0);
		this->PoissonsRatioEntry->GetWidget()->SetValueAsDouble(poissonsratio);
	}
	else{
		this->PoissonsRatioEntry->GetWidget()->SetValue("");
	}
	if(this->ViewOptionsGroup)
	{
		meshActor->ShowHideAllElementSets(0);
		this->ViewOptionsGroup->SetElementSetName(Selection);
		this->ViewOptionsGroup->Update();
	}
	// youngs modulus and poissons ratio data exists
	//if(meshActor->GetDisplayMode() == vtkMimxMeshActor::DisplayElementSets)
	//{
	//	meshActor->ShowElementSet(Selection);
	//}

	//int viewState = this->ViewPropertyButton->GetWidget()->GetSelectedState();
	//if(viewState)
	//{
	//	this->ViewPropertyButton->GetWidget()->SetSelectedState(0);
	//	this->ViewPropertyButton->GetWidget()->SetSelectedState(1);
	//}
//	meshActor->DisableMeshCuttingPlane();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	
	this->YoungsModulusEntry->SetEnabled( 1 );
  this->PoissonsRatioEntry->SetEnabled( 1 );
}

#if 0
//-------------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::ViewMaterialPropertyCallback( int mode )
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

  const char *elsetName = this->ElementSetComboBox->GetWidget()->GetValue();  
  if(!strcmp(elsetName, ""))
  {
	  if(!this->CancelStatus)
		callback->ErrorMessage("Choose an Element Set");
	  return;
  }
	char young[256];
	strcpy(young, elsetName);
	strcat(young, "_Constant_Youngs_Modulus");

	char poisson[256];
	strcpy(poisson, elsetName);
	strcat(poisson, "_Constant_Poissons_Ratio");

	// check if the field data exists.
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
           this->FEMeshList->GetItem(combobox->GetValueIndex( combobox->GetValue() )));
  if(!meshActor)
  {
	  if(!this->CancelStatus)
			callback->ErrorMessage("Choose a valid mesh");
	  return;
  }

  int viewElsetMode = this->ViewElementSetMenuButton->GetWidget()->GetSelectedState();
  if ( mode )
  {
	  char young[256];
	  strcpy(young, "Youngs_Modulus");
	  if(!viewElsetMode)
	  {
		  meshActor->SetMeshScalarName(young);
	//	  meshActor->SetElementSetScalarRangeFromElementSet( elsetName, young);
		//meshActor->SetMeshScalarName( "Youngs_Modulus" );
		 meshActor->SetMeshScalarVisibility(true);
		 this->ViewLegendButton->GetWidget()->SetSelectedState(1);
	  }
	  else{
		  int legendRangeMode = this->ScalarBarRangeComputationButton->GetWidget()->GetSelectedState();
		  if(legendRangeMode)
		  {
//			  meshActor->GenerateElementSetMapperLookUpTableFromElementSetRange(elsetName, young);
			  meshActor->SetElementSetScalarVisibility(elsetName, true);
		  }
		  else
		  {
			  meshActor->SetElementSetScalarVisibility(elsetName, false);
			 meshActor->GenerateElementSetMapperLookUpTableFromMeshRange(elsetName, young);
		  }
		  meshActor->SetElementSetScalarVisibility(elsetName, true);
	  }
     this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }
  else
  {
	meshActor->SetMeshScalarVisibility(false);
    this->ViewPropertyLegendCallback(0);
    this->ViewLegendButton->GetWidget()->SetSelectedState( 0 );
	this->GetMimxMainWindow()->GetRenderWidget()->Render(); 
  }
  
}

//-------------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::ViewPropertyLegendCallback( int mode )
{
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
           this->FEMeshList->GetItem(combobox->GetValueIndex( combobox->GetValue() )));

  if(!meshActor)	return;         
  const char *elsetName = this->ElementSetComboBox->GetWidget()->GetValue();
  if(!strcmp(elsetName, ""))	return;

  int viewElsetMode = this->ViewElementSetMenuButton->GetWidget()->GetSelectedState();
  if ( mode )
  {
	  if(viewElsetMode)
	  {
		  meshActor->SetElementSetLegendVisibility(elsetName, true);
		  meshActor->SetMeshLegendVisibility(false);
	  }
	  else{
		  meshActor->SetElementSetLegendVisibility(elsetName, false);
		  meshActor->SetMeshLegendVisibility(true);
	  }
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
  }
  else
  {
	  meshActor->SetElementSetLegendVisibility(elsetName, false);
    meshActor->SetMeshLegendVisibility(false);
    this->GetMimxMainWindow()->GetRenderWidget()->Render(); 
  }
}

//-------------------------------------------------------------------------------------
int vtkKWMimxConstMatPropElSetGroup::ClippingPlaneCallback( int mode )
{

  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *elSetName = this->ElementSetComboBox->GetWidget()->GetValue();
  if(!strcmp(elSetName, ""))	return 0;
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
	  this->FEMeshList->GetItem(combobox->GetValueIndex( combobox->GetValue() )));
  if(!meshActor)	return 0;         
  if (mode == 1)
  {
    meshActor->DisableMeshCuttingPlane();
  }
  else if (mode == 2)
  {
    meshActor->EnableMeshCuttingPlane();
    meshActor->SetInvertCuttingPlane( false );
  }
  else
  {
    meshActor->EnableMeshCuttingPlane();
    meshActor->SetInvertCuttingPlane( true );  
  }
  this->GetMimxMainWindow()->GetRenderWidget()->Render(); 

	return 1;
}

//----------------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::ScalarBarRangeComputationCallback( int mode )
{

}

//----------------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::RemovePreviousSelectionDisplay()
{
	if(!strcmp(this->objectSelectionPrevious, ""))	return;
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex( this->objectSelectionPrevious )));
	if(!meshActor)
	{
		return;
	}

	int matVisState = this->ViewPropertyButton->GetWidget()->GetSelectedState();
	if(matVisState)		meshActor->SetMeshScalarVisibility(false);

	int legendVisState = this->ViewLegendButton->GetWidget()->GetSelectedState();
	if(legendVisState)	    meshActor->SetMeshLegendVisibility(false);

	const char *cuttingPlaneState = this->ClippingPlaneMenuButton->GetWidget()->GetValue();
	
	if(strcmp(cuttingPlaneState, "Off"))	    meshActor->DisableMeshCuttingPlane();
	this->ClippingPlaneMenuButton->GetWidget()->SetValue("Off");
	this->GetMimxMainWindow()->GetRenderWidget()->Render(); 
}
//----------------------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::ViewElementSetCallback(int mode)
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

	// check if the field data exists.
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex( combobox->GetValue() )));
	if(!meshActor)
	{
		return;
	}

	const char *elsetName = this->ElementSetComboBox->GetWidget()->GetValue();  
	if(!strcmp(elsetName, ""))
	{
		return;
	}
	if ( mode )
	{
		char young[256];
		strcpy(young, "Youngs_Modulus");
//		meshActor->SetElementSetScalarName(elsetName, young);
//		meshActor->SetElementSetScalarRangeFromElementSet( elsetName, young);
		meshActor->ShowElementSet(elsetName);
		meshActor->HideMesh();
	//	meshActor->SetElementSetScalarVisibility(true);
	//	this->ViewLegendButton->GetWidget()->SetSelectedState(1);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		this->ScalarBarRangeComputationButton->GetWidget()->SetStateToNormal();
		
	}
	else
	{
		meshActor->ShowMesh();
		meshActor->HideElementSet(elsetName);
		
		this->ScalarBarRangeComputationButton->GetWidget()->SelectedStateOff();
		this->ScalarBarRangeComputationButton->GetWidget()->SetStateToDisabled();
		
//		meshActor->SetElementSetScalarVisibility(elsetName, false);
//		this->ViewPropertyLegendCallback(0);
//		this->ViewLegendButton->GetWidget()->SetSelectedState( 0 );
		this->GetMimxMainWindow()->GetRenderWidget()->Render(); 
		
	}

	
}
#endif
//----------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::DefineElementSetCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Mesh must be selected");
		return;
	}

  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Invalid Mesh was selected");
		combobox->SetValue("");
		return;
	}
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex(name)));
		
	if (!this->DefineElementSetDialog)
	{
	  this->DefineElementSetDialog = vtkKWMimxDefineElSetGroup::New();
	  this->DefineElementSetDialog->SetApplication( this->GetApplication() );
	  this->DefineElementSetDialog->SetMimxMainWindow( this->GetMimxMainWindow() );
	  this->DefineElementSetDialog->Create();
	}
	this->DefineElementSetDialog->SetMeshActor( meshActor );
	this->DefineElementSetDialog->SetElementSetCombobox(this->ElementSetComboBox->GetWidget());
	this->DefineElementSetDialog->Display();
}

//--------------------------------------------------------------------------------------------
void vtkKWMimxConstMatPropElSetGroup::ViewOptionsCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Mesh must be selected");
		return;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(
		this->FEMeshList->GetItem(combobox->GetValueIndex(name)));

	if(!strcmp(this->ElementSetComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Element Set must be specified");
		return;
	}

	combobox = this->ElementSetComboBox->GetWidget();
	name = combobox->GetValue();

	if (!this->ViewOptionsGroup)
	{
		this->ViewOptionsGroup = vtkKWMimxViewPropertiesOptionGroup::New();
		this->ViewOptionsGroup->SetApplication( this->GetApplication() );
		this->ViewOptionsGroup->SetMimxMainWindow( this->GetMimxMainWindow() );
		this->ViewOptionsGroup->SetMeshActor(meshActor);
		this->ViewOptionsGroup->SetElementSetName(name);
		this->ViewOptionsGroup->SetArrayName("Youngs_Modulus");
		this->ViewOptionsGroup->Create();
	}
	else
	{
		this->ViewOptionsGroup->SetMeshActor(meshActor);
		this->ViewOptionsGroup->SetElementSetName(name);
		this->ViewOptionsGroup->SetArrayName("Youngs_Modulus");
	}
	this->ViewOptionsGroup->Display();
}

//--------------------------------------------------------------------------------------------
