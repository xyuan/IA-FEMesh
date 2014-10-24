/*=========================================================================
Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxEditBBGroup.cxx,v $
Language:  C++
Date:      $Date: 2008/08/17 01:42:57 $
Version:   $Revision: 1.98.2.3 $

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

#include "vtkKWMimxEditBBGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxMainNotebook.h"

#include "vtkMimxUnstructuredGridWidget.h"
#include "vtkMimxExtractEdgeWidget.h"
#include "vtkMimxExtractFaceWidget.h"
#include "vtkMimxExtractCellWidget.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxMapOriginalCellAndPointIds.h"
#include "vtkMimxExtractSurface.h"
#include "vtkMimxExtrudePolyData.h"
#include "vtkDataSet.h"
#include "vtkAppendFilter.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkMimxMirrorUnstructuredHexahedronGridCell.h"

#include "vtkActor.h"
#include "vtkMimxBoundingBoxSource.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkPolyData.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMimxSplitUnstructuredHexahedronGridCell.h"
#include "vtkMimxAddUnstructuredHexahedronGridCell.h"
#include "vtkMimxDeleteUnstructuredHexahedronGridCell.h"
#include "vtkMimxSelectCellsWidget.h"
#include "vtkIdList.h"
#include "vtkKWMimxMergeBBGroup.h"
#include "vtkKWMimxEditBBGroup.h"
#include "vtkMimxSelectCellsWidget.h"
#include "vtkMimxSubdivideBoundingBox.h"
#include "vtkKWPushButtonSet.h"
#include "vtkMergeCells.h"
#include "vtkCellData.h"
#include "vtkMimxViewMergePointsWidget.h"

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
#include "vtkKWNotebook.h"
#include "vtkKWOptions.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkLinkedListWrapper.h"
#include "vtkPlaneWidget.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkKWCheckButtonSet.h"
#include "vtkKWCheckButton.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkPolyDataWriter.h"

#include "vtkObjectFactory.h"
#include "vtkKWPushButton.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWComboBox.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkDataSetCollection.h"
#include "vtkMimxMergePoints.h"
#include "vtkKWMimxApplication.h"

#include "Resources/mimxRedo.h"
#include "Resources/mimxUndo.h"
#include "Resources/mimxAdd.h"
#include "Resources/mimxDelete.h"
#include "Resources/mimxSplit.h"
#include "Resources/mimxMove.h"
#include "Resources/mimxMirror.h"
#include "Resources/mimxMerge.h"
#include "Resources/mimxApply.h"
#include "Resources/mimxCancel.h"
#include "Resources/mimxCompleteStruct.h"
#include "Resources/mimxSubStructure.h"
#include "Resources/mimxMergeBlocks.h"
#include "Resources/mimxPlaneXY.h"
#include "Resources/mimxPlaneXZ.h"
#include "Resources/mimxPlaneYZ.h"

#include <vtkUnstructuredGridWriter.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

// define the option types
#define VTK_KW_OPTION_NONE         0
#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxEditBBGroup);
vtkCxxRevisionMacro(vtkKWMimxEditBBGroup, "$Revision: 1.98.2.3 $");

//----------------------------------------------------------------------------
class vtkRightButtonPressEventCallback : public vtkCommand
{
public:
  static vtkRightButtonPressEventCallback *New() 
    { return new vtkRightButtonPressEventCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
		this->EditBBInstance->EditBBApplyCallback();
    }
  
  vtkKWMimxEditBBGroup *EditBBInstance;
};
//----------------------------------------------------------------------------
vtkKWMimxEditBBGroup::vtkKWMimxEditBBGroup()
{
  this->MimxMainWindow = NULL;
  this->ComponentFrame = NULL;
  this->ObjectListComboBox = NULL;
  this->ButtonFrame = NULL;
  this->UnstructuredGridWidget = NULL;
  this->ExtractEdgeWidget = NULL;
  this->ExtractFaceWidget = NULL;
  this->ExtractCellWidget = NULL;
  this->ExtractCellWidgetHBB = NULL;
  this->RadiusEntry = NULL;
  this->ExtrudeEntry = NULL;
  this->ToleranceEntry = NULL;
  this->MergeBBGroup = NULL;
  this->MirrorBBGroup = NULL;
   this->RadioButtonSet = NULL;
  this->AddButtonState = 0;
  this->MoveButtonState = 0;
  this->SplitButtonState = 0;
  this->MirrorButtonState = 0;
  this->RegularButtonState = 1;
  this->ConvertToHBBCount = 0;
  this->SplitCount = 0;
  this->AddCount = 0;
  this->DeleteCount = 0;
  this->DeleteButtonState = 0;
  this->MirrorCount = 0;
  this->MergeCount = 0;
  this->DoUndoButtonSet = NULL;
  this->SelectSubsetButton = NULL;
  this->SelectCellsWidget = NULL;
  this->CancelStatus = 0;
  this->EntryFrame = NULL;
  this->defaultRadiusEntry = -1.0;
  this->defaultExtrusionLength = 1.0;
  this->defaultMergeTolerance = 0.1;

  this->MirrorFrame = NULL;
  this->AxisSelection = NULL;
  this->MirrorPlaneWidget = NULL;
  
  this->MergeFrame = NULL;
  this->MultiColumnList = NULL;
  this->MergeTolerence = NULL;
  this->MergeFeedBackButton = NULL;

  this->RightButtonPressCallback = vtkRightButtonPressEventCallback::New();
  this->RightButtonPressCallback->EditBBInstance = this;
  this->ViewMergePointsWidget = NULL;
  this->DataSetCollection = vtkDataSetCollection::New();
  this->SphereSizeEntry = NULL;
  this->SphereSizeFrame = NULL;
  this->ConsolidateSphereSizeEntry = NULL;
  this->ConsolidateSphereSizeFrame = NULL;
  this->defaultSphereSizeEntry = -1.0;
}

//----------------------------------------------------------------------------
vtkKWMimxEditBBGroup::~vtkKWMimxEditBBGroup()
{
  if(this->ComponentFrame)  
    this->ComponentFrame->Delete();
  if(this->ObjectListComboBox)  
    this->ObjectListComboBox->Delete();
  this->RadioButtonSet->Delete();
  if(this->UnstructuredGridWidget)
    this->UnstructuredGridWidget->Delete();
  if(this->ExtractEdgeWidget)
    this->ExtractEdgeWidget->Delete();
  if(this->ExtractFaceWidget)
    this->ExtractFaceWidget->Delete();
  if(this->ExtractCellWidget)
	  this->ExtractCellWidget->Delete();
  if(this->RadiusEntry)
	  this->RadiusEntry->Delete();
  if(this->ExtrudeEntry)
	  this->ExtrudeEntry->Delete();
  if(this->ToleranceEntry)
	  this->ToleranceEntry->Delete();
	if(this->MergeBBGroup)
	  this->MergeBBGroup->Delete();
  this->RadioButtonSet->Delete();
  if(  this->ExtractCellWidgetHBB)
	  this->ExtractCellWidgetHBB->Delete();

  if(this->DoUndoButtonSet)
	  this->DoUndoButtonSet->Delete();
  if(this->SelectCellsWidget)
	  this->SelectCellsWidget->Delete();
	if(this->ButtonFrame)
	  this->ButtonFrame->Delete();
	if(this->EntryFrame)
		this->EntryFrame->Delete();
  if (this->SelectSubsetButton)
    this->SelectSubsetButton->Delete();
    
	if (this->MirrorFrame)
	  this->MirrorFrame->Delete();
	if (this->AxisSelection)
	  this->AxisSelection->Delete();
	if (this->MirrorPlaneWidget)
		this->MirrorPlaneWidget->Delete();
	
	if (this->MergeFrame)
	  this->MergeFrame->Delete();
	if (this->MultiColumnList)
	  this->MultiColumnList->Delete();
	if (this->MergeTolerence)
		this->MergeTolerence->Delete();
	if(this->MergeFeedBackButton)
		this->MergeFeedBackButton->Delete();
		
	this->RightButtonPressCallback->Delete();
	if(this->ViewMergePointsWidget)
		this->ViewMergePointsWidget->Delete();
	this->DataSetCollection->Delete();
	if(this->SphereSizeEntry)
		this->SphereSizeEntry->Delete();
	if(this->SphereSizeFrame)
		this->SphereSizeFrame->Delete();
	if(this->ConsolidateSphereSizeEntry)
		this->ConsolidateSphereSizeEntry->Delete();
	if(this->ConsolidateSphereSizeFrame)
		this->ConsolidateSphereSizeFrame->Delete();
}
//--------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::BBSelectionChangedCallback(const char* dummy)
{
	if ( ( this->CancelStatus )	/*|| (this->ObjectListComboBox->GetWidget()->GetState() == 0)*/ )
	{
	  return;
	}
	this->AddButtonState = 1;
	this->MoveButtonState = 1;
	this->SplitButtonState = 1;
	this->DeleteButtonState = 1;
	this->MirrorButtonState = 1;
	this->ConvertToHBBButtonState = 1;
//std::cout << "Selection Change Callback" << std::endl;
if( dummy != NULL )
{
	if(!strcmp(this->objectSelectionPrevious, dummy))
	{
		return;
	}
	strcpy(this->objectSelectionPrevious, dummy);
	this->SelectSubsetButton->SetEnabled(1);
	this->SelectFullSetCallback( 0 );
	this->defaultRadiusEntry = -1.0;
	this->defaultSphereSizeEntry = -1.0;
  if(this->RadioButtonSet->GetWidget(0)->GetSelectedState())
  {
	  this->RadioButtonSet->GetWidget(0)->SetSelectedState(0);
	  this->RadioButtonSet->GetWidget(0)->SetSelectedState(1);
	return;
  }

  if(this->RadioButtonSet->GetWidget(5)->GetSelectedState())
  {
	  this->RadioButtonSet->GetWidget(5)->SetSelectedState(0);
	  this->RadioButtonSet->GetWidget(5)->SetSelectedState(1);
	  return;
  }

  if(this->RadioButtonSet->GetWidget(1)->GetSelectedState())
  {
	  this->RadioButtonSet->GetWidget(1)->SetSelectedState(0);
    this->RadioButtonSet->GetWidget(1)->SetSelectedState(1);
 //   this->EditBBSplitCellCallback();
	return;
  }
  if(this->RadioButtonSet->GetWidget(2)->GetSelectedState())
  {
    //std::cout << "Add Cell callback" << std::endl;
	this->RadioButtonSet->GetWidget(2)->SetSelectedState(0);
    this->RadioButtonSet->GetWidget(2)->SetSelectedState(1);
 //   this->EditBBAddCellCallback();
	return;
  }
  if(this->RadioButtonSet->GetWidget(3)->GetSelectedState())
  {
	  this->RadioButtonSet->GetWidget(3)->SetSelectedState(0);
	  this->RadioButtonSet->GetWidget(3)->SetSelectedState(1);
//	  this->EditBBDeleteCellCallback();
	  return;
  }
  if(this->RadioButtonSet->GetWidget(4)->GetSelectedState())
  {
	  this->RadioButtonSet->GetWidget(4)->SetSelectedState(0);
	  this->RadioButtonSet->GetWidget(4)->SetSelectedState(1);
	  return;
  }
}

if(this->RadioButtonSet->GetWidget(6)->GetSelectedState())
{
	this->RadioButtonSet->GetWidget(6)->SetSelectedState(0);
	this->RadioButtonSet->GetWidget(6)->SetSelectedState(1);
	//   this->EditBBSplitCellCallback();
	return;
}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	if(!strcmp(combobox->GetValue(),""))
	{
		return;
	}
	const char *name = combobox->GetValue();
//	int num = combobox->GetValueIndex(name);
	if(this->DoUndoTree->GetItem(name)->Parent != NULL)	
		this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
	else
		this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);

	if(this->DoUndoTree->GetItem(name)->Child != NULL)	
		this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
	else
		this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
}
//--------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::CreateWidget()
{
  if(this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();
  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Edit Building Block");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 0 -fill x", 
    this->MainFrame->GetWidgetName());
  

  if (!this->ComponentFrame)	
     this->ComponentFrame = vtkKWFrameWithLabel::New();
  ComponentFrame->SetParent( this->MainFrame );
  ComponentFrame->Create();
  ComponentFrame->SetLabelText("Building Block");
  ComponentFrame->CollapseFrame( );
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
    this->ComponentFrame->GetWidgetName());
  
  if(!this->ObjectListComboBox)
    this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();  
  this->ObjectListComboBox->SetParent(this->ComponentFrame->GetFrame());
  this->ObjectListComboBox->Create();
  this->ObjectListComboBox->SetWidth(20);
  this->ObjectListComboBox->SetLabelText("Building Block : ");
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox->GetWidget()->SetStateToNormal();
  this->ObjectListComboBox->GetWidget()->SetCommand(this, "BBSelectionChangedCallback");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 2 -fill x", 
    this->ObjectListComboBox->GetWidgetName());

  if(!this->SelectSubsetButton)
    this->SelectSubsetButton = vtkKWCheckButton::New();
  this->SelectSubsetButton->SetParent( this->MainFrame );
  this->SelectSubsetButton->Create();
  this->SelectSubsetButton->SetBorderWidth(2);
  this->SelectSubsetButton->SetReliefToGroove();

  this->SelectSubsetButton->SetCommand(this, "SelectFullSetCallback");
  this->SelectSubsetButton->IndicatorVisibilityOff();
  this->SelectSubsetButton->SetBalloonHelpString(
	  "Toggle between the full building block and a selected subsection");
	vtkKWIcon *fullIcon = vtkKWIcon::New();
  fullIcon->SetImage(    image_mimxCompleteStruct, 
                         image_mimxCompleteStruct_width, 
                         image_mimxCompleteStruct_height, 
                         image_mimxCompleteStruct_pixel_size); 
  vtkKWIcon *partialIcon = vtkKWIcon::New();
  partialIcon->SetImage(    image_mimxSubStructure, 
                            image_mimxSubStructure_width, 
                            image_mimxSubStructure_height, 
                            image_mimxSubStructure_pixel_size);                        
  this->SelectSubsetButton->SetImageToIcon( partialIcon );
  this->SelectSubsetButton->SetSelectImageToIcon( fullIcon );
  this->GetApplication()->Script( "pack %s -side top -anchor center -expand n -padx 2 -pady 6", 
	  this->SelectSubsetButton->GetWidgetName());

  if(!this->RadioButtonSet)
    this->RadioButtonSet = vtkKWCheckButtonSet::New();
  this->RadioButtonSet->SetParent( this->MainFrame );
  this->RadioButtonSet->Create();
  this->RadioButtonSet->SetBorderWidth(2);
  this->RadioButtonSet->SetReliefToGroove();
  this->RadioButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(1);

  /***VAM - Update for the Consolidate Button */
  for (int id = 0; id < 7; id++)	  this->RadioButtonSet->AddWidget(id);

  /******************* Move Button *******************/
  vtkKWIcon *moveIcon = vtkKWIcon::New();
  moveIcon->SetImage(    image_mimxMove, 
                         image_mimxMove_width, 
                         image_mimxMove_height, 
                         image_mimxMove_pixel_size); 
  this->RadioButtonSet->GetWidget(0)->SetBorderWidth(2);
  this->RadioButtonSet->GetWidget(0)->SetImageToIcon( moveIcon );
  this->RadioButtonSet->GetWidget(0)->SetSelectImageToIcon( moveIcon );
  this->RadioButtonSet->GetWidget(0)->IndicatorVisibilityOff();
  this->RadioButtonSet->GetWidget(0)->SetBalloonHelpString("Move: Reposition a vertex, edge, and/or face of the building block");
  this->RadioButtonSet->GetWidget(0)->SetCompoundModeToLeft();
  this->RadioButtonSet->GetWidget(0)->SetCommand(this, "EditBBMoveCellCallback");

  /******************* Split Button *******************/
  vtkKWIcon *splitIcon = vtkKWIcon::New();
  splitIcon->SetImage(   image_mimxSplit, 
                         image_mimxSplit_width, 
                         image_mimxSplit_height, 
                         image_mimxSplit_pixel_size);
  this->RadioButtonSet->GetWidget(1)->SetBorderWidth(2);
  this->RadioButtonSet->GetWidget(1)->SetImageToIcon(splitIcon);
  this->RadioButtonSet->GetWidget(1)->SetSelectImageToIcon(splitIcon);
  this->RadioButtonSet->GetWidget(1)->IndicatorVisibilityOff();
  this->RadioButtonSet->GetWidget(1)->SetBalloonHelpString("Split: Subdivide a building block");
  this->RadioButtonSet->GetWidget(1)->SetCompoundModeToLeft();
  this->RadioButtonSet->GetWidget(1)->SetCommand(this, "EditBBSplitCellCallback");
  
  /******************* Add Button *******************/
  vtkKWIcon *addIcon = vtkKWIcon::New();
  addIcon->SetImage(  image_mimxAdd, 
                       image_mimxAdd_width, 
                       image_mimxAdd_height, 
                       image_mimxAdd_pixel_size);
  this->RadioButtonSet->GetWidget(2)->SetBorderWidth(2);
  this->RadioButtonSet->GetWidget(2)->SetImageToIcon( addIcon );
  this->RadioButtonSet->GetWidget(2)->SetSelectImageToIcon( addIcon );
  this->RadioButtonSet->GetWidget(2)->IndicatorVisibilityOff();
  this->RadioButtonSet->GetWidget(2)->SetBalloonHelpString("Add: Add a new block, or blocks, to the face(s) of an existing block structure");
  this->RadioButtonSet->GetWidget(2)->SetCompoundModeToLeft();
  this->RadioButtonSet->GetWidget(2)->SetCommand(this, "EditBBAddCellCallback");

  
  /******************* Delete Button *******************/
  vtkKWIcon *deleteIcon = vtkKWIcon::New();
  deleteIcon->SetImage(  image_mimxDelete, 
                         image_mimxDelete_width, 
                         image_mimxDelete_height, 
                         image_mimxDelete_pixel_size);
  this->RadioButtonSet->GetWidget(3)->SetBorderWidth(2);
  this->RadioButtonSet->GetWidget(3)->SetReliefToRaised( );
  this->RadioButtonSet->GetWidget(3)->SetImageToIcon(deleteIcon);
  this->RadioButtonSet->GetWidget(3)->SetSelectImageToIcon(deleteIcon);
  this->RadioButtonSet->GetWidget(3)->IndicatorVisibilityOff();
  this->RadioButtonSet->GetWidget(3)->SetBalloonHelpString("Delete: Remove unwanted blocks from a building block structure");
  this->RadioButtonSet->GetWidget(3)->SetCompoundModeToLeft();
  this->RadioButtonSet->GetWidget(3)->SetCommand(this, "EditBBDeleteCellCallback");

  /******************* Consolidate Button *******************/
  vtkKWIcon *consolidateIcon = vtkKWIcon::New();
  consolidateIcon->SetImage(  image_mimxMergeBlocks, 
                         image_mimxMergeBlocks_width, 
                         image_mimxMergeBlocks_height, 
                         image_mimxMergeBlocks_pixel_size);
  this->RadioButtonSet->GetWidget(4)->SetBorderWidth(2);
  this->RadioButtonSet->GetWidget(4)->SetReliefToRaised( );
  this->RadioButtonSet->GetWidget(4)->SetImageToIcon(consolidateIcon);
  this->RadioButtonSet->GetWidget(4)->SetSelectImageToIcon(consolidateIcon);
  this->RadioButtonSet->GetWidget(4)->IndicatorVisibilityOff();
  this->RadioButtonSet->GetWidget(4)->SetBalloonHelpString("Consolidate: Consolidates two or more independently defined building block structures into a single structural definition");
  this->RadioButtonSet->GetWidget(4)->SetCompoundModeToLeft();
  this->RadioButtonSet->GetWidget(4)->SetCommand(this, "EditBBConsolidateBlockCallback");
  
  /******************* Mirror Button *******************/
  vtkKWIcon *mirrorIcon = vtkKWIcon::New();
  mirrorIcon->SetImage(  image_mimxMirror, 
                       image_mimxMirror_width, 
                       image_mimxMirror_height, 
                       image_mimxMirror_pixel_size);                     
  this->RadioButtonSet->GetWidget(5)->SetBorderWidth(2);
  this->RadioButtonSet->GetWidget(5)->SetImageToIcon(mirrorIcon);
  this->RadioButtonSet->GetWidget(5)->SetSelectImageToIcon(mirrorIcon);
  this->RadioButtonSet->GetWidget(5)->IndicatorVisibilityOff();
  this->RadioButtonSet->GetWidget(5)->SetBalloonHelpString("Mirror: Mirror a building block, or block structure, about a plane");
  this->RadioButtonSet->GetWidget(5)->SetCompoundModeToLeft();
  this->RadioButtonSet->GetWidget(5)->SetCommand(this, "EditBBMirrorCallback");


  /******************* Merge Button *******************/
  vtkKWIcon *mergeIcon = vtkKWIcon::New();
  mergeIcon->SetImage( image_mimxMerge, 
                       image_mimxMerge_width, 
                       image_mimxMerge_height, 
                       image_mimxMerge_pixel_size);  
  this->RadioButtonSet->GetWidget(6)->SetBorderWidth(2);
  this->RadioButtonSet->GetWidget(6)->SetImageToIcon(mergeIcon);
  this->RadioButtonSet->GetWidget(6)->SetSelectImageToIcon(mergeIcon);
  this->RadioButtonSet->GetWidget(6)->IndicatorVisibilityOff();
  this->RadioButtonSet->GetWidget(6)->SetBalloonHelpString("Equivalence/Merge: Merge the vertices of the selected building block (within specified tolerance)");
  this->RadioButtonSet->GetWidget(6)->SetCompoundModeToLeft();
  this->RadioButtonSet->GetWidget(6)->SetCommand(this, "EditBBMergeCallback");

  this->GetApplication()->Script( "pack %s -side top -anchor center -expand n -padx 2 -pady 6", 
	  this->RadioButtonSet->GetWidgetName());
  
  if(!this->EntryFrame)
	  this->EntryFrame = vtkKWFrame::New();
  this->EntryFrame->SetParent(this->GetParent());
  this->EntryFrame->Create();

  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s", 
	  this->EntryFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());
/*
  if (!this->RadiusEntry)
   this->RadiusEntry = vtkKWEntryWithLabel::New();  
  this->RadiusEntry->SetParent(this->EntryFrame);
  this->RadiusEntry->Create();
  this->RadiusEntry->SetWidth(4);
  this->RadiusEntry->SetLabelText("Scale Spheres: ");
  this->RadiusEntry->GetWidget()->SetValueAsDouble(1.0);
  this->RadiusEntry->GetWidget()->SetCommand(this, "RadiusChangeCallback");
  this->RadiusEntry->GetWidget()->SetRestrictValueToDouble();
*/
  if(!this->SphereSizeFrame)
	  this->SphereSizeFrame = vtkKWFrame::New();
  this->SphereSizeFrame->SetParent(this->GetParent());
  this->SphereSizeFrame->Create();

  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s", 
	  this->SphereSizeFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());

  if (!this->ExtrudeEntry)
   this->ExtrudeEntry = vtkKWEntryWithLabel::New();  
  this->ExtrudeEntry->SetParent(this->EntryFrame);
  this->ExtrudeEntry->Create();
  this->ExtrudeEntry->SetWidth(4);
  this->ExtrudeEntry->SetLabelText("Extrusion Length: ");
  this->ExtrudeEntry->GetWidget()->SetValueAsDouble(1.0);
  //this->ExtrudeEntry->GetWidget()->SetCommand(this, "RadiusChangeCallback");
  this->ExtrudeEntry->GetWidget()->SetRestrictValueToDouble();  
  
  if (!this->ToleranceEntry)
   this->ToleranceEntry = vtkKWEntryWithLabel::New();  
  this->ToleranceEntry->SetParent(this->EntryFrame);
  this->ToleranceEntry->Create();
  this->ToleranceEntry->SetWidth(4);
  this->ToleranceEntry->SetLabelText("Merge Tolerance: ");
  this->ToleranceEntry->GetWidget()->SetValueAsDouble(1.0);
  this->ToleranceEntry->GetWidget()->SetCommand(this, "MergeToleranceChangeCallback");
  this->ToleranceEntry->GetWidget()->SetRestrictValueToDouble();  

  if (!this->SphereSizeEntry)
	  this->SphereSizeEntry = vtkKWEntryWithLabel::New();
  this->SphereSizeEntry->SetParent(this->SphereSizeFrame);
  this->SphereSizeEntry->Create();
  this->SphereSizeEntry->SetWidth(4);
  this->SphereSizeEntry->SetLabelText("Scale Spheres: ");
  this->SphereSizeEntry->GetWidget()->SetValueAsDouble(1.0);
  this->SphereSizeEntry->GetWidget()->SetCommand(this, "SphereSizeChangeCallback");
  this->SphereSizeEntry->GetWidget()->SetRestrictValueToDouble();

  this->SphereSizeFrame->Unpack();
  /******************* Create the Consolidate Frame *******************/
  if (!this->MergeFrame)
	  this->MergeFrame = vtkKWFrameWithLabel::New();
  this->MergeFrame->SetParent(this->GetParent());
  this->MergeFrame->Create();
  this->MergeFrame->SetLabelText("Consolidate Blocks");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s", 
	  this->MergeFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());  
  
  if (!this->MultiColumnList)
	  this->MultiColumnList = vtkKWMultiColumnListWithScrollbars::New();
  this->MultiColumnList->SetParent(this->MergeFrame->GetFrame());
  this->MultiColumnList->Create();
  this->MultiColumnList->SetHorizontalScrollbarVisibility(1);
  this->MultiColumnList->SetVerticalScrollbarVisibility(1);
  this->MultiColumnList->SetHeight(3);
  int col_index;
  col_index = MultiColumnList->GetWidget()->AddColumn("Name");
  col_index = MultiColumnList->GetWidget()->AddColumn(NULL);
  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  this->MultiColumnList->GetWidget()->SetColumnLabelImageToPredefinedIcon(col_index, vtkKWIcon::IconPlus);
  this->MultiColumnList->GetWidget()->SetSortArrowVisibility(0);
  this->MultiColumnList->GetWidget()->ColumnSeparatorsVisibilityOff();
  this->MultiColumnList->GetWidget()->SetHeight(3);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->MultiColumnList->GetWidgetName());

  if(!this->MergeFeedBackButton)
	  this->MergeFeedBackButton = vtkKWCheckButtonWithLabel::New();
  this->MergeFeedBackButton->SetParent(this->MergeFrame->GetFrame());
  this->MergeFeedBackButton->Create();
  this->MergeFeedBackButton->SetLabelText("View");
  this->MergeFeedBackButton->GetWidget()->SetCommand(this, "ViewMergeFeedbackCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor n -expand n -padx 2 -pady 2", 
	  this->MergeFeedBackButton->GetWidgetName());

  //if (!this->ConsolidateSphereSizeEntry)
	 // this->ConsolidateSphereSizeEntry = vtkKWEntryWithLabel::New();

  //this->ConsolidateSphereSizeEntry->SetParent(this->MergeFrame->GetFrame());
  //this->ConsolidateSphereSizeEntry->Create();
  //this->ConsolidateSphereSizeEntry->SetWidth(4);
  //this->ConsolidateSphereSizeEntry->SetLabelText("Scale Spheres: ");
  //this->ConsolidateSphereSizeEntry->GetWidget()->SetValueAsDouble(1.0);
  //this->ConsolidateSphereSizeEntry->GetWidget()->SetCommand(this, "SphereSizeChangeCallback");
  //this->ConsolidateSphereSizeEntry->GetWidget()->SetRestrictValueToDouble();
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor n -expand n -padx 2 -pady 2", 
	 // this->ConsolidateSphereSizeEntry->GetWidgetName());


  //if(!this->MergeTolerence)
	 // this->MergeTolerence = vtkKWEntryWithLabel::New();
  //this->MergeTolerence->SetParent(this->MergeFrame->GetFrame());
  //this->MergeTolerence->Create();
  //this->MergeTolerence->GetWidget()->SetCommand(this, "MergeToleranceChangeCallback");
  //this->MergeTolerence->SetWidth(6);
  //this->MergeTolerence->SetLabelText("Tolerance : ");
  //this->MergeTolerence->GetWidget()->SetRestrictValueToDouble();
  //this->MergeTolerence->GetWidget()->SetValueAsDouble(1.0);
  //this->MergeTolerence->GetWidget()->SetWidth(5);
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor n -expand n -padx 2 -pady 2", 
	 // this->MergeTolerence->GetWidgetName());

  this->MergeFrame->Unpack();
 
  /******************* Create the Mirror Frame *******************/
  if(!this->MirrorFrame)
	  this->MirrorFrame = vtkKWFrameWithLabel::New();
  this->MirrorFrame->SetParent(this->GetParent());
  this->MirrorFrame->Create();
  this->MirrorFrame->SetLabelText("Align Mirror Plane");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s", 
	  this->MirrorFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());

  if(!this->AxisSelection)
	  this->AxisSelection = vtkKWPushButtonSet::New();
  this->AxisSelection->SetParent(this->MirrorFrame->GetFrame());
  this->AxisSelection->Create();
  //this->AxisSelection->SetBorderWidth(2);
  this->AxisSelection->SetReliefToFlat();
  this->AxisSelection->SetPackHorizontally(1);
  this->AxisSelection->SetMaximumNumberOfWidgetsInPackingDirection(3);
  vtkKWPushButton *pb;
  vtkKWIcon *planeXYIcon = vtkKWIcon::New();
  planeXYIcon->SetImage( image_mimxPlaneXY, 
                       image_mimxPlaneXY_width, 
                       image_mimxPlaneXY_height, 
                       image_mimxPlaneXY_pixel_size);  
  pb = this->AxisSelection->AddWidget(0);
  //pb->SetText("XY");
  pb->SetBorderWidth(2);
  pb->SetImageToIcon(planeXYIcon);
  vtkKWIcon *planeXZIcon = vtkKWIcon::New();
  planeXZIcon->SetImage( image_mimxPlaneXZ, 
                       image_mimxPlaneXZ_width, 
                       image_mimxPlaneXZ_height, 
                       image_mimxPlaneXZ_pixel_size);                        
  pb = this->AxisSelection->AddWidget(1);
  pb->SetBorderWidth(2);
  //pb->SetText("XZ");
  pb->SetImageToIcon(planeXZIcon);
  
  vtkKWIcon *planeYZIcon = vtkKWIcon::New();
  planeYZIcon->SetImage( image_mimxPlaneYZ, 
                       image_mimxPlaneYZ_width, 
                       image_mimxPlaneYZ_height, 
                       image_mimxPlaneYZ_pixel_size);  
  pb = this->AxisSelection->AddWidget(2);
  pb->SetBorderWidth(2);
  //pb->SetText("YZ");
  pb->SetImageToIcon(planeYZIcon);

  this->AxisSelection->GetWidget(0)->SetCommand(this, "PlaceMirroringPlaneAboutZ");
  this->AxisSelection->GetWidget(1)->SetCommand(this, "PlaceMirroringPlaneAboutY");
  this->AxisSelection->GetWidget(2)->SetCommand(this, "PlaceMirroringPlaneAboutX");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor n -padx 2 -pady 2", 
	  this->AxisSelection->GetWidgetName());
  this->MirrorFrame->Unpack();

  /******************* Create the Button Frame *******************/
  if (!this->ButtonFrame)
    this->ButtonFrame = vtkKWFrame::New();
  this->ButtonFrame->SetParent( this->MainFrame );
  this->ButtonFrame->Create();
  this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
              this->ButtonFrame->GetWidgetName() ); 
                                            
	this->ApplyButton->SetParent(this->ButtonFrame);
	this->ApplyButton->Create();
	this->ApplyButton->SetReliefToFlat();
	this->ApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
	this->ApplyButton->SetCommand(this, "EditBBApplyCallback");
	this->ApplyButton->SetStateToDisabled();
	this->GetApplication()->Script(
		"pack %s -side left -anchor nw -expand y -padx 5 -pady 6", 
		this->ApplyButton->GetWidgetName());

  this->CancelButton->SetParent(this->ButtonFrame);
  this->CancelButton->Create();
  this->CancelButton->SetReliefToFlat();
  this->CancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
  this->CancelButton->SetCommand(this, "EditBBCancelCallback");
  this->GetApplication()->Script(
    "pack %s -side right -anchor ne -expand n -padx 5 -pady 6", 
    this->CancelButton->GetWidgetName());

  if(!this->DoUndoButtonSet)
	  this->DoUndoButtonSet = vtkKWPushButtonSet::New();

  this->DoUndoButtonSet->SetParent( this->MainFrame );
  this->DoUndoButtonSet->Create();
  this->DoUndoButtonSet->SetBorderWidth(2);
  this->DoUndoButtonSet->SetReliefToGroove();
  this->DoUndoButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(1);
  for (int id = 0; id < 2; id++)	  this->DoUndoButtonSet->AddWidget(id);
  
  
  /******************* Undo Button *******************/
  vtkKWIcon *undoIcon = vtkKWIcon::New();
  undoIcon->SetImage(  image_mimxUndo, 
                       image_mimxUndo_width, 
                       image_mimxUndo_height, 
                       image_mimxUndo_pixel_size);
  this->DoUndoButtonSet->GetWidget(0)->SetImageToIcon( undoIcon );
  this->DoUndoButtonSet->GetWidget(0)->SetBalloonHelpString("Undo, or roll-back, the building-block editing operations");
  this->DoUndoButtonSet->GetWidget(0)->SetCompoundModeToLeft();
  this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
  this->DoUndoButtonSet->GetWidget(0)->SetCommand(this, "UndoBBCallback");

  /******************* Redo Button *******************/
  vtkKWIcon *redoIcon = vtkKWIcon::New();
  redoIcon->SetImage(  image_mimxRedo, 
                       image_mimxRedo_width, 
                       image_mimxRedo_height, 
                       image_mimxRedo_pixel_size);
  this->DoUndoButtonSet->GetWidget(1)->SetImageToIcon( redoIcon );
  this->DoUndoButtonSet->GetWidget(1)->SetBalloonHelpString("Redo, or roll-forward, the building-block editing operations");
  this->DoUndoButtonSet->GetWidget(1)->SetCompoundModeToRight();
  this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
  this->DoUndoButtonSet->GetWidget(1)->SetCommand(this, "DoBBCallback");

  this->GetApplication()->Script( "pack %s -side top -anchor center -padx 2 -pady 6", 
	  this->DoUndoButtonSet->GetWidgetName());
	
  this->DoUndoButtonSet->SetEnabled(0);
  this->SelectSubsetButton->SetEnabled(0);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxEditBBGroup::EditBBApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	callback->SetState(0);
  //std::cout << "In Apply Callback" << std::endl;
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Building-block structure");
		combobox->SetValue("");
		return 0;
	}
    const char * foundationname = this->BBoxList->GetItem(num)->GetFoundationName();
// Merge Callback
if(this->RadioButtonSet->GetWidget(6)->GetSelectedState())
{
		
		if(!this->ViewMergePointsWidget)	return 0;
		if(!this->ViewMergePointsWidget->GetEnabled())	return 0;
		
		vtkUnstructuredGrid *ugrid = this->ViewMergePointsWidget->GetMergedDataSet();

		vtkUnstructuredGrid *Ugrid = vtkUnstructuredGrid::New();
		vtkMergeCells* mergecells = vtkMergeCells::New();
		mergecells->SetUnstructuredGrid(Ugrid);
		double tol = this->ToleranceEntry->GetWidget()->GetValueAsDouble();
		this->defaultMergeTolerance = tol;
		mergecells->SetPointMergeTolerance(tol);
		mergecells->SetTotalNumberOfDataSets(1);
		mergecells->SetTotalNumberOfCells(ugrid->GetNumberOfCells());
		mergecells->SetTotalNumberOfPoints(ugrid->GetNumberOfPoints());
		mergecells->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		mergecells->MergeDataSet(ugrid);
		mergecells->Finish();
		mergecells->RemoveObserver(callback);

		if (callback->GetState())
		{
			mergecells->RemoveObserver(callback);
			mergecells->Delete();
			this->RadioButtonSet->GetWidget(6)->SelectedStateOn();
			Ugrid->Delete();
			return 0;
		}
		else
		{
			vtkFieldData *fieldData = Ugrid->GetFieldData();
			if(fieldData)
			{
				vtkIntArray *meshSeed = vtkIntArray::SafeDownCast(fieldData->GetArray("Mesh_Seed"));
				if(meshSeed)	Ugrid->GetFieldData()->RemoveArray("Mesh_Seed");
			}
			//
			vtkMimxMergePoints *mergepoints = vtkMimxMergePoints::New();
			mergepoints->SetTolerance(tol);
			mergepoints->SetInput(Ugrid);
			mergepoints->Update();
			Ugrid->Delete();
			Ugrid = mergepoints->GetOutput();
			//
			this->AddEditedBB(num, Ugrid, "Merge-", this->MergeCount, foundationname);
			this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
			mergecells->RemoveObserver(callback);

			vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1))->GetDataSet();
			vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(
			  this->GetMimxMainWindow()->GetApplication());
      double edgeLength = app->GetAverageElementLength();
			vtkMimxUnstructuredGridActor::SafeDownCast(
				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1))->
				MeshSeedFromAverageElementLength(edgeLength, edgeLength, edgeLength);
			
			vtkActor *actor = this->BBoxList->GetItem(
				this->BBoxList->GetNumberOfItems()-1)->GetActor();

			vtkMimxMapOriginalCellAndPointIds *mapcellspoints = 
				vtkMimxMapOriginalCellAndPointIds::New();
			mapcellspoints->SetCompleteMesh(ugrid);
			mapcellspoints->SetPartialMesh(ugrid);
			mapcellspoints->Update();
			mapcellspoints->Delete();

			this->SelectCellsWidget->SetInputAndCurrentSelectedMesh(
				ugrid, ugrid);
			//std::cout << "EditBBApplyCallback AddActor - Merge" << std::endl;
			//this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->AddActor(
			//	this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			mergecells->Delete();
			this->RadioButtonSet->GetWidget(6)->SetSelectedState(0);
			this->RadioButtonSet->GetWidget(6)->SelectedStateOn();
			return 1;
		}
		return 0;
}


  // Split Cell Callback
  if(this->RadioButtonSet->GetWidget(1)->GetSelectedState())
  {
    if (this->ExtractEdgeWidget)
    {
	  vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::
		  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
      vtkUnstructuredGrid *ugrid = ugridactor->GetDataSet();

	  vtkIdList *edgepoints = this->ExtractEdgeWidget->GetEdgePoints();
	  if(edgepoints->GetNumberOfIds() != 2)
	  {
		  callback->ErrorMessage("Invalid edge selection Or the mouse should be positioned very close to the picked edge");
		  return 0;
	  }

	  vtkIdList *edgepointscomplete = this->ExtractEdgeWidget->GetEdgePointsCompleteGrid();
	  if(edgepointscomplete->GetNumberOfIds() != 2)
	  {
		  callback->ErrorMessage("Invalid edge selection");
		  return 0;
	  }

	  vtkMimxSplitUnstructuredHexahedronGridCell *splitpartial = 
		  vtkMimxSplitUnstructuredHexahedronGridCell::New();
	  splitpartial->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());

	  splitpartial->SetIdList(edgepoints);
	  splitpartial->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
	  splitpartial->Update();

	  if (callback->GetState())
	  {
		  splitpartial->RemoveObserver(callback);
		  splitpartial->Delete();
		  this->RadioButtonSet->GetWidget(1)->SelectedStateOn();
		  return 0;
	  }
	  else
	  {
		  this->ExtractEdgeWidget->SetEnabled(0);
		  splitpartial->RemoveObserver(callback);
	  }

	  vtkMimxSplitUnstructuredHexahedronGridCell *splitcomplete = 
		  vtkMimxSplitUnstructuredHexahedronGridCell::New();
	  splitcomplete->SetInput(ugrid);
	  splitcomplete->SetIdList(edgepointscomplete);
	  splitcomplete->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
	  splitcomplete->Update();

	  if (callback->GetState())
	  {
		  splitcomplete->RemoveObserver(callback);
		  splitcomplete->Delete();
		  this->RadioButtonSet->GetWidget(1)->SelectedStateOn();
		  return 0;
	  }
	  else
	  {
		  this->ExtractEdgeWidget->SetEnabled(0);
		  this->AddEditedBB(num, splitcomplete->GetOutput(), "Split-", this->SplitCount, foundationname);
		  //this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			//  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
		  splitcomplete->RemoveObserver(callback);

		  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1))->GetDataSet();
		  vtkActor *actor = this->BBoxList->GetItem(
			  this->BBoxList->GetNumberOfItems()-1)->GetActor();

		  vtkMimxMapOriginalCellAndPointIds *mapcellspoints = 
			  vtkMimxMapOriginalCellAndPointIds::New();
		  mapcellspoints->SetCompleteMesh(splitcomplete->GetOutput());
		  mapcellspoints->SetPartialMesh(splitpartial->GetOutput());
		  mapcellspoints->Update();
		  this->SelectCellsWidget->SetInputAndCurrentSelectedMesh(
			  ugrid, splitpartial->GetOutput());
			//std::cout << "EditBBApplyCallback AddActor - Split" << std::endl;
		  this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->AddActor(
			  this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		  this->GetMimxMainWindow()->GetRenderWidget()->Render();
		  splitcomplete->Delete();
		  splitpartial->Delete();
		  mapcellspoints->Delete();
		  
		  this->GetMimxMainWindow()->SetStatusText("Split Building Block");
		  this->RadioButtonSet->GetWidget(1)->SelectedStateOff();
		  this->RadioButtonSet->GetWidget(1)->SelectedStateOn();
		  return 1;
	  }

     }
	//this->RadioButtonSet->GetWidget(6)->SelectedStateOn();
	return 0;
  }
  
  // Add Cell Callback
  if(this->RadioButtonSet->GetWidget(2)->GetSelectedState())
  {
	  if(this->ExtractFaceWidget)
	  {	  
		  vtkMimxUnstructuredGridActor *ugridactorprev = vtkMimxUnstructuredGridActor::
			  SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));

		  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			  this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

		  vtkIntArray *intarray = vtkIntArray::SafeDownCast(
			  ugrid->GetCellData()->GetArray("Mesh_Seed"));
			  
			if ((this->ExtractFaceWidget->GetPickedCellList()->GetNumberOfIds() <= 0) || 
		      (this->ExtractFaceWidget->GetPickedFaceList()->GetNumberOfIds() <= 0))
		  {
  		  callback->ErrorMessage("Please select a face before adding a new block.");
			  this->RadioButtonSet->GetWidget(2)->SelectedStateOn();
			  return 0;
		  }

		  // for partial mesh
		  vtkMimxExtractSurface *extractpartial = vtkMimxExtractSurface::New();
		  extractpartial->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		  extractpartial->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
		  extractpartial->SetCellIdList(this->ExtractFaceWidget->GetPickedCellList());
		  extractpartial->SetFaceIdList(this->ExtractFaceWidget->GetPickedFaceList());
		  extractpartial->Update();
		
		  if (callback->GetState())
		  {
			  extractpartial->RemoveObserver(callback);
			  extractpartial->Delete();
			  this->RadioButtonSet->GetWidget(2)->SelectedStateOn();
			  return 0;
		  }
		  else
		  {
			  extractpartial->RemoveObserver(callback);
		  }

		  this->defaultExtrusionLength = this->ExtrudeEntry->GetWidget()->GetValueAsDouble();
		  vtkMimxExtrudePolyData *extrudepartial = vtkMimxExtrudePolyData::New();
		  extrudepartial->SetExtrusionLength( this->defaultExtrusionLength );
		  extrudepartial->SetInput(extractpartial->GetOutput());
		  extrudepartial->Update();

		  //vtkUnstructuredGridWriter *writer2 = vtkUnstructuredGridWriter::New();
		  //writer2->SetInput(extrudepartial->GetOutput());
		  //writer2->SetFileName("Extrude.vtk");
		  //writer2->Write();

		  vtkAppendFilter *AppendPartial = vtkAppendFilter::New();
		  AppendPartial->AddInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
		  AppendPartial->AddInput(extrudepartial->GetOutput());
		  AppendPartial->Update();

		  vtkMergeCells *mergecellspartial = vtkMergeCells::New();
		  vtkUnstructuredGrid *ugridpartial = vtkUnstructuredGrid::New();

		  mergecellspartial->SetUnstructuredGrid(ugridpartial);
		  mergecellspartial->SetTotalNumberOfDataSets(1);
		  mergecellspartial->SetTotalNumberOfCells(1000);
		  mergecellspartial->SetTotalNumberOfPoints(1000);
		  mergecellspartial->MergeDataSet(AppendPartial->GetOutput());
		  mergecellspartial->Finish();
		  mergecellspartial->Delete();
		  extrudepartial->Delete();
		  extractpartial->Delete();
		  AppendPartial->Delete();
		  // for complete mesh
		  vtkMimxExtractSurface *extractcomplete = vtkMimxExtractSurface::New();
		  extractcomplete->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		  extractcomplete->SetInput(ugrid);
		  extractcomplete->SetCellIdList(this->ExtractFaceWidget->GetCompletePickedCellList());
		  extractcomplete->SetFaceIdList(this->ExtractFaceWidget->GetCompletePickedFaceList());
		  extractcomplete->Update();

		  if (callback->GetState())
		  {
			  extractcomplete->RemoveObserver(callback);
			  extractcomplete->Delete();
			  this->RadioButtonSet->GetWidget(2)->SelectedStateOn();
			  return 0;
		  }
		  else
		  {
			  this->ExtractFaceWidget->SetEnabled(0);
			  extractcomplete->RemoveObserver(callback);
		  }

		  this->defaultExtrusionLength = this->ExtrudeEntry->GetWidget()->GetValueAsDouble();
		  vtkMimxExtrudePolyData *extrudecomplete = vtkMimxExtrudePolyData::New();
		  extrudecomplete->SetExtrusionLength( this->defaultExtrusionLength );
		  extrudecomplete->SetInput(extractcomplete->GetOutput());
		  extrudecomplete->Update();

		  vtkAppendFilter *AppendComplete = vtkAppendFilter::New();
		  AppendComplete->AddInput(ugrid);
		  AppendComplete->AddInput(extrudecomplete->GetOutput());
		  AppendComplete->Update();

		  vtkMergeCells *mergecellscomplete = vtkMergeCells::New();
		  vtkUnstructuredGrid *ugridcomplete = vtkUnstructuredGrid::New();

		  mergecellscomplete->SetUnstructuredGrid(ugridcomplete);
		  mergecellscomplete->SetTotalNumberOfDataSets(1);
		  mergecellscomplete->SetTotalNumberOfCells(1000);
		  mergecellscomplete->SetTotalNumberOfPoints(1000);
		  mergecellscomplete->MergeDataSet(AppendComplete->GetOutput());
		  mergecellscomplete->Finish();
		  mergecellscomplete->Delete();
		  extrudecomplete->Delete();
		  extractcomplete->Delete();
		  AppendComplete->Delete();

		  this->AddEditedBB(num, ugridcomplete, "Add-", this->AddCount, foundationname);
		  //this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			//  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());

		  vtkMimxUnstructuredGridActor *ugridactor = vtkMimxUnstructuredGridActor::SafeDownCast(
			  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
		  // if mesh seeds are present then copy the mesh seeds of the previous mesh and set the
		  // mesh seeds to be one for the newly added building blocks

		  ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1))->GetDataSet();


		  ugridactor = vtkMimxUnstructuredGridActor::SafeDownCast(
			  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));

		  vtkActor *actor = this->BBoxList->GetItem(
			  this->BBoxList->GetNumberOfItems()-1)->GetActor();

		  vtkMimxMapOriginalCellAndPointIds *mapcellspoints = 
			  vtkMimxMapOriginalCellAndPointIds::New();
		  mapcellspoints->SetCompleteMesh(ugridcomplete);
		  mapcellspoints->SetPartialMesh(ugridpartial);
		  mapcellspoints->Update();
		  this->SelectCellsWidget->SetInputAndCurrentSelectedMesh(
			  ugrid, ugridpartial);
			//std::cout << "EditBBApplyCallback AddActor - Add" << std::endl;
		  this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->AddActor(
			  this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		  this->GetMimxMainWindow()->GetRenderWidget()->Render();

		  if(intarray)
		  {
			  vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(
			    this->GetMimxMainWindow()->GetApplication());
        double edgeLength = app->GetAverageElementLength();
			  
			  ugridactor->MeshSeedFromAverageElementLength(edgeLength, edgeLength, edgeLength);
			  
			   int dim[3];
			   dim[0] =2; dim[1] =2; dim[2] =2;
			   int i,j;
				for (i=0; i<ugrid->GetNumberOfCells(); i++)
				{
					for (j=0; j<3; j++)
					{
						ugridactor->ChangeMeshSeed(i,j, dim[j]);
					}
				}
				
				for (i=0; i<intarray->GetNumberOfTuples(); i++)
				{
					intarray->GetTupleValue(i, dim);
					for (j=0; j<3; j++)
					{
						ugridactor->ChangeMeshSeed(i,j, dim[j]);
					}
				}
		  }

		  mapcellspoints->Delete();
		  ugridcomplete->Delete();
		  ugridpartial->Delete();		  
		  this->GetMimxMainWindow()->SetStatusText("Added Building Block");
		  this->RadioButtonSet->GetWidget(2)->SetSelectedState(0);	  
		  this->RadioButtonSet->GetWidget(2)->SelectedStateOn();
		  return 1;
	}

	  return 0;	
  }

  // Delete Cell Callback
  if(this->RadioButtonSet->GetWidget(3)->GetSelectedState())
  {
	  if(this->ExtractCellWidget)
	  {
		  if(this->ExtractCellWidget->GetPickedCellList()->GetNumberOfIds() > 0)
		  {
		  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			  this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

		  vtkMimxDeleteUnstructuredHexahedronGridCell *deletepartial = 
			  vtkMimxDeleteUnstructuredHexahedronGridCell::New();
		  deletepartial->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());


		  vtkIdList *idlist= this->ExtractCellWidget->GetPickedCellList();
		  vtkIdList *idlistComplete= vtkIdList::New();
		  idlistComplete->DeepCopy( this->ExtractCellWidget->GetCompletePickedCellList() );
		  
		  //deletepartial->SetCellNum(idlist->GetId(0));
		  deletepartial->SetCellList(idlist);
		  deletepartial->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		  deletepartial->Update();

		  if (callback->GetState())
		  {
			  deletepartial->RemoveObserver(callback);
			  deletepartial->Delete();
			  this->RadioButtonSet->GetWidget(3)->SelectedStateOn();
			  return 0;
		  }
		  else
		  {
			  deletepartial->RemoveObserver(callback);
		  }

		  vtkMimxDeleteUnstructuredHexahedronGridCell *deletecomplete = 
			  vtkMimxDeleteUnstructuredHexahedronGridCell::New();
		  
		  deletecomplete->SetInput(ugrid);
		  //deletecomplete->SetCellNum(idlistComplete->GetId(0));
		  //idlist= this->ExtractCellWidget->GetCompletePickedCellList();
		  deletecomplete->SetCellList(idlistComplete);
		  deletecomplete->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		  deletecomplete->Update();
		  
		  idlistComplete->Delete();

		  if (callback->GetState())
		  {
			  deletecomplete->RemoveObserver(callback);
			  deletecomplete->Delete();
//			  this->ExtractCellWidget->SetEnabled(0);
			  this->RadioButtonSet->GetWidget(3)->SelectedStateOn();
			  return 0;
		  }
		  else
		  {
			  this->ExtractCellWidget->SetEnabled(0);
			  this->AddEditedBB(num, deletecomplete->GetOutput(), "Delete-", this->DeleteCount, foundationname);
			  //this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
				//  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
			  deletecomplete->RemoveObserver(callback);

			  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
				  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1))->GetDataSet();
			  vtkActor *actor = this->BBoxList->GetItem(
				  this->BBoxList->GetNumberOfItems()-1)->GetActor();

			  vtkMimxMapOriginalCellAndPointIds *mapcellspoints = 
				  vtkMimxMapOriginalCellAndPointIds::New();
			  mapcellspoints->SetCompleteMesh(deletecomplete->GetOutput());
			  mapcellspoints->SetPartialMesh(deletepartial->GetOutput());
			  mapcellspoints->Update();
			  this->SelectCellsWidget->SetInputAndCurrentSelectedMesh(
				  ugrid, deletepartial->GetOutput());
				//std::cout << "EditBBApplyCallback AddActor - Delete" << std::endl;
			  this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->AddActor(
				  this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
			  this->GetMimxMainWindow()->GetRenderWidget()->Render();
			  deletecomplete->Delete();
			  deletepartial->Delete();
			  mapcellspoints->Delete();
			  this->GetMimxMainWindow()->SetStatusText("Deleted Building Block Cell");
			  this->RadioButtonSet->GetWidget(3)->SetSelectedState(0);
			  this->RadioButtonSet->GetWidget(3)->SelectedStateOn();
			  return 1;
		  }
	  }
	  }
	  //this->RadioButtonSet->GetWidget(6)->SelectedStateOn();
	  return 0;	
  }

  // Consolidate Apply Callback
  if(this->RadioButtonSet->GetWidget(4)->GetSelectedState())
  {
    
    int i, count_value;
  	vtkUnstructuredGrid *singleugrid;	
  	if (this->MultiColumnList->GetWidget()->GetNumberOfRows() > 0)
  	{
  		int count = 0;
  		for (i=0; i < this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
  		{
  			if(this->MultiColumnList->GetWidget()->
  				GetCellWindowAsCheckButton(i,1)->GetSelectedState())
  			{
  				count++;
  				if(count == 1)
  				{
  					singleugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
  						this->BBoxList->GetItem(i))->GetDataSet();
  					count_value = i;
  				}
  			}
  		}
  		if(count < 1)
  		{
  			callback->ErrorMessage("Minimum of one building block should be selected");
  			return 0;
  		}
		
  		// append the chosen items first
  		/*
  		vtkAppendFilter *AppendFilter = vtkAppendFilter::New();
  		AppendFilter->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  		*/
  		
  		vtkUnstructuredGrid *Ugrid = vtkUnstructuredGrid::New();
  		vtkMergeCells* mergecells = vtkMergeCells::New();
  		mergecells->SetUnstructuredGrid(Ugrid);
  		this->defaultMergeTolerance = this->ToleranceEntry->GetWidget()->GetValueAsDouble();
  		//std::cout << "Tolerance " << tol << std::endl;
  		mergecells->SetPointMergeTolerance((float)this->defaultMergeTolerance);
  		mergecells->MergeDuplicatePointsOn();
  		
  		int numberOfPoints = 0;
  		int numberOfCells = 0;
  		int numberOfGrids = 0;
  		
  		for (i=0; i < this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
  		{
  			if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,1)->GetSelectedState())
  			{
  			  vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
  					this->BBoxList->GetItem(i));
  			  vtkUnstructuredGrid *mergeGrid = ugridActor->GetDataSet();
  				numberOfPoints += mergeGrid->GetNumberOfPoints();
  				numberOfCells += mergeGrid->GetNumberOfCells();
  			  numberOfGrids ++;
  			}
  		}
  		
  		mergecells->SetTotalNumberOfDataSets(numberOfGrids);
  		mergecells->SetTotalNumberOfCells( numberOfCells );
  		mergecells->SetTotalNumberOfPoints( numberOfPoints );
  		mergecells->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		
		vtkUnstructuredGrid *mergeGrid = NULL;
		if(this->ViewMergePointsWidget)
		{
			if(this->ViewMergePointsWidget->GetEnabled())
			{
				mergeGrid = this->ViewMergePointsWidget->GetMergedDataSet();
			}
		}
		if(mergeGrid)
		{
			mergecells->MergeDataSet( mergeGrid );
			this->ViewMergePointsWidget->SetEnabled(0);
			//std::cout << "Has Merged Grid" << std::endl;
			for (i=0; i < this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
  			{
  				if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,1)->GetSelectedState())
  				{
  				  vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
  						this->BBoxList->GetItem(i));
  					this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp( ugridActor->GetActor() );
  					this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();  	
  				}
  			}
		}
		else
		{
  			//std::cout << "No Merged Grid" << std::endl;
  			
  			for (i=0; i < this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
  			{
  				if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,1)->GetSelectedState())
  				{
  				  vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
  						this->BBoxList->GetItem(i));
  				  mergeGrid = ugridActor->GetDataSet();
	  					
  				  mergecells->MergeDataSet( mergeGrid );
  					this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp( ugridActor->GetActor() );
  					this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();  	
  				}
  			}
		} 		
  		mergecells->Finish();
  		mergecells->RemoveObserver(callback);
  
		vtkMimxMergePoints *mergepoints = vtkMimxMergePoints::New();
		mergepoints->SetTolerance((float)this->defaultMergeTolerance);
		mergepoints->SetInput(Ugrid);
		mergepoints->Update();
		Ugrid->Delete();
		Ugrid = mergepoints->GetOutput();

  		if(!callback->GetState())
  		{
  			this->Count++;
  			this->BBoxList->AppendItem(vtkMimxUnstructuredGridActor::New());
  			this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->
  				SetDataType(ACTOR_BUILDING_BLOCK);

        vtkMimxUnstructuredGridActor *newActor = vtkMimxUnstructuredGridActor::SafeDownCast(
  				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
  			vtkUnstructuredGrid *ugrid = newActor->GetDataSet();
  		  
			// compute the merged nodes. this is used to mark the nodes that were merged
			// which in turn will be used in providing visual confirmation. a pointdata array
			// mimx_Merge will be appended to the merged dataset. the entries will either be 0
			// or 1. 0 implies the point is not merged and 1 the point is merged.
			//vtkPointLocator *locator = vtkPointLocator::New();
			//locator->SetDataSet(Ugrid);
			//vtkPoints *locatorpoints = vtkPoints::New();
			//locatorpoints->Allocate(1000);
			//locator->InitPointInsertion(locatorpoints, Ugrid->GetBounds());

			//for (i=0; i<Ugrid->GetNumberOfPoints(); i++)
			//{
			//	locator->InsertNextPoint(Ugrid->GetPoint(i));
			//}

			////vtkIntArray *intarray = vtkIntArray::New();
			////intarray->SetNumberOfTuples(Ugrid->GetNumberOfPoints());
			////intarray->SetName("mimx_Merge");
			////for (i=0; i<Ugrid->GetNumberOfPoints(); i++)
			////{
			////	intarray->SetValue(i,0);
			////}
			////Ugrid->GetPointData()->AddArray(intarray);


			//for (i=0; i < this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
			//{
			//	if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,1)->GetSelectedState())
			//	{
			//		vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
			//			this->BBoxList->GetItem(i));
			//		vtkUnstructuredGrid *mergeGrid = ugridActor->GetDataSet();
			//		int j;
			//		for (j=0; j<mergeGrid->GetNumberOfPoints(); j++)
			//		{
			//			if(locator->IsInsertedPoint(mergeGrid->GetPoint(j)) == -1)
			//			{
			//				double dist;
			//				int pointnum = locator->FindClosestPointWithinRadius(
			//					this->defaultMergeTolerance, mergeGrid->GetPoint(j), dist);
			//				//intarray->SetValue(pointnum, 1);
			//			}
			//		}
			//	}
			//}
			////intarray->Delete();
			//locator->Delete();
			//locatorpoints->Delete();
			//
			vtkFieldData *fieldData = Ugrid->GetFieldData();
			if(fieldData)
			{
				vtkIntArray *meshSeed = vtkIntArray::SafeDownCast(fieldData->GetArray("Mesh_Seed"));
				if(meshSeed)	Ugrid->GetFieldData()->RemoveArray("Mesh_Seed");
			}
  			ugrid->DeepCopy(Ugrid);
  //			Ugrid->Delete();
			this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->SetFoundationName(foundationname);
  			vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(
  				this->BBoxList->GetNumberOfItems()-1))->SetObjectName("Consolidate-",this->Count);
  			vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(
  				this->BBoxList->GetNumberOfItems()-1))->GetDataSet()->Modified();
  			this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor()->
  				GetProperty()->SetRepresentationToWireframe( );
  			this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
  				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
  			this->GetMimxMainWindow()->GetRenderWidget()->Render();
  			this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  			this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(
  				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
			  vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(
			    this->GetMimxMainWindow()->GetApplication());
        double edgeLength = app->GetAverageElementLength();
			  vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(
				  this->BBoxList->GetNumberOfItems()-1))->MeshSeedFromAverageElementLength(edgeLength, edgeLength, edgeLength); 			
				this->DoUndoTree->AppendItem(new Node);
				int num = this->DoUndoTree->GetNumberOfItems()-1;
				this->DoUndoTree->GetItem(num)->Data = 
					vtkMimxUnstructuredGridActor::SafeDownCast(
					this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
				this->DoUndoTree->GetItem(num)->Parent = NULL;
				this->DoUndoTree->GetItem(num)->Child = NULL;
				
				this->UpdateObjectLists();		
  			if ( this->SelectCellsWidget ) this->SelectCellsWidget->SetInput(ugrid);
  			this->RadioButtonSet->GetWidget(4)->SetSelectedState(0);
    		this->RadioButtonSet->GetWidget(4)->SelectedStateOn();
  			this->GetMimxMainWindow()->SetStatusText("Consolidated Building Block");
  			this->GetMimxMainWindow()->GetRenderWidget()->Render();
			this->MergeFeedBackButton->GetWidget()->SetSelectedState(0);	
			
			  newActor->ChangeMeshSeed(0, 0, newActor->GetMeshSeed(0, 0));
		    newActor->ChangeMeshSeed(0, 1, newActor->GetMeshSeed(0, 1));	
		    newActor->ChangeMeshSeed(0, 2, newActor->GetMeshSeed(0, 2));
			mergepoints->Delete();
  			return 1;
  		}
  		else
  		{
  	//		Ugrid->Delete();
			mergepoints->Delete();
  			return 0;
  		}
    
    }  
  }
  
  // Mirror Apply Callback
  if(this->RadioButtonSet->GetWidget(5)->GetSelectedState())
  {
	  if(this->MirrorPlaneWidget)
	  {
		  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			  this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		  vtkMimxMirrorUnstructuredHexahedronGridCell *mirror = 
			  vtkMimxMirrorUnstructuredHexahedronGridCell::New();
		  mirror->SetInput(ugrid);
		  vtkPlane *Plane = vtkPlane::New();
		  this->MirrorPlaneWidget->GetPlane(Plane);
		  mirror->SetMirrorPlane(Plane);
		  mirror->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		  mirror->Update();
		  vtkIdType objnum = combobox->GetValueIndex(name);
		  if (!callback->GetState())
		  {
			  this->AddEditedBB(num, mirror->GetOutput(), "Mirror-", this->MirrorCount, foundationname);
			  mirror->RemoveObserver(callback);
			  mirror->Delete();
			  Plane->Delete();
			  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
				  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1))->GetDataSet();
			  this->SelectCellsWidget->SetInput(ugrid);
			  this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			  this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
			  //this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
				//  this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
			  //this->RadioButtonSet->GetWidget(5)->SetSelectedState(0);
			  this->GetMimxMainWindow()->SetStatusText("Mirrored Building Block");
			  //this->MirrorPlaneWidget->SetEnabled(0);
			  //this->RadioButtonSet->GetWidget(5)->SelectedStateOn();
			  return 1;
		  }
		  return 0;
	  }
  }

  // Convert to Hypercube Callback
  //if(//this->RadioButtonSet->GetWidget(7)->GetSelectedState())
  //{
	 // if(this->ExtractCellWidgetHBB)
	 // {
		//  if(this->ExtractCellWidgetHBB->GetPickedCells()->GetNumberOfIds() > 0)
		//  {
		//	  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		//	  const char *name = combobox->GetValue();

		//	  int num = combobox->GetValueIndex(name);
		//	  if(num < 0 || num > combobox->GetNumberOfValues()-1)
		//	  {
		//		  callback->ErrorMessage("Choose valid Building-block structure");
		//		  combobox->SetValue("");
		//		  return 0;
		//	  }

		//	  vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
		//		  this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		//	  vtkMimxSubdivideBoundingbox * subdivide = vtkMimxSubdivideBoundingbox::New();
		//	  subdivide->SetInput(ugrid);
		//	  subdivide->SetCellNum(this->ExtractCellWidgetHBB->GetPickedCells()->GetId(0));
		//	  subdivide->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
		//	  subdivide->Update();

		//	  if (!callback->GetState())
		//	  {
		//		  this->ExtractCellWidgetHBB->SetEnabled(0);
		//		  this->AddEditedBB(num, subdivide->GetOutput(), "ConvertToHBB_", this->ConvertToHBBCount);
  //        subdivide->RemoveObserver(callback);
  //        subdivide->Delete();
  //        return 1;
		//	  }
		//	  subdivide->RemoveObserver(callback);
		//	  subdivide->Delete();
		//	  //this->RadioButtonSet->GetWidget(6)->SelectedStateOn();
		//	  return 0;
		//  }
	 // }
	 // //this->RadioButtonSet->GetWidget(6)->SelectedStateOn();
	 // return 0;	
  //}

  //
 /* if(!this->RadioButtonSet->GetWidget(1)->GetSelectedState())
  {
	callback->ErrorMessage("Choose an editing button");
  }*/
  return 0;
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBCancelCallback()
{
  int radioMode;
  int structureMode;
  this->GetUserInterfaceMode(radioMode, structureMode);
  //std::cout << "Radio Mode : " << radioMode << std::endl;
  //std::cout << "Structure Mode : " << structureMode << std::endl;
  if ((radioMode > 0 ) || (structureMode > 0))
  {
    if ( radioMode > 0) this->RadioButtonSet->GetWidget(radioMode-1)->SelectedStateOff();
    if ( structureMode > 0)  this->SelectSubsetButton->SelectedStateOff(); 
//    return;
  }
  
	//if(this->RadioButtonSet->GetWidget(2)->GetSelectedState())
	//{
	//	if(this->ExtractFaceWidget)
	//	{
	//		
	//		if(this->ExtractFaceWidget->GetEnabled())
	//		{
	//			std::cout << "Extract Face Enabled " << std::endl;
	//			this->ExtractFaceWidget->RemoveHighlightedFaces(this->ExtractFaceWidget);
	//			this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//			//return;
	//		}
	//	}
	//}
	this->CancelStatus = 1;
	this->DeselectAllButtons();
//	this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName());
	this->MainFrame->Unpack();
	this->MenuGroup->SetMenuButtonsEnabled(1);
	if(this->ExtractCellWidget)
		this->ExtractCellWidget->Delete();
	this->ExtractCellWidget = NULL;
/***VAM - Problem with the Delete here ***/
//	if(this->ExtractFaceWidget)
//		this->ExtractFaceWidget->Delete();
	this->ExtractFaceWidget = NULL;

	if(this->ExtractEdgeWidget)
		this->ExtractEdgeWidget->Delete();
	this->ExtractEdgeWidget = NULL;
	
	if(this->ExtractCellWidgetHBB)
		this->ExtractCellWidgetHBB->Delete();
	this->ExtractCellWidgetHBB = NULL;

	if(this->UnstructuredGridWidget)
		this->UnstructuredGridWidget->Delete();
	this->UnstructuredGridWidget = NULL;
	
	if(this->ViewMergePointsWidget)
	{
		this->MergeFeedBackButton->GetWidget()->SetSelectedState(0);
		this->ViewMergePointsWidget->Delete();
		this->ViewMergePointsWidget = NULL;
	}
	if(this->SelectCellsWidget)
	{
		if(this->SelectCellsWidget->GetCurrentSelectedSubsetActor())
		{
			this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
				this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		}
		this->SelectCellsWidget->SetEnabled(0);
		this->SelectCellsWidget->Delete();
		this->SelectCellsWidget = NULL;
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	}
	this->ObjectListComboBox->GetWidget()->SetStateToNormal();
	
	this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);

	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		this->CancelStatus = 0;
    return;
	}

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		this->CancelStatus = 0;
    return;
	}

  //std::cout << "Add Actor : " << combobox->GetValueIndex(name) << std::endl;
 
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
		this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor());
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();


	this->CancelStatus = 0;
	
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBDeleteCellCallback(int Mode)
{
//std::cout << "Delete Mode " << Mode << std::endl;

	if(!Mode)
	{
		this->ApplyButton->SetStateToDisabled();
		
		if(this->ExtractCellWidget)
		{
			if(this->ExtractCellWidget->GetEnabled())
			{
				this->ExtractCellWidget->SetEnabled(0);
			}
			this->ExtractCellWidget->RemoveObservers(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback);  
		}
		this->EntryFrame->Unpack();
		this->SetDoUndoButtonSelectSubsetButton();
		this->ObjectListComboBox->GetWidget()->SetStateToNormal();
		if(!this->SelectCellsWidget)	return;
		if(!this->SelectCellsWidget->GetCurrentSelectedSubsetActor()) return;
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		return;
	}

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

	this->CancelStatus = 1;
	this->EntryFrame->Unpack();
	this->CancelStatus = 0;
//	this->RadiusEntry->SetEnabled(0);
	this->SelectSubsetButton->SetEnabled(0);
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
		}
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			this->DeselectAllButtons();
			return;
		}
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

		if(!this->SelectCellsWidget)
		{
			this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
			//this->SelectCellsWidget->SetInputActor(this->BBoxList
			//	->GetItem(combobox->GetValueIndex(name))->GetActor());
			this->SelectCellsWidget->SetInput(ugrid);
			this->SelectCellsWidget->SetInteractor(
				this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
			this->SelectCellsWidget->Initialize();
		}

		//if(this->DeleteButtonState)
		//{
				if(this->ExtractCellWidget)
			{
				if(this->ExtractCellWidget->GetEnabled())
				{
					this->ExtractCellWidget->SetEnabled(0);
				}
				//this->ExtractCellWidget->Delete();
				//this->ExtractCellWidget = NULL;
			}
			else
			{
				this->ExtractCellWidget = vtkMimxExtractCellWidget::New();
			}
			this->ExtractCellWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
				->GetRenderWindowInteractor());
			//this->ExtractCellWidget->SetInputActor(this->BBoxList
			//	->GetItem(combobox->GetValueIndex(name))->GetActor());
			this->ExtractCellWidget->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
			this->ExtractCellWidget->AddObserver(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback, 1.0);  
			this->ExtractCellWidget->SetEnabled(
				this->RadioButtonSet->GetWidget(3)->GetSelectedState());
			this->DeleteButtonState = 0;
		//}
		//else
		//{
			//if(!this->ExtractCellWidget)
			//{
			//	vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			//		this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
			//	this->ExtractCellWidget = vtkMimxExtractCellWidget::New();
			//	this->ExtractCellWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
			//		->GetRenderWindowInteractor());
			//	this->ExtractCellWidget->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
			//	this->ExtractCellWidget->SetInputActor(this->BBoxList
			//		->GetItem(combobox->GetValueIndex(name))->GetActor());
			//	this->ExtractCellWidget->SetEnabled(1);
			//	this->DeleteButtonState = 0;
			//}
			//else
			//{
			//	if(!this->ExtractCellWidget->GetEnabled())
			//	{
			//		this->ExtractCellWidget->SetEnabled(1);
			//	}
			//}
		//}
			//if(this->RadioButtonSet->GetWidget(3)->GetSelectedState())
			//{
				//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
				//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
				//this->SelectSubsetButton->SetEnabled(0);
			//}
			//else
			//{
				//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
				//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
				//this->SelectSubsetButton->SetEnabled(1);
			//}
	}
	int location = 3;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}
	
	if(this->RadioButtonSet->GetWidget(3)->GetSelectedState())
	{
		this->RepackEntryFrame(3);
	}
	else
	{
		this->EntryFrame->Unpack();
	}

	if (this->MirrorPlaneWidget) this->MirrorPlaneWidget->SetEnabled(0);
		
  this->ObjectListComboBox->GetWidget()->SetStateToDisabled();
  this->ApplyButton->SetStateToNormal();
  this->SelectSubsetButton->SetEnabled(0);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBSplitCellCallback(int Mode)
{
//std::cout << "Split Mode " << Mode << std::endl;
//std::cout << "Radio Button value " << this->RadioButtonSet->GetWidget(0)->GetValueAsInt() << std::endl;
	//this->GetApplication()->Script(
	//	"pack forget %s", this->RadiusEntry->GetWidgetName());
	if(!Mode)
	{
	  if ( this->SelectSubsetButton->GetSelectedState() == 0 )
	    this->ObjectListComboBox->GetWidget()->SetStateToNormal();
		this->ApplyButton->SetStateToDisabled();
		if(this->ExtractEdgeWidget)
		{
			if(this->ExtractEdgeWidget->GetEnabled())
			{
				this->ExtractEdgeWidget->SetEnabled(0);
			}
			this->ExtractEdgeWidget->RemoveObservers(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback);  
		}
		this->EntryFrame->Unpack();
		this->SetDoUndoButtonSelectSubsetButton();
		//this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor()
		//	  ->RemoveObservers(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback);
		if(!this->SelectCellsWidget)	return;
		if(!this->SelectCellsWidget->GetCurrentSelectedSubsetActor()) return;
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		return;
	}

	this->CancelStatus = 1;
	this->EntryFrame->Unpack();
	this->CancelStatus = 0;
	this->SelectSubsetButton->SetEnabled(0);
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
			return;
		}
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			this->DeselectAllButtons();
			return;
		}
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

		if(!this->SelectCellsWidget)
		{
			this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
			//this->SelectCellsWidget->SetInputActor(this->BBoxList
			//	->GetItem(combobox->GetValueIndex(name))->GetActor());
			this->SelectCellsWidget->SetInput(ugrid);
			this->SelectCellsWidget->SetInteractor(
				this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
			//this->SelectCellsWidget->Initialize();
		}

		//if(this->SplitButtonState)
		//{
		  if(this->ExtractEdgeWidget)
		  {
			if(this->ExtractEdgeWidget->GetEnabled())
			{
			  this->ExtractEdgeWidget->SetEnabled(0);
			}
			//this->ExtractEdgeWidget->Delete();
			//this->ExtractEdgeWidget = NULL;
		  }
		  else
		  {
			  this->ExtractEdgeWidget = vtkMimxExtractEdgeWidget::New();
		  }
		  this->ExtractEdgeWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
			->GetRenderWindowInteractor());
		  this->ExtractEdgeWidget->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
		  this->ExtractEdgeWidget->AddObserver(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback, 1.0);  
		  this->ExtractEdgeWidget->SetEnabled(
			  this->RadioButtonSet->GetWidget(1)->GetSelectedState());
		  //this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor()
			 // ->AddObserver(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback);  
		  this->SplitButtonState = 0;
		//}
		//else
		//{
		 // if(!this->ExtractEdgeWidget)
		 // {
			//this->ExtractEdgeWidget = vtkMimxExtractEdgeWidget::New();
			//this->ExtractEdgeWidget->SetInteractor(this->GetMimxMainWindow()->
			//	GetRenderWidget()->GetRenderWindowInteractor());
			//this->ExtractEdgeWidget->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
			//this->ExtractEdgeWidget->SetInputActor(this->BBoxList
			//	->GetItem(combobox->GetValueIndex(name))->GetActor());
			//this->ExtractEdgeWidget->SetEnabled(1);
			//this->SplitButtonState = 0;
		 // }
		 // else
		 // {
			//if(!this->ExtractEdgeWidget->GetEnabled())
			//{
			//  this->ExtractEdgeWidget->SetEnabled(1);
			//}
		 // }
		//}
		//if(this->RadioButtonSet->GetWidget(1)->GetSelectedState())
		//{
			//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
			//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
			//this->SelectSubsetButton->SetEnabled(0);
		//}
		//else
		//{
			//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
			//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
		//	this->SelectSubsetButton->SetEnabled(1);
		//}
	}
	
	if(this->RadioButtonSet->GetWidget(1)->GetSelectedState())
	{
		this->RepackEntryFrame(1);
	}
	else
	{
		this->EntryFrame->Unpack();
	}

	if (this->MirrorPlaneWidget) this->MirrorPlaneWidget->SetEnabled(0);
	
	int location = 1;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}
	this->ObjectListComboBox->GetWidget()->SetStateToDisabled();
	this->ApplyButton->SetStateToNormal();
	this->SelectSubsetButton->SetEnabled(0);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBAddCellCallback(int Mode)
{
//std::cout << "Add Mode " << Mode << std::endl;
//std::cout << "Radio Button value " << this->RadioButtonSet->GetWidget(0)->GetValueAsInt() << std::endl;

	if(!Mode)
	{
		if ( this->SelectSubsetButton->GetSelectedState() == 0 )
		  this->ObjectListComboBox->GetWidget()->SetStateToNormal();
		this->ApplyButton->SetStateToDisabled();
		if(this->ExtractFaceWidget)
		{
			if(this->ExtractFaceWidget->GetEnabled())
			{
				this->ExtractFaceWidget->SetEnabled(0);
			}
			this->ExtractFaceWidget->RemoveObservers(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback);  	
		}
		this->EntryFrame->Unpack();
		this->SetDoUndoButtonSelectSubsetButton();
		if(!this->SelectCellsWidget)	return;
		if(!this->SelectCellsWidget->GetCurrentSelectedSubsetActor()) return;
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		return;
	}

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
		}
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			this->DeselectAllButtons();
			return;
		}

		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

		if(!this->SelectCellsWidget)
		{
			this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
			this->SelectCellsWidget->SetInput(ugrid);
			this->SelectCellsWidget->SetInteractor(
				this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
			this->SelectCellsWidget->Initialize();
		}

		//if(this->AddButtonState)
		//{
//std::cout << "Add Box: " << name << std::endl;
		  if(this->ExtractFaceWidget)
		  {
			if(this->ExtractFaceWidget->GetEnabled())
			{
			  this->ExtractFaceWidget->SetEnabled(0);
			}
			//this->ExtractFaceWidget->Delete();
			//this->ExtractFaceWidget = NULL;
		  }
		  else
		  {
			  this->ExtractFaceWidget = vtkMimxExtractFaceWidget::New();
		  }
		  this->ExtractFaceWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
			->GetRenderWindowInteractor());
		  this->ExtractFaceWidget->SetCompleteUGrid(ugrid);
		  this->ExtractFaceWidget->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
		  //this->ExtractFaceWidget->SetInputActor(this->BBoxList
			 // ->GetItem(combobox->GetValueIndex(name))->GetActor());
		  //this->ExtractFaceWidget->SetInputActor(this->BBoxList
			 // ->GetItem(combobox->GetValueIndex(name))->GetActor());
		  this->ExtractFaceWidget->AddObserver(vtkCommand::RightButtonPressEvent, this->RightButtonPressCallback, 1.0);  
		  this->ExtractFaceWidget->SetEnabled(
			  this->RadioButtonSet->GetWidget(2)->GetSelectedState());
		  this->AddButtonState = 0;
		//}
		//else
		//{
		 // if(!this->ExtractFaceWidget)
		 // {
			//vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			//  this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
			//this->ExtractFaceWidget = vtkMimxExtractFaceWidget::New();
			//this->ExtractFaceWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
			//  ->GetRenderWindowInteractor());
			//this->ExtractFaceWidget->SetInput(this->SelectCellsWidget->GetCurrentSelectedSubset());
			////this->ExtractFaceWidget->SetInputActor(this->BBoxList
			////	->GetItem(combobox->GetValueIndex(name))->GetActor());
			//this->ExtractFaceWidget->SetEnabled(1);
			//this->AddButtonState = 0;
		 // }
		 // else
		 // {
			//if(!this->ExtractFaceWidget->GetEnabled())
			//{
			//  this->ExtractFaceWidget->SetEnabled(1);
			//}
		 // }
		//}
		  //if(this->RadioButtonSet->GetWidget(2)->GetSelectedState())
		  //{
			  //this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
			  //this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
			  //this->SelectSubsetButton->SetEnabled(0);
		  //}
		  //else
		  //{
			  //this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
			  //this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
			//  this->SelectSubsetButton->SetEnabled(1);
		  //}
	}
	int location = 2;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}

	if(this->RadioButtonSet->GetWidget(2)->GetSelectedState())
	{
		this->RepackEntryFrame(2);
	}
	else
	{
		this->EntryFrame->Unpack();
	}
		if(this->MirrorPlaneWidget)
			{this->MirrorPlaneWidget->SetEnabled(0);}
			
  this->ObjectListComboBox->GetWidget()->SetStateToDisabled();
  this->ApplyButton->SetStateToNormal();
  this->SelectSubsetButton->SetEnabled(0);
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBMoveCellCallback(int Mode)
{
//std::cout << "Move Mode " << Mode << std::endl;
//std::cout << "Radio Button value " << this->RadioButtonSet->GetWidget(0)->GetValueAsInt() << std::endl;

	if(!Mode)
	{
		if ( this->SelectSubsetButton->GetSelectedState() == 0 )
		  this->ObjectListComboBox->GetWidget()->SetStateToNormal();
		this->ApplyButton->SetStateToDisabled();
		if(this->UnstructuredGridWidget)
		{
			if(this->UnstructuredGridWidget->GetEnabled())
			{
				this->UnstructuredGridWidget->SetEnabled(0);
			}
		}
		this->EntryFrame->Unpack();
    this->SphereSizeEntry->Unpack();
		this->SetDoUndoButtonSelectSubsetButton();
		if(!this->SelectCellsWidget)	return;
		if(!this->SelectCellsWidget->GetCurrentSelectedSubsetActor()) return;
		vtkPoints *points = this->SelectCellsWidget->GetCurrentSelectedSubset()->GetPoints();
		this->SelectCellsWidget->GetCurrentSelectedSubset()->GetPoints()->Modified();
		this->SelectCellsWidget->GetCurrentSelectedSubsetActor()->Modified();
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		return;
	}

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
		}
	}
	else
	{
//		this->RadiusEntry->SetEnabled(1);

 /*     if(this->MoveButtonState)
      {*/
        vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
        const char *name = combobox->GetValue();
        int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			this->DeselectAllButtons();
			return;
		}
		//
		this->SelectSubsetButton->SetEnabled(0);
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		if(!this->SelectCellsWidget)
		{
			this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
			this->SelectCellsWidget->SetInput(ugrid);
			this->SelectCellsWidget->SetInteractor(
				this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		}
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		//
         if(this->UnstructuredGridWidget)
        {
          if(this->UnstructuredGridWidget->GetEnabled())
          {
            this->UnstructuredGridWidget->SetEnabled(0);
          }
          this->UnstructuredGridWidget->Delete();
          this->UnstructuredGridWidget = NULL;
        }
        this->UnstructuredGridWidget = vtkMimxUnstructuredGridWidget::New();
        this->UnstructuredGridWidget->SetUGrid(this->SelectCellsWidget->GetCurrentSelectedSubset());
		this->UnstructuredGridWidget->SetCompleteUGrid(ugrid);
        this->UnstructuredGridWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
          ->GetRenderWindowInteractor());
/***VAM***/
		if(this->defaultSphereSizeEntry == -1.0)
		{
			this->UnstructuredGridWidget->
				ComputeHandleSize(this->UnstructuredGridWidget);
			this->Parameter = 1.0/this->UnstructuredGridWidget->GetHandleSize();
			this->SphereSizeEntry->GetWidget()->SetValueAsDouble(
				this->UnstructuredGridWidget->GetHandleSize()*this->Parameter);
		}
		else
		{
			this->SphereSizeEntry->GetWidget()->SetValueAsDouble(this->defaultSphereSizeEntry);
			this->UnstructuredGridWidget->SetHandleSize(this->SphereSizeEntry->GetWidget()->
				GetValueAsDouble()*(1.0/this->Parameter));
		}
		this->UnstructuredGridWidget->Execute(this->UnstructuredGridWidget);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		//this->SphereSizeEntry->SetEnabled(
		//	this->RadioButtonSet->GetWidget(0)->GetSelectedState());
    this->MoveButtonState = 0;
      //}
    //  else
    //  {
    //    if(!this->UnstructuredGridWidget)
    //    {
    //      vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    //      const char *name = combobox->GetValue();
    //      int num = combobox->GetValueIndex(name);
		  //if(num < 0 || num > combobox->GetNumberOfValues()-1)
		  //{
			 // callback->ErrorMessage("Choose valid Building-block structure");
			 // combobox->SetValue("");
			 // return;
		  //}
		  ////
		  //this->SelectSubsetButton->SetEnabled(0);
		  //if(!this->SelectCellsWidget)
		  //{
			 // this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
			 // this->SelectCellsWidget->SetInputActor(this->BBoxList
				//  ->GetItem(combobox->GetValueIndex(name))->GetActor());
			 // this->SelectCellsWidget->SetInteractor(
				//  this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
			 // this->SelectCellsWidget->Initialize();
		  //}
		  ////
    //      //vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
    //      //  ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
    //      this->UnstructuredGridWidget = vtkMimxUnstructuredGridWidget::New();
    //      this->UnstructuredGridWidget->SetUGrid(this->SelectCellsWidget->GetCurrentSelectedSubset());
    //      this->UnstructuredGridWidget->SetInteractor(this->GetMimxMainWindow()->
    //        GetRenderWidget()->GetRenderWindowInteractor());
    //      this->UnstructuredGridWidget->Execute();
		  //this->UnstructuredGridWidget->SetHandleSize(
			 // this->RadiusEntry->GetWidget()->GetValueAsDouble());
    //      this->UnstructuredGridWidget->SetEnabled(1);
    //      this->MoveButtonState = 0;
		  //this->RadiusEntry->SetEnabled(1);
    //    }
    //    else
    //    {
    //      if(!this->UnstructuredGridWidget->GetEnabled())
    //      {
			 // this->UnstructuredGridWidget->SetHandleSize(
				//  this->RadiusEntry->GetWidget()->GetValueAsDouble());
    //        this->UnstructuredGridWidget->SetEnabled(1);
    //      }
    //    }
    //  }

		//if(this->RadioButtonSet->GetWidget(0)->GetSelectedState())
		//{
			//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
			//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
			//this->SelectSubsetButton->SetEnabled(0);
		//}
		//else
		//{
			//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
			//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
		//	this->SelectSubsetButton->SetEnabled(1);
		//}
	}

	int location = 0;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}
	if(this->RadioButtonSet->GetWidget(0)->GetSelectedState())
	{
		this->RepackEntryFrame(0);
		//this->RadiusEntry->SetLabelText("Scale Spheres: ");
	}
	else
	{
		this->EntryFrame->Unpack();
	}
	if(this->MirrorPlaneWidget) this->MirrorPlaneWidget->SetEnabled(0);
		
	this->ObjectListComboBox->GetWidget()->SetStateToDisabled();
	this->ApplyButton->SetStateToDisabled();
	this->SelectSubsetButton->SetEnabled(0);
}

void vtkKWMimxEditBBGroup::EditBBVtkInteractionCallback(int Mode)
{
  if(this->UnstructuredGridWidget)
  {
    if (this->UnstructuredGridWidget->GetEnabled())
    {
      this->UnstructuredGridWidget->SetEnabled(0);
	  
    }
	
  }
  if(this->ExtractEdgeWidget)
  {
    if(this->ExtractEdgeWidget->GetEnabled())
    {
      this->ExtractEdgeWidget->SetEnabled(0);
    }
  }
  if(this->ExtractFaceWidget)
  {
	  if(this->ExtractFaceWidget->GetEnabled())
	  {
		  this->ExtractFaceWidget->SetEnabled(0);
	  }
  }
  if(this->ExtractCellWidget)
  {
	  if(this->ExtractCellWidget->GetEnabled())
	  {
		  this->ExtractCellWidget->SetEnabled(0);
		  return;
	  }
  }
   if(this->ExtractCellWidgetHBB)
  {
	  if(this->ExtractCellWidgetHBB->GetEnabled())
	  {
		  this->ExtractCellWidgetHBB->SetEnabled(0);
		  return;
	  }
  }
  
   if(this->SelectCellsWidget)
   {
	   if(this->SelectCellsWidget->GetCurrentSelectedSubsetActor())
	   {
		   this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			   this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
	   }
	   if(this->SelectCellsWidget->GetCurrentSelectedSubsetActor())
	   {
		   this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			   this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
	   }
	   this->SelectCellsWidget->SetEnabled(0);
	   //this->SelectCellsWidget->Delete();
	   //this->SelectCellsWidget = NULL;
	   this->GetMimxMainWindow()->GetRenderWidget()->Render();
   }

   vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  if(strcmp(combobox->GetValue(),""))
  {
	  const char *name = combobox->GetValue();
	  int num = combobox->GetValueIndex(name);
/*
std::cout << "Item Name: " << name << std::endl;
std::cout << "Item Index: " << num << std::endl;
std::cout << "Item: " << this->DoUndoTree->GetItem(name) << std::endl;
std::cout << "Item 0: " << this->DoUndoTree->GetItem(static_cast<vtkIdType>(0)) << std::endl;
std::cout << "Item 1: " << this->DoUndoTree->GetItem(static_cast<vtkIdType>(1)) << std::endl;
std::cout << "Call Get GetItemNumber " << this->DoUndoTree->GetItemNumber(name) << std::endl;
std::cout << "Item Child: " << this->DoUndoTree->GetItem(name)->Child << std::endl;
*/
	  if(this->DoUndoTree->GetItem(name)->Child != NULL)
	  {
		  this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
	  }
	  else{
		  this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
	  }
	  if(this->DoUndoTree->GetItem(name)->Parent != NULL)
	  {
		  this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
	  }
	  else
	  {
		  this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
	  }
  }
  else
  {
	    this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
		this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
  }
  this->CancelStatus = 1;
  this->EntryFrame->Unpack();
  this->CancelStatus = 0;
  //	this->RadiusEntry->SetEnabled(0);
  //this->SelectSubsetButton->SetEnabled(1);
}
//-----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
	strcpy(this->objectSelectionPrevious,"");
	int defaultItem = -1;
	for (int i = 0; i < this->BBoxList->GetNumberOfItems(); i++)
	{
		ObjectListComboBox->GetWidget()->AddValue(
			this->BBoxList->GetItem(i)->GetFileName());
		int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
			this->BBoxList->GetItem(i)->GetActor());
		if ( (defaultItem == -1) && ( viewedItem ) )
		{
		  defaultItem = i;
		}
	}
	//std::cout << "Default Item : " << defaultItem << std::endl;
	if ((defaultItem == -1) && (this->BBoxList->GetNumberOfItems() > 0))
	  defaultItem = 0;
	  
	if (defaultItem != -1)
  {
    ObjectListComboBox->GetWidget()->SetValue(
          this->BBoxList->GetItem(defaultItem)->GetFileName());
	this->SelectSubsetButton->SetEnabled(1);
	strcpy(this->objectSelectionPrevious, this->ObjectListComboBox->GetWidget()->GetValue());
  }
	else
	{
		ObjectListComboBox->GetWidget()->SetValue("");
		this->SelectSubsetButton->SetEnabled(0);
	}
  this->SetDoUndoButtonSelectSubsetButton(); 
  return;
}
//--------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::RadiusChangeCallback(const char *dummy)
{
#if 0
	if(this->CancelStatus)
	{
		return;
	}
	
  if(this->UnstructuredGridWidget)
	{
		if(this->UnstructuredGridWidget->GetEnabled())
		{
			this->UnstructuredGridWidget->SetEnabled(0);
			this->UnstructuredGridWidget->SetHandleSize(this->RadiusEntry->
				GetWidget()->GetValueAsDouble()*(1.0/this->Parameter));
			this->defaultRadiusEntry = this->RadiusEntry->GetWidget()->GetValueAsDouble();
			this->UnstructuredGridWidget->SetEnabled(1);
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
		}
	}
	
	if(this->ViewMergePointsWidget)
	{
		if(this->ViewMergePointsWidget->GetEnabled())
		{
			this->ViewMergePointsWidget->SetEnabled(0);
			this->ViewMergePointsWidget->SetHandleSize(this->RadiusEntry->
				GetWidget()->GetValueAsDouble()*(1.0/this->Parameter));
			this->defaultRadiusEntry = this->RadiusEntry->GetWidget()->GetValueAsDouble();
			this->ViewMergePointsWidget->SetEnabled(1);
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
		}
	}
#endif
}
//--------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBDoneCallback()
{
	if(this->EditBBApplyCallback())
		this->EditBBCancelCallback();
}
//---------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBMirrorCallback(int Mode)
{
	if(!Mode)
	{
		this->ApplyButton->SetStateToDisabled();
		this->MirrorFrame->Unpack();
		this->SetDoUndoButtonSelectSubsetButton();
		this->ObjectListComboBox->GetWidget()->SetStateToNormal();
		if(this->MirrorPlaneWidget)
		{this->MirrorPlaneWidget->SetEnabled(0);}
		if(!this->SelectCellsWidget)	return;
		if(!this->SelectCellsWidget->GetCurrentSelectedSubsetActor()) return;
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		return;
	}

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
			return;
		}
		return;
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		if(!this->SelectCellsWidget)
		{
			this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
			this->SelectCellsWidget->SetInteractor(
				this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		}
		this->SelectCellsWidget->SetInput(ugrid);
		//
		//if(this->RadioButtonSet->GetWidget(4)->GetSelectedState())
		//{
			//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
			//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
			//this->SelectSubsetButton->SetEnabled(0);
		//}
		//else
		//{
			//this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
			//this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
			//this->SelectSubsetButton->SetEnabled(1);
		//}
	}

	int location = 5;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}
	this->RepackEntryFrame(5);
	this->RepackMirrorFrame();
	this->PlaceMirroringPlane();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	this->ApplyButton->SetStateToNormal();
	this->SelectSubsetButton->SetEnabled(0);
	this->ObjectListComboBox->GetWidget()->SetStateToDisabled();
}
//----------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBMergeCallback(int Mode)
{
	if(!Mode)
	{
		this->ApplyButton->SetStateToDisabled();
		this->SetDoUndoButtonSelectSubsetButton();
		this->ObjectListComboBox->GetWidget()->SetStateToNormal();
		if(this->ViewMergePointsWidget)
		{
			if(this->ViewMergePointsWidget->GetEnabled())
			{
				this->ViewMergePointsWidget->SetEnabled(0);
			}
		}
		this->EntryFrame->Unpack();
		this->SphereSizeFrame->Unpack();
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
		{
			if(!this->CancelStatus)
			{
				return;
			}
		}
		
		if(!this->SelectCellsWidget)
		{
			this->ViewProperties->UpdateVisibilityList();
			this->MimxMainWindow->GetRenderWidget()->Render();
			return;
		}
		if(!this->SelectCellsWidget->GetCurrentSelectedSubsetActor()) return;
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());

		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::
			SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(ugridActor->GetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		return;
	}


	int location = 6;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}
	this->SelectSubsetButton->SetEnabled(0);
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
			return;
		}
	}

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::
		SafeDownCast(this->BBoxList->GetItem(combobox->GetValueIndex(name)));
	vtkUnstructuredGrid *ugrid = ugridActor->GetDataSet();

	if(!this->SelectCellsWidget)
	{
		this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
		this->SelectCellsWidget->SetInteractor(
			this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	}
	this->SelectCellsWidget->SetInput(ugrid);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		this->SelectCellsWidget->GetCurrentSelectedSubsetActor());

	this->MimxMainWindow->GetRenderWidget()->RemoveViewProp(ugridActor->GetActor());
	this->RepackEntryFrame(6);
//	this->RepackSphereSizeFrame();
	//this->RadiusEntry->SetLabelText("Merge Tolerance: ");
	//this->RadiusEntry->GetWidget()->SetValueAsDouble( defaultMergeTolerance );
	//this->RadiusEntry->SetEnabled(1);

	if(!this->ViewMergePointsWidget)
	{
		this->ViewMergePointsWidget = vtkMimxViewMergePointsWidget::New();
	}
	this->ViewMergePointsWidget->SetInteractor(
		this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	this->DataSetCollection->RemoveAllItems();
	this->DataSetCollection->AddItem(ugrid);

	//this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(ugridActor->GetActor());		
	this->ViewMergePointsWidget->SetInput(this->DataSetCollection);
	this->ViewMergePointsWidget->SetTolerance(this->ToleranceEntry->GetWidget()->GetValueAsDouble());

	//std::cout << "Default Size Entry: " << this->defaultSphereSizeEntry << std::endl;
	
	if (this->defaultSphereSizeEntry == -1.0)
	{
		this->ViewMergePointsWidget->
			ComputeHandleSize(this->ViewMergePointsWidget);
		this->Parameter = 1.0/this->ViewMergePointsWidget->GetHandleSize();
		this->SphereSizeEntry->GetWidget()->SetValueAsDouble(
			this->ViewMergePointsWidget->GetHandleSize()*this->Parameter);
	}
	else
	{
		this->SphereSizeEntry->GetWidget()->SetValueAsDouble(this->defaultSphereSizeEntry);
		this->ViewMergePointsWidget->SetHandleSize(this->SphereSizeEntry->GetWidget()->
			GetValueAsDouble()*(1.0/this->Parameter));
	}
	
  this->ObjectListComboBox->GetWidget()->SetStateToDisabled();

	if(this->MirrorPlaneWidget)
		{this->MirrorPlaneWidget->SetEnabled(0);}
	this->ApplyButton->SetStateToNormal();
	this->SelectSubsetButton->SetEnabled(0);
	
	if(!this->ViewMergePointsWidget->GetEnabled())
	{
		this->ViewMergePointsWidget->SetEnabled(1);
	}
	this->GetMimxMainWindow()->GetRenderWidget()->Render();	
}
//---------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditBBConsolidateBlockCallback(int Mode)
{
	if (!Mode)
	{
		this->MergeFeedBackButton->GetWidget()->SetSelectedState(0);
		this->MergeFrame->Unpack();
		this->EntryFrame->Unpack();
		this->SphereSizeEntry->Unpack();
		this->ApplyButton->SetEnabled(1);
		this->ObjectListComboBox->GetWidget()->SetStateToNormal();
		return;
	}


	
	int location = 4;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}
	
	if(this->MirrorPlaneWidget) this->MirrorPlaneWidget->SetEnabled(0);	
	this->ObjectListComboBox->GetWidget()->SetStateToDisabled();
	this->ApplyButton->SetStateToNormal();
	this->SelectSubsetButton->SetEnabled(0);
	
	this->RepackEntryFrame(4);
	this->RepackMergeFrame();
}

//------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::EditConvertToHBBCallback(int Mode)
{
	this->CancelStatus = 1;
	this->EntryFrame->Unpack();
	this->CancelStatus = 0;
	//	this->RadiusEntry->SetEnabled(0);
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
		}
	}
	else
	{
		if(this->ConvertToHBBButtonState)
		{
			vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
			const char *name = combobox->GetValue();
			int num = combobox->GetValueIndex(name);
			if(num < 0 || num > combobox->GetNumberOfValues()-1)
			{
				callback->ErrorMessage("Choose valid Building-block structure");
				combobox->SetValue("");
				this->DeselectAllButtons();
				return;
			}
			vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
				->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
			if(this->ExtractCellWidgetHBB)
			{
				if(this->ExtractCellWidgetHBB->GetEnabled())
				{
					this->ExtractCellWidgetHBB->SetEnabled(0);
				}
				this->ExtractCellWidgetHBB->Delete();
				this->ExtractCellWidgetHBB = NULL;
			}
			this->ExtractCellWidgetHBB = vtkMimxExtractCellWidget::New();
			this->ExtractCellWidgetHBB->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
				->GetRenderWindowInteractor());
			this->ExtractCellWidgetHBB->SetInput(ugrid);
			this->ExtractCellWidgetHBB->SetInputActor(this->BBoxList
				->GetItem(combobox->GetValueIndex(name))->GetActor());
			this->ExtractCellWidgetHBB->SetEnabled(1);
			this->ConvertToHBBButtonState = 0;
		}
		else
		{
			if(!this->ExtractCellWidgetHBB)
			{
				vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
				const char *name = combobox->GetValue();
				int num = combobox->GetValueIndex(name);
				if(num < 0 || num > combobox->GetNumberOfValues()-1)
				{
					callback->ErrorMessage("Choose valid Building-block structure");
					combobox->SetValue("");
					this->DeselectAllButtons();
					return;
				}
				vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
					this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
				this->ExtractCellWidgetHBB = vtkMimxExtractCellWidget::New();
				this->ExtractCellWidgetHBB->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
					->GetRenderWindowInteractor());
				this->ExtractCellWidgetHBB->SetInput(ugrid);
				this->ExtractCellWidgetHBB->SetInputActor(this->BBoxList
					->GetItem(combobox->GetValueIndex(name))->GetActor());
				this->ExtractCellWidgetHBB->SetEnabled(1);
				this->ConvertToHBBButtonState = 0;
			}
			else
			{
				if(!this->ExtractCellWidgetHBB->GetEnabled())
				{
					this->ExtractCellWidgetHBB->SetEnabled(1);
				}
			}
		}
		this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);
		this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
	}
	if(UnstructuredGridWidget)
	{
		if(UnstructuredGridWidget->GetEnabled())
		{
			UnstructuredGridWidget->SetEnabled(0);
			return;
		}
	}
	if(this->ExtractEdgeWidget)
	{
		if(this->ExtractEdgeWidget->GetEnabled())
		{
			this->ExtractEdgeWidget->SetEnabled(0);
			return;
		}
	}
	if(this->ExtractFaceWidget)
	{
		if(this->ExtractFaceWidget->GetEnabled())
		{
			this->ExtractFaceWidget->SetEnabled(0);
			return;
		}
	}

	if(this->ExtractCellWidget)
	{
		if(this->ExtractCellWidget->GetEnabled())
		{
			this->ExtractCellWidget->SetEnabled(0);
			return;
		}
	}
}
//----------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::DoBBCallback()
{
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	if(strcmp(combobox->GetValue(),""))
	{
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(this->DoUndoTree->GetItem(name)->Child != NULL)
		{
			double defaultSize = this->defaultSphereSizeEntry;
			Node *childnode = this->DoUndoTree->GetItem(name)->Child;
			combobox->DeleteValue(num);
			combobox->AddValue(childnode->Data->GetFileName());
			combobox->SetValue(childnode->Data->GetFileName());
			this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
				this->BBoxList->GetItem(num)->GetActor());
			this->BBoxList->RemoveItem(num);
			this->BBoxList->AppendItem(childnode->Data);
			this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
			this->GetMimxMainWindow()->GetViewProperties()->DeleteObjectList(5, num);
			this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(
				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
			this->BBSelectionChangedCallback(combobox->GetValue());
			this->defaultSphereSizeEntry = defaultSize;
		}
	}
}
//----------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::UndoBBCallback()
{
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	if(strcmp(combobox->GetValue(),""))
	{
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(this->DoUndoTree->GetItem(name)->Parent != NULL)
		{
			//vtkPlane *plane = vtkPlane::New();
			//if (this->MirrorPlaneWidget)
			//  this->MirrorPlaneWidget->GetPlane( plane );
			double defaultSize = this->defaultSphereSizeEntry;
			Node *parentnode = this->DoUndoTree->GetItem(name)->Parent;
			combobox->DeleteValue(num);
			combobox->AddValue(parentnode->Data->GetFileName());
			combobox->SetValue(parentnode->Data->GetFileName());
			this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
				this->BBoxList->GetItem(num)->GetActor());
			this->BBoxList->RemoveItem(num);
			this->BBoxList->AppendItem(parentnode->Data);
			this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
			this->GetMimxMainWindow()->GetViewProperties()->DeleteObjectList(5, num);
			this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(
				this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
			this->BBSelectionChangedCallback(combobox->GetValue());
			//if (this->MirrorPlaneWidget)
			//  this->MirrorPlaneWidget->SetPlane( plane );
			//plane->Delete();
			this->defaultSphereSizeEntry = defaultSize;
		}
	}
}
//----------------------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::AddEditedBB(int BBNum, vtkUnstructuredGrid *Output, 
									   const char* Name, vtkIdType& Count, const char *foundationname)
{
	////// new code
	// create a new node and store the latest building block structure
	Node *chosennode = this->DoUndoTree->GetItem(
		this->BBoxList->GetItem(BBNum)->GetFileName());
	// enable/disable do/undo buttons
	char *chosennodename = chosennode->Data->GetFileName();
	this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
	this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
	//
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		this->BBoxList->GetItem(BBNum)->GetActor());


	// delete all the building-blocks after the current one
	while (chosennode->Child != NULL)
	{
		chosennode = chosennode->Child;
	}

	while (strcmp(chosennodename, chosennode->Data->GetFileName()))
	{
		Node *currnode = chosennode;
		chosennode = chosennode->Parent;
		this->DoUndoTree->RemoveItem(
			this->DoUndoTree->GetItemNumber(currnode->Data->GetFileName()));
		currnode->Data->Delete();
		delete currnode;
		chosennode->Child = NULL;
	}
	this->DoUndoTree->AppendItem(new Node);
	this->BBoxList->AppendItem(vtkMimxUnstructuredGridActor::New());
	this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->SetFoundationName(foundationname);
	this->DoUndoTree->GetItem(this->DoUndoTree->GetNumberOfItems()-1)
		->Data = vtkMimxUnstructuredGridActor::SafeDownCast(
		this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
	this->DoUndoTree->GetItem(this->DoUndoTree->GetNumberOfItems()-1)
		->Parent = chosennode;
	chosennode->Child = this->DoUndoTree->GetItem(this->DoUndoTree->GetNumberOfItems()-1);
	this->DoUndoTree->GetItem(this->DoUndoTree->GetNumberOfItems()-1)
		->Child = NULL;
	this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->
		SetDataType(ACTOR_BUILDING_BLOCK);
	////
	vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(
		this->BBoxList->GetNumberOfItems()-1))->GetDataSet()->
		DeepCopy(Output);
	Count++;
	vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(
		this->BBoxList->GetNumberOfItems()-1))->SetObjectName(Name,Count);
	strcpy(this->objectSelectionPrevious, vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(
		this->BBoxList->GetNumberOfItems()-1))->GetFileName()); 
	vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(
		this->BBoxList->GetNumberOfItems()-1))->GetDataSet()->Modified();
	//
	vtkMimxUnstructuredGridActor *newActor = vtkMimxUnstructuredGridActor::SafeDownCast(
	   this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
	vtkMimxUnstructuredGridActor *oldActor = vtkMimxUnstructuredGridActor::SafeDownCast(
	   this->BBoxList->GetItem(BBNum));
	this->CopyActorProperties(  newActor, oldActor ); 	
	
	//
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
		this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor());
	this->ObjectListComboBox->GetWidget()->SetValue(
		this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetFileName());
	this->ObjectListComboBox->GetWidget()->AddValue(
		this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetFileName());
	//this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1)->GetActor()->GetProperty()
	//	->SetColor(this->BBoxList->GetItem(BBNum)->GetActor()->GetProperty()->GetColor());
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//        this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
	this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(
		this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
	// remove the parent from the all the lists
	this->BBoxList->RemoveItem(BBNum);
	this->ObjectListComboBox->GetWidget()->DeleteValue(BBNum);
	this->GetMimxMainWindow()->GetViewProperties()->DeleteObjectList(5, BBNum);
	//
	//this->RadioButtonSet->GetWidget(6)->SelectedStateOn();
	this->BBSelectionChangedCallback(NULL);
	
	/* Update the Mesh Seed to force progation to New Cell */
	/* Should be handled in the Unstructured Grid Filters
	vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
			    this->BBoxList->GetItem(this->BBoxList->GetNumberOfItems()-1));
	int xSeed = ugridActor->GetMeshSeed(0, 0);
	int ySeed = ugridActor->GetMeshSeed(0, 1);
	int zSeed = ugridActor->GetMeshSeed(0, 2);
	ugridActor->ChangeMeshSeed(0, 0, xSeed);	
	ugridActor->ChangeMeshSeed(0, 1, ySeed);	
	ugridActor->ChangeMeshSeed(0, 2, zSeed);	 
	*/    
	//
}
//---------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::ApplySelectSubsetCallback()
{
//	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
//	if(this->SelectCellsWidget)
//	{
//		if (this->SelectCellsWidget->GetEnabled())
//		{
////			this->SelectCellsWidget->AcceptSelectedMesh();
//			this->SelectCellsWidget->SetEnabled(0);
//			this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->AddActor(
//				this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
//			this->GetMimxMainWindow()->GetRenderWidget()->Render();
//		}
//		else
//		{
//			callback->ErrorMessage("Enable cells selections");
//		}
//	}
}
//---------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::SelectSubsetCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
		}
	}
	else{
			vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
			const char *name = combobox->GetValue();
			int num = combobox->GetValueIndex(name);
			if(num < 0 || num > combobox->GetNumberOfValues()-1)
			{
				callback->ErrorMessage("Choose valid Building-block structure");
				combobox->SetValue("");
				this->DeselectAllButtons();
				return;
			}
			vtkActor *actor = this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor();
			vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
				this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

			if(!this->SelectCellsWidget)
			{
				this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
			}
			this->SelectCellsWidget->SetInput(ugrid);
			this->SelectCellsWidget->SetInteractor(
				this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
			this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->RemoveActor(actor);
			this->SelectCellsWidget->SetEnabled(1);
	}

}
//---------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::CancelSelectSubsetCallback()
{
	//if(this->SelectCellsWidget)
	//{
	//	if (this->SelectCellsWidget->GetEnabled())
	//	{
	//		this->SelectCellsWidget->SetEnabled(0);
	//		this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->AddActor(
	//			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
	//		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//	}
	//}
}
//---------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::SelectFullSetCallback(int mode)
{
//std::cout << "Select Mode " << mode << std::endl;
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
		}
	}
	
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Building-block structure");
		combobox->SetValue("");
		this->DeselectAllButtons();
		return;
	}
  		

	if ( mode )
	{
  	vtkActor *actor = this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor();
  	vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
  		this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

  	if(!this->SelectCellsWidget)
  	{
  		this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
  	}
  	this->SelectCellsWidget->SetInput(ugrid);
  	this->SelectCellsWidget->SetInteractor(
  		this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
  	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->RemoveActor(actor);
  	this->SelectCellsWidget->SetEnabled(1);
  	
  	/* Currently Deselect the Mirror and Merge Options when selecting a subset
  	   of the mesh - Should be re-enabled eventually */
  	this->RadioButtonSet->GetWidget(4)->SetStateToDisabled( );
  	this->RadioButtonSet->GetWidget(5)->SetStateToDisabled( );
	this->RadioButtonSet->GetWidget(6)->SetStateToDisabled();
  	this->ObjectListComboBox->GetWidget()->SetStateToDisabled();
	}
	else
	{
		vtkActor *actor = this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor();
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
			this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		if (this->SelectCellsWidget)
		{
				this->SelectCellsWidget->SetInput(ugrid);
				this->SelectCellsWidget->SetEnabled(0);
				this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->RemoveActor(
					this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		}
		this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->AddActor(actor);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
		
		/* Enable the Mirror and Merge Options when in Full mode */
		this->RadioButtonSet->GetWidget(4)->SetStateToNormal( );
  	this->RadioButtonSet->GetWidget(5)->SetStateToNormal( );
	this->RadioButtonSet->GetWidget(6)->SetStateToNormal();
  	this->ObjectListComboBox->GetWidget()->SetStateToNormal();
	}	
}
//---------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::DeselectAllButtons()
{
	int i;
	for (i=0; i<7; i++)
	{
		if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
	}
	if(this->MirrorPlaneWidget)
		{this->MirrorPlaneWidget->SetEnabled(0);}
	this->EntryFrame->Unpack();
	this->SelectSubsetButton->SetEnabled(1);
}
//----------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::SetDoUndoButtonSelectSubsetButton()
{
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		this->DoUndoButtonSet->SetEnabled(0);
		this->SelectSubsetButton->SetEnabled(0);
		return;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	this->SelectSubsetButton->SetEnabled(1);
				
	if(this->DoUndoTree->GetItem(name)->Parent != NULL)	
		this->DoUndoButtonSet->GetWidget(0)->SetEnabled(1);
	else
		this->DoUndoButtonSet->GetWidget(0)->SetEnabled(0);

	if(this->DoUndoTree->GetItem(name)->Child != NULL)	
		this->DoUndoButtonSet->GetWidget(1)->SetEnabled(1);
	else
		this->DoUndoButtonSet->GetWidget(1)->SetEnabled(0);
		
}
//----------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::RepackEntryFrame(int type)
{
	this->GetApplication()->Script(
		"pack %s -side top -anchor center -expand n -padx 2 -pady 2 -fill x -after %s", 
		this->EntryFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());

  switch (type)
  {
    case 0:
      this->GetApplication()->Script( "pack forget %s", this->ExtrudeEntry->GetWidgetName());
      this->GetApplication()->Script( "pack forget %s", this->ToleranceEntry->GetWidgetName());
      this->RepackSphereSizeFrame();
      //this->GetApplication()->Script(
    	//	"pack %s -side top -anchor center -expand 0 -padx 2 -pady 6", 
    	//	this->SphereSizeEntry->GetWidgetName());
    	break;
    case 1:
      this->GetApplication()->Script( "pack forget %s", this->ToleranceEntry->GetWidgetName());
      this->GetApplication()->Script( "pack forget %s", this->ExtrudeEntry->GetWidgetName());
      this->GetApplication()->Script( "pack forget %s", this->SphereSizeEntry->GetWidgetName());
      break;
    case 2:
       this->GetApplication()->Script( "pack forget %s", this->SphereSizeEntry->GetWidgetName());
       this->GetApplication()->Script( "pack forget %s", this->ToleranceEntry->GetWidgetName());
       this->GetApplication()->Script(
    		"pack %s -side top -anchor center -expand 0 -padx 2 -pady 6", 
    		this->ExtrudeEntry->GetWidgetName());
    	 break;
    case 3:
      this->GetApplication()->Script( "pack forget %s", this->ToleranceEntry->GetWidgetName());
      this->GetApplication()->Script( "pack forget %s", this->ExtrudeEntry->GetWidgetName());
      this->GetApplication()->Script( "pack forget %s", this->SphereSizeEntry->GetWidgetName());
      break;
    case 5:
      this->GetApplication()->Script( "pack forget %s", this->ToleranceEntry->GetWidgetName());
      this->GetApplication()->Script( "pack forget %s", this->ExtrudeEntry->GetWidgetName());
      this->GetApplication()->Script( "pack forget %s", this->SphereSizeEntry->GetWidgetName());
      break;
    case 4:
    case 6:
      this->GetApplication()->Script( "pack forget %s", this->ExtrudeEntry->GetWidgetName());
      this->RepackSphereSizeFrame();
      //this->GetApplication()->Script(
    	//	"pack %s -side top -anchor center -expand 0 -padx 2 -pady 6", 
    	//	this->SphereSizeEntry->GetWidgetName());
    	this->GetApplication()->Script(
    		"pack %s -side top -anchor center -expand 0 -padx 2 -pady 6", 
    		this->ToleranceEntry->GetWidgetName());
    	break;
  }	
}
//-----------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::PlaceMirroringPlaneAboutX()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Building Block selection required");
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			return;
		}
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		vtkActor *actor = this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor();
		if(!this->MirrorPlaneWidget)
		{
			this->MirrorPlaneWidget = vtkPlaneWidget::New();
			double bounds[6];
			ugrid->GetBounds(bounds);
			this->MirrorPlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
				->GetRenderWindowInteractor());
			this->MirrorPlaneWidget->SetProp3D(actor);
			this->MirrorPlaneWidget->PlaceWidget(bounds);
			/***VAM
			 *     Image plane widget appears to be resizing the Handle Size
			 ***/
			double handleSize = this->MirrorPlaneWidget->GetHandleSize();
			handleSize /= 2.0;
			this->MirrorPlaneWidget->SetHandleSize( handleSize );
			/***/
		}
		else
		{
			double center[3];
			this->MirrorPlaneWidget->SetEnabled(0);
			this->MirrorPlaneWidget->GetOrigin(center);
			this->MirrorPlaneWidget->SetProp3D(actor);
			this->MirrorPlaneWidget->SetOrigin(center);
		}
		this->MirrorPlaneWidget->SetNormal(1.0,0.0,0.0);
		//this->MirrorPlaneWidget->NormalToXAxisOn();
		this->MirrorPlaneWidget->SetEnabled(1);
	}
}
//----------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::PlaceMirroringPlaneAboutY()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Building Block selection required");
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			return;
		}
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		vtkActor *actor = this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor();
		if(!this->MirrorPlaneWidget)
		{
			this->MirrorPlaneWidget = vtkPlaneWidget::New();
			double bounds[6];
			ugrid->GetBounds(bounds);
			this->MirrorPlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
				->GetRenderWindowInteractor());
			this->MirrorPlaneWidget->SetProp3D(actor);
			this->MirrorPlaneWidget->PlaceWidget(bounds);
		}
		else
		{
			double center[3];
			this->MirrorPlaneWidget->SetEnabled(0);
			this->MirrorPlaneWidget->GetOrigin(center);
			this->MirrorPlaneWidget->SetProp3D(actor);
			this->MirrorPlaneWidget->SetOrigin(center);
		}
		this->MirrorPlaneWidget->SetNormal(0.0,1.0,0.0);
		this->MirrorPlaneWidget->SetEnabled(1);
	}

}
//----------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::PlaceMirroringPlaneAboutZ()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Building Block selection required");
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			return;
		}
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		vtkActor *actor = this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor();
		if(!this->MirrorPlaneWidget)
		{
			this->MirrorPlaneWidget = vtkPlaneWidget::New();
			double bounds[6];
			ugrid->GetBounds(bounds);
			this->MirrorPlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
				->GetRenderWindowInteractor());
			this->MirrorPlaneWidget->SetProp3D(actor);
			this->MirrorPlaneWidget->PlaceWidget(bounds);
		}
		else
		{
			double center[3];
			this->MirrorPlaneWidget->SetEnabled(0);
			this->MirrorPlaneWidget->GetOrigin(center);
			this->MirrorPlaneWidget->SetProp3D(actor);
			this->MirrorPlaneWidget->SetOrigin(center);
		}
		this->MirrorPlaneWidget->SetNormal(0.0,0.0,1.0);
		this->MirrorPlaneWidget->SetEnabled(1);
	}
}
//----------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::PlaceMirroringPlane()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Building Block selection required");
	}
	else
	{
		vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		int num = combobox->GetValueIndex(name);
		if(num < 0 || num > combobox->GetNumberOfValues()-1)
		{
			callback->ErrorMessage("Choose valid Building-block structure");
			combobox->SetValue("");
			return;
		}
		vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList
			->GetItem(combobox->GetValueIndex(name)))->GetDataSet();
		vtkActor *actor = this->BBoxList->GetItem(combobox->GetValueIndex(name))->GetActor();
		if(!this->MirrorPlaneWidget)
		{
			this->MirrorPlaneWidget = vtkPlaneWidget::New();
			double bounds[6];
			ugrid->GetBounds(bounds);
			this->MirrorPlaneWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()
				->GetRenderWindowInteractor());
			this->MirrorPlaneWidget->SetProp3D(actor);
			this->MirrorPlaneWidget->PlaceWidget(bounds);
			this->MirrorPlaneWidget->SetNormal(0.0,0.0,1.0);
		}
		this->MirrorPlaneWidget->SetEnabled(1);
	}
}

//----------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::RepackMirrorFrame()
{
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s", 
		this->MirrorFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());
/*
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
		this->TypeOfMirroring->GetWidgetName());
*/
	this->GetApplication()->Script(
		"pack %s -side top -anchor n -padx 2 -pady 2", 
		this->AxisSelection->GetWidgetName());
	
//	this->AxisSelection->GetWidget(0)->SelectedStateOff( );
//  this->AxisSelection->GetWidget(1)->SelectedStateOff( );
//  this->AxisSelection->GetWidget(2)->SelectedStateOff( );
  
}

//----------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::RepackMergeFrame()
{
	this->MultiColumnList->GetWidget()->DeleteAllRows();
	int localcount = 0;
	for (int i=0; i < this->BBoxList->GetNumberOfItems(); i++)
	{
		vtkMimxUnstructuredGridActor *actor = vtkMimxUnstructuredGridActor::
			SafeDownCast(this->BBoxList->GetItem(i));
		vtkUnstructuredGrid *ugrid = actor->GetDataSet();
		this->MultiColumnList->GetWidget()->InsertCellText(i, 0, actor->GetFileName());
		this->MultiColumnList->GetWidget()->InsertCellTextAsInt(i, 1, 0);
		this->MultiColumnList->GetWidget()->SetCellWindowCommandToCheckButton(i, 1);
	}
	
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s", 
		this->MergeFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());	
}
//----------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::GetUserInterfaceMode(int &radioMode, int &structureMode )
{
  if (this->SelectSubsetButton->GetSelectedState())
	  structureMode = 1;
	else
	  structureMode = 0;
	
	radioMode = 0;  
  for (int i=0; i<6; i++)
	{
		if (this->RadioButtonSet->GetWidget(i)->GetSelectedState())
		{
			radioMode = i+1;
		}
	}
	  
	return;  
	  
}
//---------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::CopyConstraintValues(vtkUnstructuredGrid *input, vtkUnstructuredGrid *output)
{
	vtkIntArray *constrain = vtkIntArray::SafeDownCast(
		input->GetPointData()->GetArray("Constrain"));
	if(constrain)
	{
		vtkPoints *inpoints = input->GetPoints();
		vtkPoints *outpoints = output->GetPoints();
		vtkPointLocator *locator = vtkPointLocator::New();

		int i;
		vtkPoints *points = vtkPoints::New();
		locator->InitPointInsertion(points, outpoints->GetBounds());
		for (i=0; i<inpoints->GetNumberOfPoints(); i++)
		{
			locator->InsertNextPoint(inpoints->GetPoint(i));
		}

		vtkIntArray *constrainout = vtkIntArray::New();
		constrainout->SetNumberOfValues(outpoints->GetNumberOfPoints());
		for (i=0; i<outpoints->GetNumberOfPoints(); i++)
		{
			vtkIdType pos;
			pos = locator->IsInsertedPoint(outpoints->GetPoint(i));
			if (pos != -1)
			{
				constrainout->SetValue(i, constrain->GetValue(pos));
			}
			else{
				constrainout->SetValue(i, 0);
			}
		}
		constrainout->SetName("Constrain");
		output->GetPointData()->AddArray(constrainout);
		constrainout->Delete();
		locator->Delete();
		points->Delete();
	}
}
//-----------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::ViewMergeFeedbackCallback(int Mode)
{
	if(!Mode)
	{
		this->ApplyButton->SetStateToDisabled();
		this->SetDoUndoButtonSelectSubsetButton();
		this->MultiColumnList->SetEnabled(1);
		if(this->ViewMergePointsWidget)
		{
			if(this->ViewMergePointsWidget->GetEnabled())
			{
				this->ViewMergePointsWidget->SetEnabled(0);
			}
		}
		this->ViewProperties->UpdateVisibilityList();
//		this->SphereSizeFrame->SetParent(this->GetParent());
		//int i;
		//for (i=0; i < this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
		//{
		//	if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,1)->GetSelectedState())
		//	{
		//		vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
		//			this->BBoxList->GetItem(i));
		//		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(ugridActor->GetActor());
		//	}
		//}
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		if (!this->SelectCellsWidget)	return;
		if (!this->SelectCellsWidget->GetCurrentSelectedSubsetActor()) return;
		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
			this->SelectCellsWidget->GetCurrentSelectedSubsetActor());
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		return;
	}


	int location = 4;
	for (int i=0; i<7; i++)
	{
		if(location != i)
		{
			if(this->RadioButtonSet->GetWidget(i)->GetSelectedState())
			{
				this->RadioButtonSet->GetWidget(i)->SelectedStateOff();
			}
		}
	}
	this->SelectSubsetButton->SetEnabled(0);
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		if(!this->CancelStatus)
		{
			callback->ErrorMessage("Building Block selection required");
			this->DeselectAllButtons();
			return;
		}
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	vtkUnstructuredGrid *ugrid = vtkMimxUnstructuredGridActor::SafeDownCast(
		this->BBoxList->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	if(!this->ViewMergePointsWidget)
	{
		this->ViewMergePointsWidget = vtkMimxViewMergePointsWidget::New();
	}
	this->ViewMergePointsWidget->SetInteractor(
		this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	this->DataSetCollection->RemoveAllItems();
	int i;
	for (i=0; i < this->MultiColumnList->GetWidget()->GetNumberOfRows(); i++)
	{
		if(this->MultiColumnList->GetWidget()->GetCellWindowAsCheckButton(i,1)->GetSelectedState())
		{
			vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(
				this->BBoxList->GetItem(i));
			this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(ugridActor->GetActor());
			vtkUnstructuredGrid *mergeGrid = ugridActor->GetDataSet();
			this->DataSetCollection->AddItem(mergeGrid);
		}
	}

	if(!this->DataSetCollection->GetNumberOfItems())
	{
		callback->ErrorMessage("Select at least one building block structure from the list");
		return;
	}

	this->defaultMergeTolerance = this->ToleranceEntry->GetWidget()->GetValueAsDouble();

	this->ViewMergePointsWidget->SetInput(this->DataSetCollection);
	this->ViewMergePointsWidget->SetTolerance( this->defaultMergeTolerance );
/***VAM***/
	if(this->defaultSphereSizeEntry == -1.0)
	{
		this->ViewMergePointsWidget->
			ComputeHandleSize(this->ViewMergePointsWidget);
		this->Parameter = 1.0/this->ViewMergePointsWidget->GetHandleSize();
		this->SphereSizeEntry->GetWidget()->SetValueAsDouble(
			this->ViewMergePointsWidget->GetHandleSize()*this->Parameter);
	}
	else
	{
		this->SphereSizeEntry->GetWidget()->SetValueAsDouble(this->defaultSphereSizeEntry);
		this->ViewMergePointsWidget->SetHandleSize(this->SphereSizeEntry->GetWidget()->
			GetValueAsDouble()*(1.0/this->Parameter));
	}

	if(!this->ViewMergePointsWidget->GetEnabled())
	{
		this->ViewMergePointsWidget->SetEnabled(1);
	}
	this->MultiColumnList->SetEnabled(0);
	if(!this->SelectCellsWidget)
	{
		this->SelectCellsWidget = vtkMimxSelectCellsWidget::New();
		this->SelectCellsWidget->SetInteractor(
			this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	}
	this->SelectCellsWidget->SetInput(ugrid);
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		this->SelectCellsWidget->GetCurrentSelectedSubsetActor());

	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	this->ApplyButton->SetStateToNormal();	
}
//-----------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::MergeToleranceChangeCallback(const char *Tol)
{
	if(!strcmp(Tol, ""))	return;
	if(this->CancelStatus)
	{
		return;
	}
	if(!this->ViewMergePointsWidget)
	{
		return;
	}
	if(!this->ViewMergePointsWidget->GetEnabled())
	{
		return;
	}
	
	this->defaultMergeTolerance = this->ToleranceEntry->GetWidget()->GetValueAsDouble();
	//std::cout << "Update merge Tolerance : " << this->defaultMergeTolerance << std::endl;
	this->ViewMergePointsWidget->SetTolerance( this->defaultMergeTolerance );
	this->ViewMergePointsWidget->ComputeMergePointsAndFaces(this->ViewMergePointsWidget);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//------------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::RepackSphereSizeFrame()
{
	this->GetApplication()->Script(
		"pack %s -side top -anchor center -expand n -padx 2 -pady 2 -fill x -after %s", 
		this->SphereSizeFrame->GetWidgetName(), this->RadioButtonSet->GetWidgetName());

	this->GetApplication()->Script(
		"pack %s -side top -anchor center -expand 0 -padx 2 -pady 6", 
		this->SphereSizeEntry->GetWidgetName());
}
//-----------------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::SphereSizeChangeCallback(const char *dummy)
{
	if(this->CancelStatus)
	{
		return;
	}
	
	this->defaultSphereSizeEntry = this->SphereSizeEntry->GetWidget()->GetValueAsDouble();
	
	if(this->UnstructuredGridWidget)
	{
		if(this->UnstructuredGridWidget->GetEnabled())
		{
			this->UnstructuredGridWidget->SetEnabled(0);
			this->UnstructuredGridWidget->SetHandleSize(this->defaultSphereSizeEntry*(1.0/this->Parameter));
			this->UnstructuredGridWidget->SetEnabled(1);
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
		}
	}
	
	if(this->ViewMergePointsWidget)
	{
		if(this->ViewMergePointsWidget->GetEnabled())
		{
			this->ViewMergePointsWidget->SetEnabled(0);
			this->ViewMergePointsWidget->SetHandleSize(this->defaultSphereSizeEntry*(1.0/this->Parameter));
			this->ViewMergePointsWidget->SetEnabled(1);
			this->GetMimxMainWindow()->GetRenderWidget()->Render();
		}
	}
	
	return;
	
	/*
	if(this->RadioButtonSet->GetWidget(6)->GetSelectedState() ||
		this->RadioButtonSet->GetWidget(4)->GetSelectedState())
	{
		if(this->ViewMergePointsWidget)
		{
			double handlesize = this->SphereSizeEntry->
				GetWidget()->GetValueAsDouble()*(1.0/this->SphereSizeFactor);

			this->defaultSphereSizeEntry = this->SphereSizeEntry->GetWidget()->GetValueAsDouble();

			if(this->ViewMergePointsWidget->GetEnabled())
			{
				this->ViewMergePointsWidget->SetEnabled(0);
				this->ViewMergePointsWidget->SetHandleSize(handlesize);
				this->ViewMergePointsWidget->SetEnabled(1);
			}
			else
			{
				this->ViewMergePointsWidget->SetHandleSize(handlesize);
			}
		}
		return;
	}
	*/
	//if( this->RadioButtonSet->GetWidget(4)->GetSelectedState())
	//{
	//	if(this->ViewMergePointsWidget)
	//	{
	//		double handlesize = this->ConsolidateSphereSizeEntry->
	//			GetWidget()->GetValueAsDouble()*(1.0/this->SphereSizeFactor);

	//		this->defaultSphereSizeEntry = this->ConsolidateSphereSizeEntry->GetWidget()->GetValueAsDouble();

	//		if(this->ViewMergePointsWidget->GetEnabled())
	//		{
	//			this->ViewMergePointsWidget->SetEnabled(0);
	//			this->ViewMergePointsWidget->SetHandleSize(handlesize);
	//			this->ViewMergePointsWidget->SetEnabled(1);
	//		}
	//		else
	//		{
	//			this->ViewMergePointsWidget->SetHandleSize(handlesize);
	//		}
	//	}
	//	return;
	//}
}
//--------------------------------------------------------------------------------
void vtkKWMimxEditBBGroup::CopyActorProperties( vtkMimxUnstructuredGridActor *newActor,  vtkMimxUnstructuredGridActor *oldActor)
{
	double red, green, blue;
	oldActor->GetFillColor(red, green, blue);
	newActor->SetFillColor(red, green, blue);	
	oldActor->GetOutlineColor(red, green, blue);
	newActor->SetOutlineColor(red, green, blue);

	double opacity = oldActor->GetActor()->GetProperty()->GetOpacity( );
	newActor->GetActor()->GetProperty()->SetOpacity( opacity );
  
  double lineWidth = oldActor->GetActor()->GetProperty()->GetLineWidth( );
  newActor->GetActor()->GetProperty()->SetLineWidth( lineWidth );
  
  int mode = oldActor->GetDisplayType();
	newActor->SetDisplayType( mode );
}
//--------------------------------------------------------------------------------
