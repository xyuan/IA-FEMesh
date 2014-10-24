#include "vtkKWMyWindow.h"

// VTK includes

#include "vtkCornerAnnotation.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkPlaneWidget.h"
#include "vtkGeometryFilter.h"
#include "vtkProperty.h"
#include "vtkCommand.h"
#include "vtkPlane.h"
#include "vtkRendererCollection.h"
#include "vtkCamera.h"
#include "vtkToolkits.h"
#include <vtksys/SystemTools.hxx>
#include "vtkAnnotatedCubeActor.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPropAssembly.h"
#include "vtkAxesActor.h"
#include "vtkPolyData.h"
#include "vtkMapper.h"
#include "vtkTransform.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkUnstructuredGrid.h"

 // KWWidgets includes 

#include "vtkKWApplication.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithSpinButtons.h"
#include "vtkKWMenuButtonWithSpinButtonsWithLabel.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWCheckButtonWithChangeColorButton.h"
#include "vtkKWNotebook.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWScale.h"
#include "vtkKWWindow.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWUserInterfacePanel.h"
#include "vtkKWPushButtonSet.h"
#include "vtkKWPushButton.h"
#include "vtkKWRange.h"
#include "vtkKWThumbWheel.h"
#include "vtkKWListBoxWithScrollbars.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWText.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWComboBox.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWMenu.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMyWindow );
vtkCxxRevisionMacro(vtkKWMyWindow, "$Revision: 1.11 $");

//----------------------------------------------------------------------------


// Create a callback function so the selection plane can invoke this when it is
// moved by the operator.  The corresponding implicit plane function is passed 
// back into a pre-allocated vtkPlane instance for use elsewhere in the application

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
      if (((vtkKWMyWindow*)myInterface)->meshQualityInstance->GetElementsHighlighted())
  	  {
  	    ((vtkKWMyWindow*)myInterface)->meshQualityInstance->CancelElementHighlight();
  	    ((vtkKWMyWindow*)myInterface)->HighlightCellsButton->GetWidget()->SelectedStateOff();
  	  }
  	  // if real-time updates are selected, then redraw the interface after
  	  // each movement of the plane. Could be slow for large meshes, so it is
  	  // gated by the RealTimeWarningUpdate status variable, set through the GUI  
      if (((vtkKWMyWindow*)myInterface)->RealTimeWarningUpdate)
      {
         ((vtkKWMyWindow*)myInterface)->UpdateRenderingState();
      }
    }
  
  void SetUserInterfacePointer (vtkKWWindow *userInterface)
  { myInterface = userInterface;}
  
  vtkPlaneWidgetEventCallback():PlaneInstance(0) {}
  vtkPlane *PlaneInstance;
  vtkKWWindow *myInterface;

};

// Create callbacks so the buttons can be selected to enable or disable the 
// display of various actors that come back from the quality class
/****
class vtkOutlineDisplayEventCallback : public vtkCommand
{
public:
  static vtkOutlineDisplayEventCallback *New() 
    { return new vtkOutlineDisplayEventCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkKWCheckButtonWithLabel *buttonWidget = reinterpret_cast<vtkKWCheckButtonWithLabel*>(caller);
      buttonWidget->GetState();
      cout << "button callback called" << endl;
    }
  vtkPlaneWidgetEventCallback() {}

};
*****/

vtkKWMyWindow::vtkKWMyWindow()
{
  this->RenderWidget = NULL;
  this->SliceScale = NULL;
  this->ThresholdScale = NULL;
  this->OutlineSelector = NULL;
  this->ClippedOutlineSelector = NULL;
  this->InteriorOutlineSelector = NULL;
  this->HedgehogSelector = NULL;
  this->HedgehogScale = NULL;
  this->FilledElementSelector = NULL;
  this->InvertSelector = NULL;
  this->ElementOpacity = NULL;
  this->QualityButtons = NULL;
  this->CuttingPlaneOpacityScale = NULL;
  this->CuttingPlaneEnabled = 0;
  this->SavedGridReader = NULL;
  this->meshQualityInstance = NULL;
  this->ElementColorRange = NULL;
  this->OverrideColorButton = NULL;
  this->HighlightCellsButton = NULL;
  this->DecimalDisplayPrecisionWidget = NULL;
  this->OrientationCueSelector = NULL;
  this->SavedOrientationWidget = NULL;
  this->RealTimeWarningButton = NULL;
  this->selectionControls = NULL;
  this->meshControls = NULL;
  this->BottomPanelText = NULL;
  this->SavedLoadSaveButton = NULL;
  this->QualityButtons = NULL;
  this->myshrink = NULL;
  this->SavedPlaneWidget = NULL;
  this->SavedImplicitPlane = NULL;
  this->SavedElementActor = NULL;
  this->SavedOutlineActor = NULL;
  this->SavedClippedOutlineActor = NULL;
  this->SavedInteriorOutlinesActor = NULL;
  this->SavedSurfaceHedgehogActor = NULL;      
  this->SavedColorLegendActor = NULL;
  this->RadioButtonValue = 0;
  this->SavedThresholdValue = 0.0;
  
  // set the default number of digits of precision to display; changed through GUI action
  this->DecimalDisplayPrecision = 3;
  
  // set the state so that movement of the clipping plane automatically 
  // forces a recalculation of the mesh.  This may need to be disabled for larger
  // meshes, where recalculaion is slower.
  this->RealTimeWarningUpdate = 1;
  
  this->meshQualityInstance = (vtkMeshQualityClass*) vtkMeshQualityClass::New(); 
  
  // try doing these things before a dataset is read in just to be sure we 
  // catch logic errors 
  this->meshQualityInstance->SetElementOpacity(1.0);
  this->meshQualityInstance->SetElementShrinkFactor(1.0);
  //this->meshQualityInstance->UpdatePipeline();
  //this->meshQualityInstance->SetQualityMeasure(0);
   this->meshQualityInstance->SetShowSurfaceHedgehog(0);

  //this->meshQualityInstance->InitializeFromFile("/Users/clisle/Projects/fe/code/IaFeMEsh/Testing/Data/Meshes/finger_hex.vtk");
  //this->meshQualityInstance->InitializeFromFile("/Users/clisle/Projects/fe/code/IaFeMEsh/Testing/Data/Meshes/Vert_Bod.vtk");

  // instantiate a single reader and keep it for the life of the VTK pipeline so
  // we can keep the pipeline in an updated state
  
  //cout << "got here preread" << endl;
  
  this->SavedGridReader = vtkUnstructuredGridReader::New();
  //this->SavedGridReader = reader;
  
  //cout << "about to set filename" << endl;
  //reader->SetFileName("/Users/clisle/Projects/fe/code/IaFeMesh/Testing/Data/Meshes/finger_hex.vtk");
  //reader->SetFileName("/Users/clisle/Projects/fe/code/IaFeMEsh/Testing/Data/Meshes/finger_hex.vtk");
  //cout << "about to read file" << endl;
  //reader->Update();
  //cout << "read file completed" << endl;
  vtkUnstructuredGrid *nullMesh=vtkUnstructuredGrid::New();
  
  this->meshQualityInstance->InitializeFromExternalMesh( nullMesh );
  
  // setup the properties of the plane widget used to select some of the elements
  // The plane is setup as a dark, semitransparent brown.  We could sample the dataset and 
  // put a contour on the plane, as well, but lets get it working first :-) 
  // For future extension, please refer to the ProbingWithPlaneWidget.tcl VTK example
  
  this->SavedImplicitPlane = vtkPlane::New();
  this->SavedPlaneWidget = (vtkPlaneWidget*) vtkPlaneWidget::New();
  this->SavedPlaneWidget->SetInput((vtkDataSet*)(nullMesh));
  this->SavedPlaneWidget->SetRepresentationToSurface();
  this->SavedPlaneWidget->GetPlaneProperty()->SetColor(0.2,0.2,0);
  this->SavedPlaneWidget->GetPlaneProperty()->SetOpacity(0.2);
 
  //cout << "meshQual myWindow constructor complete" << endl; 
  
} 

//----------------------------------------------------------------------------
vtkKWMyWindow::~vtkKWMyWindow()
{
  if (this->SliceScale)
    {
    this->SliceScale->Delete();
    }

  if (this->RenderWidget)
    {
    this->RenderWidget->Delete();
    }

}


void vtkKWMyWindow::InitializeInterfaceFromMeshFile(char* filename)
{
   if (this->meshQualityInstance)
       this->meshQualityInstance->InitializeFromFile(filename);
}



//----------------------------------------------------------------------------
void vtkKWMyWindow::CreateWidget()
{
  // Check if already created
  
  //cout << "myKWWindow-CreateWidget called" << endl;

  if (this->IsCreated())
    {
    vtkErrorMacro("class already created");
    return;
    }

  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  //cout << "got back from superclass create" << endl;

  vtkKWApplication *app = this->GetApplication();

  // Add a render widget, attach it to the view frame, and pack

  if (!this->RenderWidget)
    {
    this->RenderWidget = vtkKWRenderWidget::New();
    }
  this->RenderWidget->SetParent(this->GetViewFrame());
  this->RenderWidget->Create();
  this->RenderWidget->CornerAnnotationVisibilityOn();

  app->Script("pack %s -expand y -fill both -anchor c -expand y", 
              this->RenderWidget->GetWidgetName());
			  
			  //cout << "got here 0" << endl;

  // Create the rest of the VTK pipeline attached to the quality filter.  We create
  // the renderer and pass it to the quality filter because several actors will be created
  // by the quality filter.  This is different from the original version, which created only
  // a single actor.  Multiple actors were best so that multiple types of rendering can be 
  // controlled from the mesh quality class. 
  
  vtkRenderer *myRenderer = this->RenderWidget->GetRenderer();
  myRenderer->TwoSidedLightingOn();
  myRenderer->SetBackground(1,1,1);

  // process this mesh and prepare for rendering
  this->meshQualityInstance->SetShowOutline(0);
  this->meshQualityInstance->SetShowClippedOutline(1);
  this->meshQualityInstance->SetShowInteriorOutlines(0);
  this->meshQualityInstance->SetShowFilledElements(1);
  this->meshQualityInstance->SetShowSurfaceHedgehog(0);
  //cout << "got here 0.5"  << endl;
  this->meshQualityInstance->CalculateMeshQuality();
  
  this->SavedElementActor = this->meshQualityInstance->ReturnFilledElementsActor();
  this->SavedInteriorOutlinesActor = this->meshQualityInstance->ReturnInternalOutlinesActor();
  this->SavedOutlineActor = this->meshQualityInstance->ReturnOutlineActor();
  this->SavedSurfaceHedgehogActor = this->meshQualityInstance->ReturnHedgehogActor();
  this->SavedColorLegendActor = this->meshQualityInstance->ReturnColorLegendActor();
  this->SavedClippedOutlineActor = this->meshQualityInstance->ReturnClippedOutlineActor();
  
  
  // add the quality actors to the render widget
  myRenderer->AddActor(this->SavedElementActor); 
  myRenderer->AddActor(this->SavedInteriorOutlinesActor); 
  myRenderer->AddActor(this->SavedOutlineActor); 
  myRenderer->AddActor(this->SavedClippedOutlineActor);
  myRenderer->AddActor(this->SavedSurfaceHedgehogActor);
  myRenderer->AddActor(this->SavedColorLegendActor);
  

    
  //this->RenderWidget->ResetCamera();
  //this->RenderWidget->Render();
  
  //cout << "got here 1" << endl;
  //----------------------------------------------------------
  // continue initializing the plane widget, now that we have 
  // a vtkRenderWindowInteractor to use to set up event handling
  //----------------------------------------------------------

  this->SavedPlaneWidget->SetInteractor(this->RenderWidget->GetRenderWindowInteractor());
  
  // register a callback that is invoked when the plane is moved or dragged, so the mesh quality
  // class can get the new positions and update the rendered actors appropriately.  A vtkPlane
  // instance is updated with the information from the plane widget.  Then the vtkPlane instance
  // can be passed to the quality filter so the implicit function can enable/disable element display
  // accordingly. 
  
  vtkPlaneWidgetEventCallback* PlaneMoveCallback = vtkPlaneWidgetEventCallback::New();
  PlaneMoveCallback->SetUserInterfacePointer(this);
  PlaneMoveCallback->PlaneInstance = this->SavedImplicitPlane;
  this->SavedPlaneWidget->AddObserver(vtkCommand::InteractionEvent,PlaneMoveCallback);
  this->meshQualityInstance->SetCuttingPlaneFunction(this->SavedImplicitPlane);

  
  //----------------------------------------------------------
  // Create text field for file information to be dispkayed  
  //----------------------------------------------------------
 
  this->BottomPanelText = vtkKWText::New();
  this->BottomPanelText->SetParent(this->GetSecondaryPanelFrame());
  this->BottomPanelText->Create();
  this->BottomPanelText->SetText("Current Mesh: ");
  this->BottomPanelText->SetWidth(500);
  //this->BottomPanelText->SetHeight(12);
  this->BottomPanelText->SetBalloonHelpString(
    "This display area presents information about the mesh currently being viewed");
  this->SetStatusText( "Current Mesh: " );
  app->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
    this->BottomPanelText->GetWidgetName());
  
  
  //----------------------------------------------------------
  // Create empty frames to put the controls in.  Frames are named for their control  
  //----------------------------------------------------------

  vtkKWMenu* fileMenu = this->GetFileMenu();
  fileMenu->InsertCommand (0, "Load Mesh", this, "LoadMeshCallback");
  fileMenu->InsertCommand (1, "Save Selected Mesh", this, "SaveSelectionMeshCallback");

  fileMenu->InsertSeparator(2);
  fileMenu->InsertCommand (3, "Save Snapshot", this, "TakeWindowSnapshotWithFilename");
  fileMenu->InsertSeparator(4);
  fileMenu->DeleteItem (5);
  //load_button->SetCommand(this, "LoadMeshCallback");
  
  /*
  vtkKWFrameWithLabel *file_frame = vtkKWFrameWithLabel::New();
  file_frame->SetParent(this->GetMainPanelFrame());
  file_frame->Create();
  file_frame->SetLabelText("Files and Snapshots");
  app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2 ",
              file_frame->GetWidgetName());
*/
 // Also add push butttons for control of the mesh
/*    meshControls = vtkKWPushButtonSet::New();
    meshControls->SetParent(file_frame->GetFrame());
    meshControls->Create();
    meshControls->SetPadX(2);
    meshControls->SetPadY(2);
    meshControls->SetBorderWidth(2);
    meshControls->SetReliefToGroove();
    meshControls->ExpandWidgetsOn();
    meshControls->SetWidgetsPadY(1);
    
    app->Script("pack %s -expand n -fill x -padx 2 -pady 2", 
      meshControls->GetWidgetName());
    

    vtkKWPushButton *load_button = meshControls->AddWidget(1);; 
    load_button->SetText("Load Mesh File");
    load_button->SetCommand(this, "LoadMeshCallback");
*/  
  // Add a user interface panel to the secondary user interface manager
  // Note how we do not interfere with the notebook, we just create
  // a panel, hand it over to the manager, and ask the panel to return
  // us where we should pack our widgets (i.e., the actually implementation
  // is left to the window). 

  vtkKWUserInterfacePanel *label_panel = vtkKWUserInterfacePanel::New();
  label_panel->SetName("Display Interface");
  label_panel->SetUserInterfaceManager(this->GetMainUserInterfaceManager());
  label_panel->Create();

  // be careful.  When we reference these tabs later, the string must match exactly
  // to avoid a runtime error.
  
  label_panel->AddPage("Mesh Quality", "Select Quality Metric to Display", NULL);
  label_panel->AddPage("Display", "Enable/Disable Mesh Views", NULL);
  label_panel->AddPage("Elements", "Select Individual Elements from the mesh", NULL);
  
  
    //printf("got here 1 \n"); fflush(stdout);

   //----------------------------------------------------------
  vtkKWWidget *page_widget = label_panel->GetPageWidget("Display");
   
  vtkKWFrameWithLabel *outline_frame = vtkKWFrameWithLabel::New();
  outline_frame->SetParent(label_panel->GetPagesParentWidget());
  outline_frame->Create();
  outline_frame->SetLabelText("Options");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2  -in %s",
              outline_frame->GetWidgetName(),page_widget->GetWidgetName());    
   /*
   cout << "before clipped outline frame" << endl;
   
   vtkKWFrameWithLabel *clipped_outline_frame = vtkKWFrameWithLabel::New();
   clipped_outline_frame->SetParent(label_panel->GetPagesParentWidget());
   clipped_outline_frame->Create();
   clipped_outline_frame->SetLabelText("Clippable Outline Display");
      app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2  -in %s",
                 clipped_outline_frame->GetWidgetName(),page_widget->GetWidgetName());    
   
   cout << "after clipped outline frame" << endl;
      
  vtkKWFrameWithLabel *interior_outline_frame = vtkKWFrameWithLabel::New();
  interior_outline_frame->SetParent(label_panel->GetPagesParentWidget());
  interior_outline_frame->Create();
  interior_outline_frame->SetLabelText("Interior Outlines Display");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2  -in %s",
              interior_outline_frame->GetWidgetName(),page_widget->GetWidgetName());     
              
  //printf("got here 1.5\n"); fflush(stdout);
  
  vtkKWFrameWithLabel *filled_elements_frame = vtkKWFrameWithLabel::New();
  filled_elements_frame->SetParent(label_panel->GetPagesParentWidget());
  filled_elements_frame->Create();
    filled_elements_frame->SetLabelText("Display Filled Elements");
  app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2 -in %s",
              filled_elements_frame->GetWidgetName(),page_widget->GetWidgetName());      
   
  vtkKWFrameWithLabel *hedgehog_frame = vtkKWFrameWithLabel::New();
  hedgehog_frame->SetParent(label_panel->GetPagesParentWidget());
  hedgehog_frame->Create();
  hedgehog_frame->SetLabelText("Surface Normals Display");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2 -in %s",
              hedgehog_frame->GetWidgetName(),page_widget->GetWidgetName());    
   
   vtkKWFrameWithLabel *orientation_axis_control = vtkKWFrameWithLabel::New();
   orientation_axis_control->SetParent(label_panel->GetPagesParentWidget());
   orientation_axis_control->Create();
   orientation_axis_control->SetLabelText("Orientation Axis Display");
      app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2 -in %s",
    		  orientation_axis_control->GetWidgetName(),page_widget->GetWidgetName());    
*/
  //--------------------------------------------------------------
  page_widget = label_panel->GetPageWidget("Elements");

  // Removing threshold control because threshold filter was becoming confused by 
  // scalar values.  May reinstate it later, but it is simpler to remove at the moment
  // as this feature (select elements by percent of range) was seldom used anyway. 
  
//  vtkKWFrameWithLabel *thresh_frame = vtkKWFrameWithLabel::New();
//  thresh_frame->SetParent(label_panel->GetPagesParentWidget());
//  thresh_frame->Create();
//  thresh_frame->SetLabelText("Element Display Threshold");
//   app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2 -in %s",
//              thresh_frame->GetWidgetName(),page_widget->GetWidgetName());
              
  vtkKWFrameWithLabel *elemSize_frame = vtkKWFrameWithLabel::New();
  elemSize_frame->SetParent(label_panel->GetPagesParentWidget());
  elemSize_frame->Create();
  elemSize_frame->SetLabelText("Element Properties");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2 -in %s",
              elemSize_frame->GetWidgetName(),page_widget->GetWidgetName());    
 
  // a frame for the opacity control
  vtkKWFrameWithLabel *opacity_frame = vtkKWFrameWithLabel::New();
  opacity_frame->SetParent(label_panel->GetPagesParentWidget());
  opacity_frame->Create();
  opacity_frame->SetLabelText("Element Opacity");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2 -in %s",
              opacity_frame->GetWidgetName(),page_widget->GetWidgetName());   
  
   // printf("got here 2\n"); fflush(stdout);
    
    // a frame for the selection plane control buttons
 
  vtkKWFrameWithLabel *PlaneControlFrame = vtkKWFrameWithLabel::New();
  PlaneControlFrame->SetParent(label_panel->GetPagesParentWidget());
  PlaneControlFrame->Create();
  PlaneControlFrame->SetLabelText("Selection Plane Controls");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2 -in %s",
              PlaneControlFrame->GetWidgetName(),page_widget->GetWidgetName());  
                   
              
  // a frame inside the plane  section.  This wraps a button
  // to invert the selection of the plane
/* 
    vtkKWFrameWithLabel *InvertPlaneSubframe = vtkKWFrameWithLabel::New();
    InvertPlaneSubframe->SetParent(label_panel->GetPagesParentWidget());
    InvertPlaneSubframe->Create();
    InvertPlaneSubframe->SetLabelText("Invert Selection");
    app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2 -in %s",
              InvertPlaneSubframe->GetWidgetName(),page_widget->GetWidgetName());  
 */             
              
   //----------------------------------------------------------
   page_widget = label_panel->GetPageWidget("Mesh Quality");
  
  // a frame for the quality metric selector buttons
 
  vtkKWFrameWithLabel *QualityFrame = vtkKWFrameWithLabel::New();
  QualityFrame->SetParent(label_panel->GetPagesParentWidget());
  QualityFrame->Create();
  QualityFrame->SetLabelText("Quality Metric");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2 -in %s",
              QualityFrame->GetWidgetName(),page_widget->GetWidgetName());   
         
         
  vtkKWFrameWithLabel *ReportFrame = vtkKWFrameWithLabel::New();
  ReportFrame->SetParent(label_panel->GetPagesParentWidget());
  ReportFrame->Create();
  ReportFrame->SetLabelText("Distorted Elements");
   app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2 -in %s",
              ReportFrame->GetWidgetName(),page_widget->GetWidgetName());   
   
   vtkKWListBoxWithScrollbars *canvas = vtkKWListBoxWithScrollbars::New();
   //canvas->SetParent(ReportFrame);
   canvas->SetParent(ReportFrame->GetFrame());
   canvas->Create();
   canvas->SetBorderWidth(2);
   canvas->SetReliefToGroove();
   canvas->SetPadX(2);
   canvas->SetPadY(2);
   canvas->SetWidth(40);
   canvas->SetHeight(10);
   //canvas->CreateVerticalScrollbar();
   //app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2 -in %s",
   //           canvas->GetWidgetName(),ReportFrame->GetWidgetName()); 
   app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
              canvas->GetWidgetName()); 
   //cout << "defining highlight cells button" << endl;
   
   if (!this->HighlightCellsButton)
      {
        this->HighlightCellsButton = vtkKWCheckButtonWithLabel::New();
      }  
      this->HighlightCellsButton->SetParent(ReportFrame->GetFrame());
      this->HighlightCellsButton->Create();
      this->HighlightCellsButton->GetWidget()->SetCommand(this, "SetHighlightCellsFromButton");
      this->HighlightCellsButton->GetWidget()->SetText("Show only Distorted Elements");
      app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
                  this->HighlightCellsButton->GetWidgetName());
                  
      //cout << "done defining highlight cells button" << endl; 
 
    if (!this->RealTimeWarningButton)
      {
        this->RealTimeWarningButton = vtkKWCheckButtonWithLabel::New();
      }  
      this->RealTimeWarningButton->SetParent(ReportFrame->GetFrame());
      this->RealTimeWarningButton->Create();
      this->RealTimeWarningButton->GetWidget()->SetCommand(this, "SetRealTimeWarningUpdateFromButton");
      this->RealTimeWarningButton->GetWidget()->SetText("Dynamically Update");
      app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
                  this->RealTimeWarningButton->GetWidgetName());
                  
      //cout << "done defining highlight cells button" << endl; 
  
      // Also add push butttons for control of the mesh
/*      vtkKWPushButtonSet *snapshotControls = vtkKWPushButtonSet::New();
          snapshotControls->SetParent(file_frame->GetFrame());
          snapshotControls->Create();
          snapshotControls->SetPadX(2);
          snapshotControls->SetPadY(2);
          snapshotControls->SetBorderWidth(2);
          snapshotControls->SetReliefToGroove();
          snapshotControls->ExpandWidgetsOn();
          snapshotControls->SetWidgetsPadY(1);
          
          app->Script("pack %s -expand n -fill x -padx 2 -pady 2", 
        		  snapshotControls->GetWidgetName());
          
          vtkKWPushButton *snap_button = snapshotControls->AddWidget(1); 
          snap_button->SetText("Auto Screen Snapshot into Current Directory");
          snap_button->SetCommand(this, "TakeWindowSnapshot");
      
          vtkKWLoadSaveButton *snap_dialog = vtkKWLoadSaveButton::New() ;
          snap_dialog->SetParent(file_frame->GetFrame()) ;
          snap_dialog->Create();
          snap_dialog->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("FEMeshSnapshotPath");
          snap_dialog->SetText ("Snapshot to Specific Filename");
          snap_dialog->GetLoadSaveDialog()->SetTitle ("Snapshot to Specific Filename");
          snap_dialog->GetLoadSaveDialog()->SetFileTypes ("{{JPEG files} {.jpg}}");
          snap_dialog->GetLoadSaveDialog()->SaveDialogOn();
          snap_dialog->GetLoadSaveDialog()->SetDefaultExtension (".jpg");
          snap_dialog->SetCommand(this, "TakeWindowSnapshotWithFilename");
    
          app->Script("pack %s -expand n -fill x -padx 2 -pady 2", 
        		  snap_dialog->GetWidgetName());
*/          
          // save a pointer so the callback can get access and request the 
          // filename from this instance
          //this->SavedLoadSaveButton = snap_dialog;
      
   // keep reference to this object to allow contents to be added
   // and deleted 
   this->meshQualityInstance->AddErrorReportingCanvas(canvas);

   
         
    //printf("got here 3\n"); fflush(stdout);
  
    // Create a scale to control the threshold value when elements
    // are set to be visible

    // create a UI element that allows the user to select the number
    // of decimal places used to display quality metrics on the color bar as
    // well as anywhere else quality values are displayed.
    
    
    if (!this->DecimalDisplayPrecisionWidget)
        {
        this->DecimalDisplayPrecisionWidget = vtkKWComboBoxWithLabel::New();
        }
    this->DecimalDisplayPrecisionWidget->SetParent(ReportFrame);
    this->DecimalDisplayPrecisionWidget->Create();
    this->DecimalDisplayPrecisionWidget->SetLabelText("Floating Point Precision: ");
    this->DecimalDisplayPrecisionWidget->GetWidget()->SetCommand(this, "SetDecimalPrecisionCallback");
    char string_i[128];
    for (int i = 0; i < 7 ; i++)
      {
    	  sprintf(string_i,"%d",i);
    	  this->DecimalDisplayPrecisionWidget->GetWidget()->AddValue(string_i);
      }
    this->DecimalDisplayPrecisionWidget->GetWidget()->SetValue("3");
      app->Script(
        "pack %s -side top -anchor nw -expand n -padx 2 -pady 6", 
        this->DecimalDisplayPrecisionWidget->GetWidgetName());
    
    
    
//  if (!this->ThresholdScale)
//    {
//    this->ThresholdScale = vtkKWScale::New();
//    }
//  this->ThresholdScale->SetParent(thresh_frame->GetFrame());
//  this->ThresholdScale->Create();
//  this->ThresholdScale->SetLabelText("Percent of Value Range to Display");
//  this->ThresholdScale->SetCommand(this, "SetThresholdFromScaleCallback");
//  
//    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
//              this->ThresholdScale->GetWidgetName());
//              
  //printf("got here 4\n"); fflush(stdout);


  // Create a scale to control the element size

  if (!this->SliceScale)
    {
    this->SliceScale = vtkKWScale::New();
    }
  this->SliceScale->SetParent(elemSize_frame->GetFrame());
  this->SliceScale->Create();
  this->SliceScale->SetLabelText("Element Size (%)");
  this->SliceScale->SetCommand(this, "SetElementSizeFromScaleCallback");
  
    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
              this->SliceScale->GetWidgetName());
    
    if (!this->OverrideColorButton)
    {
      this->OverrideColorButton = vtkKWCheckButtonWithLabel::New();
    }  
    this->OverrideColorButton->SetParent(elemSize_frame->GetFrame());
    this->OverrideColorButton->Create();
    this->OverrideColorButton->GetWidget()->SetCommand(this, "SetOverrideColorRangeFromButton");
    this->OverrideColorButton->GetWidget()->SetText("Manually Set Color Range");
    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
                this->OverrideColorButton->GetWidgetName());
                
   
    
    if (!this->ElementColorRange)
      {   
    	this->ElementColorRange  = vtkKWRange::New();
    	this->ElementColorRange->SetParent(elemSize_frame->GetFrame());
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
        app->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
             this->ElementColorRange->GetWidgetName());
      }
    
   //printf("got here 5\n"); fflush(stdout);

  // Create a scale to control the element opacity

  if (!this->ElementOpacity)
    {
    this->ElementOpacity = vtkKWScale::New();
    }
  this->ElementOpacity->SetParent(opacity_frame->GetFrame());
  this->ElementOpacity->Create();
  this->ElementOpacity->SetLabelText("Percent Visible");
  this->ElementOpacity->SetCommand(this, "SetElementAlphaFromScaleCallback");
  
    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
              this->ElementOpacity->GetWidgetName());
              
   //printf("got here 6\n"); fflush(stdout);

  if (!this->OutlineSelector)
  {
    this->OutlineSelector = vtkKWCheckButton::New();
  }  
  this->OutlineSelector->SetParent(outline_frame->GetFrame());
  this->OutlineSelector->SetText("Original Outline");
  this->OutlineSelector->Create();
  this->OutlineSelector->SetCommand(this, "SetOutlineEnableFromButton");
  // app->Script("pack %s -side left -expand n -fill x -padx 2 -pady 2",
  //            this->OutlineSelector->GetWidgetName());
  app->Script("grid %s -row 0 -column 0 -sticky nw -padx 2 -pady 2",
               this->OutlineSelector->GetWidgetName()); 
                           
  //cout << "declaring clipped outline button" << endl;
  
  if (!this->ClippedOutlineSelector)
   {
     this->ClippedOutlineSelector = vtkKWCheckButton::New();
   }  
   this->ClippedOutlineSelector->SetParent(outline_frame->GetFrame());
   this->ClippedOutlineSelector->SetText("Clippable Outline");
   this->ClippedOutlineSelector->Create();
   this->ClippedOutlineSelector->SetCommand(this, "SetClippedOutlineEnableFromButton");
   //app->Script("pack %s -side left -expand n -fill x -padx 2 -pady 2",
   //            this->ClippedOutlineSelector->GetWidgetName());
   app->Script("grid %s -row 0 -column 1 -sticky nw -padx 2 -pady 2",
               this->ClippedOutlineSelector->GetWidgetName()); 
               
   //cout << "done clipped outline button" << endl;
                  
  if (!this->InteriorOutlineSelector)
  {
    this->InteriorOutlineSelector = vtkKWCheckButton::New();
  }  
  this->InteriorOutlineSelector->SetParent(outline_frame->GetFrame());
  this->InteriorOutlineSelector->SetText("Interior Outlines");
  this->InteriorOutlineSelector->Create();
  this->InteriorOutlineSelector->SetCommand(this, "SetInteriorOutlineEnableFromButton");
  // app->Script("pack %s -side left -expand n -fill x -padx 2 -pady 2",
  //            this->InteriorOutlineSelector->GetWidgetName());
  app->Script("grid %s -row 1 -column 0 -sticky nw -padx 2 -pady 2",
               this->InteriorOutlineSelector->GetWidgetName());
               
  if (!this->FilledElementSelector)
  {
    this->FilledElementSelector = vtkKWCheckButton::New();
  }  
  this->FilledElementSelector->SetParent(outline_frame->GetFrame());
  this->FilledElementSelector->SetText( "Filled Elements");
  this->FilledElementSelector->Create();
  this->FilledElementSelector->SetCommand(this, "SetFilledElementEnableFromButton");
  //app->Script("pack %s -side left -expand n -fill x -padx 2 -pady 2",
  //            this->FilledElementSelector->GetWidgetName());
  app->Script("grid %s -row 1 -column 1 -sticky nw -padx 2 -pady 2",
               this->FilledElementSelector->GetWidgetName());
                           
  if (!this->HedgehogSelector)
  {
    this->HedgehogSelector = vtkKWCheckButton::New();
  }  
  this->HedgehogSelector->SetParent(outline_frame->GetFrame());
  this->HedgehogSelector->SetText("Surface Normals");
  this->HedgehogSelector->Create();
  this->HedgehogSelector->SetCommand(this, "SetHedgehogEnableFromButton");
  //app->Script("pack %s -side left -expand n -fill x -padx 2 -pady 2",
  //            this->HedgehogSelector->GetWidgetName());
  app->Script("grid %s -row 2 -column 0 -sticky nw -padx 2 -pady 2",
               this->HedgehogSelector->GetWidgetName());
               
  if (!this->OrientationCueSelector)
   {
     this->OrientationCueSelector = vtkKWCheckButton::New();
   }  
   this->OrientationCueSelector->SetParent(outline_frame->GetFrame());
   this->OrientationCueSelector->SetText("Axis Orientation");
   this->OrientationCueSelector->Create();
   this->OrientationCueSelector->SetCommand(this, "SetOrientationCueEnableFromButton");
   //app->Script("pack %s -side left -expand n -fill x -padx 2 -pady 2",
   //            this->OrientationCueSelector->GetWidgetName());
   app->Script("grid %s -row 2 -column 1 -sticky nw -padx 2 -pady 2",
               this->OrientationCueSelector->GetWidgetName()); 


  
  // removed because it didn't update the scale of the vectors.  Follow-up later 
  
//  if (!this->HedgehogScale)
//   {   this->HedgehogScale = vtkKWThumbWheel::New();
//	   this->HedgehogScale->SetParent(hedgehog_frame->GetFrame());
//	   this->HedgehogScale->Create();
//	   this->HedgehogScale->SetCommand(this, "SetHedgehogScale");
//	   this->HedgehogScale->SetLength(150);
//	   this->HedgehogScale->DisplayEntryOn();
//	   this->HedgehogScale->DisplayLabelOn();
//	   //this->HedgehogScale->GetLabel()->SetText("Normal Vector Scale");
//	   this->HedgehogScale->SetBalloonHelpString(
//	     "Adjust the thumbwheel to lengthen or shorten the normal vectors centered on each polygon on the surface. ");
//	   app->Script(
//	     "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
//	     this->HedgehogScale->GetWidgetName());
//   }
          

  /***
   * create buttons that let the user select the quality test to apply
   * callback routines are attached to each button.  the routines are provided
   * below as other methods in this class.  Each callback invokes a method on the 
   * quality class to set its value 
   ***/
         
           //printf("got here 7\n"); fflush(stdout);

    
  if (!this->QualityButtons)
  {
    //cout << "instantiating the radio button set" << endl;
    this->QualityButtons = vtkKWRadioButtonSet::New();
  }
    this->QualityButtons->SetParent(QualityFrame->GetFrame());
    this->QualityButtons->Create();
    this->QualityButtons->SetBorderWidth(2);
    this->QualityButtons->SetReliefToGroove();
    vtkKWRadioButton *rb = this->QualityButtons->AddWidget(0); rb->SetText("Volume");
    vtkKWRadioButton *rb1 = this->QualityButtons->AddWidget(1); rb1->SetText("Edge Collapse");
    vtkKWRadioButton *rb2 = this->QualityButtons->AddWidget(2); rb2->SetText("Jacobian");
    vtkKWRadioButton *rb3 = this->QualityButtons->AddWidget(3); rb3->SetText("Skew");
    vtkKWRadioButton *rb4 = this->QualityButtons->AddWidget(4); rb4->SetText("Angle Out Of Bounds");
    this->QualityButtons->GetWidget(0)->SetSelectedState(1);
    rb->SetCommand(this, "SetQualityMetricFromButton");
    rb1->SetCommand(this, "SetQualityMetricFromButton1");
    rb2->SetCommand(this, "SetQualityMetricFromButton2");
    rb3->SetCommand(this, "SetQualityMetricFromButton3");
    rb4->SetCommand(this, "SetQualityMetricFromButton4");  
    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
              this->QualityButtons->GetWidgetName());


 
  // Also add push butttons for control of the selection plane widget
    selectionControls = vtkKWRadioButtonSet::New();
    selectionControls->SetParent(PlaneControlFrame->GetFrame());
    selectionControls->Create();
    selectionControls->SetPadX(2);
    selectionControls->SetPadY(2);
    selectionControls->SetBorderWidth(2);
    selectionControls->SetReliefToGroove();
    selectionControls->ExpandWidgetsOn();
    selectionControls->SetWidgetsPadY(1);
    selectionControls->SetPackHorizontally(1);
    app->Script("pack %s -expand n -fill x -padx 2 -pady 2", 
      selectionControls->GetWidgetName());
    
    vtkKWRadioButton *pb = selectionControls->AddWidget(1);
    pb->SetText("Enable");
    pb->SetCommand(this, "EnableSelectionPlaneCallback");
	
	  //pb = selectionControls->AddWidget(2);
    //pb->SetText("Save Current Selection as New Mesh");
    //pb->SetCommand(this, "SaveSelectionMeshCallback");
    
    pb = selectionControls->AddWidget(2);
    pb->SetText("Disable");
    pb->SetCommand(this, "ClearSelectionPlaneCallback");
    pb->SelectedStateOn();
              
  if (!this->InvertSelector)
  {
    this->InvertSelector = vtkKWCheckButton::New();
  }  
  this->InvertSelector->SetParent(PlaneControlFrame->GetFrame());
  this->InvertSelector->Create();
  this->InvertSelector->SetText("Invert Selection");
  this->InvertSelector->SetCommand(this, "SetInvertSelectionFromButton");
  app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2",
              this->InvertSelector->GetWidgetName());

  //cout << "done creating widgets" << endl; 
}

void vtkKWMyWindow::AddOrientationWidget(void)
{
	  vtkProperty* property;
	  
	  vtkAnnotatedCubeActor* cube = vtkAnnotatedCubeActor::New();
	  cube->SetFaceTextScale( 0.666667 );

	  vtkPropCollection* props = vtkPropCollection::New();
	  cube->GetActors( props );

	  vtkAppendPolyData* append = vtkAppendPolyData::New();

	  vtkTransformPolyDataFilter* transformFilter = vtkTransformPolyDataFilter::New();
	  vtkTransform* transform = vtkTransform::New();
	  transformFilter->SetTransform( transform );

	  vtkCollectionSimpleIterator sit;
	  props->InitTraversal( sit );
	  int nprops = props->GetNumberOfItems();

	  for ( int i = 0; i < nprops; i++ )
	    {
	    vtkActor *node = vtkActor::SafeDownCast( props->GetNextProp( sit ) );

	    // the first prop in the collection will be the cube outline, the last
	    // will be the text outlines
	    //
	    if ( node && i == 0 || i == (nprops - 1) )
	      {
	      vtkPolyData* poly = vtkPolyData::SafeDownCast(node->GetMapper()->GetInput());
	      if ( poly )
	        {
	        transformFilter->SetInput( poly );
	        transform->Identity();
	        transform->SetMatrix( node->GetMatrix() );
	        transform->Scale( 2.0, 2.0, 2.0 );
	        transformFilter->Update();

	        vtkPolyData* newpoly = vtkPolyData::New();
	        newpoly->DeepCopy( transformFilter->GetOutput() );
	        append->AddInput( newpoly );
	        newpoly->Delete();
	        }
	      }
	    }

	  // the orientation marker passed to the widget will be composed of two
	  // actors: vtkAxesActor and a vtkAnnotatedCubeActor
	  //
	  cube->SetFaceTextScale( 0.65 );

	  property = cube->GetCubeProperty();
	  property->SetColor( 0.5, 1, 1 );

	  property = cube->GetTextEdgesProperty();
	  property->SetLineWidth( 1 );
	  property->SetDiffuse( 0 );
	  property->SetAmbient( 1 );
	  property->SetColor( 0.1800, 0.2800, 0.2300 );

	  // this static function improves the appearance of the text edges
	  // since they are overlaid on a surface rendering of the cube's faces
	  //
	  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

	  // anatomic labelling
	  //
	  cube->SetXPlusFaceText ( "X" );
	  cube->SetXMinusFaceText( "-x" );
	  cube->SetYPlusFaceText ( "Y" );
	  cube->SetYMinusFaceText( "-y" );
	  cube->SetZPlusFaceText ( "Z" );
	  cube->SetZMinusFaceText( "-z" );

	  // change the vector text colors
	  //
	  property = cube->GetXPlusFaceProperty();
	  property->SetColor(1, 0, 0);
	  property->SetInterpolationToFlat();
	  property = cube->GetXMinusFaceProperty();
	  property->SetColor(1, 0, 0);
	  property->SetInterpolationToFlat();
	  property = cube->GetYPlusFaceProperty();
	  property->SetColor(0, 1, 0);
	  property->SetInterpolationToFlat();
	  property = cube->GetYMinusFaceProperty();
	  property->SetColor(0, 1, 0);
	  property->SetInterpolationToFlat();
	  property = cube->GetZPlusFaceProperty();
	  property->SetColor(0, 0, 1);
	  property->SetInterpolationToFlat();
	  property = cube->GetZMinusFaceProperty();
	  property->SetColor(0, 0, 1);
	  property->SetInterpolationToFlat();

	  vtkAxesActor* axes2 = vtkAxesActor::New();

	  // simulate a left-handed coordinate system
	  //
	  transform->Identity();
	  //transform->RotateY(90);
	  axes2->SetShaftTypeToCylinder();
	  axes2->SetUserTransform( transform );
	  axes2->SetXAxisLabelText( "X" );
	  axes2->SetYAxisLabelText( "Y" );
	  axes2->SetZAxisLabelText( "Z" );

	  axes2->SetTotalLength( 1.5, 1.5, 1.5 );
	  axes2->SetCylinderRadius( 0.500 * axes2->GetCylinderRadius() );
	  axes2->SetConeRadius    ( 1.025 * axes2->GetConeRadius() );
	  axes2->SetSphereRadius  ( 1.500 * axes2->GetSphereRadius() );

	  vtkTextProperty* tprop = axes2->GetXAxisCaptionActor2D()->
	    GetCaptionTextProperty();
	  tprop->ItalicOn();
	  tprop->ShadowOn();
	  tprop->SetFontFamilyToTimes();
	  tprop->SetColor(0.5,0.5,0.5);

	  axes2->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy( tprop );
	  axes2->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy( tprop );

	  // combine orientation markers into one with an assembly
	  //
	  vtkPropAssembly* assembly = vtkPropAssembly::New();
	  assembly->AddPart( axes2 );
	  assembly->AddPart( cube );
	
	vtkOrientationMarkerWidget* widget = vtkOrientationMarkerWidget::New();
	widget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
	widget->SetOrientationMarker( assembly );
	widget->SetInteractor( this->RenderWidget->GetRenderWindowInteractor() );
	widget->SetViewport( 0.0, 0.0, 0.4, 0.4 );
	widget->SetEnabled( 1 );
	widget->InteractiveOff();
	this->SavedOrientationWidget = widget;
	//widget->InteractiveOn();
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetGUIState(void)
{

  // set initial state of the check buttons on the interface.  They
  // are set directly from the state of the mesh quality filter to avoid the
  // widget starting out of sync
  
  this->meshQualityInstance->GetShowOutline() ? this->OutlineSelector->SelectedStateOn() :
 											     this->OutlineSelector->SelectedStateOff();
     
  this->meshQualityInstance->GetShowFilledElements() ? this->FilledElementSelector->SelectedStateOn() : 
  												this->FilledElementSelector->SelectedStateOff();
 
  this->meshQualityInstance->GetShowClippedOutline() ? this->ClippedOutlineSelector->SelectedStateOn() : 
   												this->ClippedOutlineSelector->SelectedStateOff();

  
  this->meshQualityInstance->GetShowSurfaceHedgehog() ?   this->HedgehogSelector->SelectedStateOn() : 
  												          this->HedgehogSelector->SelectedStateOff();
  this->meshQualityInstance->GetShowInteriorOutlines() ? this->InteriorOutlineSelector->SelectedStateOff() : 
  												         this->InteriorOutlineSelector->SelectedStateOff();
    
  // set the initial values of the sliders to be fully on

  this->ElementOpacity->SetValue(100.0*this->meshQualityInstance->GetElementOpacity());
  this->SliceScale->SetValue(100.0*this->meshQualityInstance->GetElementOpacity());
  this->InvertSelector->SetSelectedState( 1 );
  this->RenderWidget->ResetCamera();
  
  // draw an orientation cue in the corner.  this is a small actor
    this->AddOrientationWidget();
    this->OrientationCueSelector->SetSelectedState(1);
    
    this->RealTimeWarningButton->GetWidget()->SetSelectedState(1);
 
  this->Modified();
  this->Update();
  this->RenderWidget->Render();
  //this->Render();
  
}

void vtkKWMyWindow::SetColorRangeCallback(double low, double high)
{
	//cout << "color range callback";
	//cout << " low: " << low << " high: " << high << endl;
	this->meshQualityInstance->SetMeshColorRange(low,high);
	this->meshQualityInstance->UpdatePipeline();
	this->RenderWidget->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::LoadMeshCallback()
{
	
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this->RenderWidget->GetParentTopLevel()) ;
  dialog->Create();
  dialog->RetrieveLastPathFromRegistry("FEMeshDataPath");
  dialog->SetTitle ("Load Unstructured Mesh");
  dialog->SetFileTypes ("{{VTK unstructured mesh} {.vtk}}");
  dialog->SetDefaultExtension (".vtk");
  
  if ( dialog->Invoke () == 0 )
  {
    return ;
  }
  
  dialog->SaveLastPathToRegistry("FEMeshDataPath");
  // reload the reader from the new filename
  this->SavedGridReader->SetFileName(dialog->GetFileName());
  this->SavedGridReader->Update();

  // update the selection plane since a new reader was used to read the file
  this->SavedPlaneWidget->Modified();
  this->meshQualityInstance->InitializeFromExternalMesh(this->SavedGridReader->GetOutput());
  this->meshQualityInstance->SetCuttingPlaneFunction(this->SavedImplicitPlane);

  // update the name of the meshfile on the bottom panel of the interface
  char filetext[128];
  sprintf(filetext,"Current Mesh: %s",dialog->GetFileName());
  //this->BottomPanelText->SetText(filetext);
  this->SetStatusText( filetext );
  dialog->Delete();
  
  
  // clear away the selection plane in case the user forgot to
  this->ClearSelectionPlaneCallback();
	
  // remove the old actors from the previous mesh, since these are outdated 
  // add new actors we got back from the mesh quality filter after it was updated
 

  this->SavedElementActor = this->meshQualityInstance->ReturnFilledElementsActor();
  this->SavedInteriorOutlinesActor = this->meshQualityInstance->ReturnInternalOutlinesActor();
  this->SavedOutlineActor = this->meshQualityInstance->ReturnOutlineActor();
  this->SavedSurfaceHedgehogActor = this->meshQualityInstance->ReturnHedgehogActor();
  this->SavedColorLegendActor = this->meshQualityInstance->ReturnColorLegendActor();
  
  // set the rendering to have the right quality metric and reset the camera to center the new mesh
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->ResetCamera();
  this->RenderWidget->Render();
  
  // add the quality actors to the render widget
  //myRenderer->AddActor(this->SavedElementActor); 
  //myRenderer->AddActor(this->SavedInteriorOutlinesActor); 
  //myRenderer->AddActor(this->SavedOutlineActor); 
  //myRenderer->AddActor(this->SavedSurfaceHedgehogActor);
  //myRenderer->AddActor(this->SavedColorLegendActor);  
  //this->RenderWidget->ResetCamera();
  //this->RenderWidget->Render();
  
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::EnableSelectionPlaneCallback()
{
  //cout << "enable selection plane" << endl;
  this->SavedPlaneWidget->SetInput((vtkDataSet*)(this->SavedGridReader->GetOutput()));
  this->SavedPlaneWidget->PlaceWidget();
  this->SavedPlaneWidget->GetPlane( this->SavedImplicitPlane );
  this->SavedPlaneWidget->SetEnabled(1);
  this->meshQualityInstance->EnableCuttingPlane();

  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::ClearSelectionPlaneCallback()
{
  //cout << "disable selection plane" << endl;
  this->SavedPlaneWidget->SetEnabled(0);
  this->meshQualityInstance->DisableCuttingPlane();
  this->RenderWidget->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::SaveSelectionMeshCallback()
{
  if (this->SavedPlaneWidget->GetEnabled()==1) {
    //cout << "save selection as new file" << endl;
    vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
    dialog->SaveDialogOn();
    dialog->SetParent(this->RenderWidget->GetParentTopLevel()) ;
    dialog->Create();
    dialog->RetrieveLastPathFromRegistry("FEMeshDataPath");
    dialog->SetTitle ("Save New Unstructured Mesh File");
    dialog->SetFileTypes ("{{VTK unstructured mesh} {.vtk}}");
    dialog->SetDefaultExtension (".vtk");
    if ( dialog->Invoke () == 0 )
    {
      return ;
    }
    dialog->SaveLastPathToRegistry("FEMeshDataPath");
    char newFileName[256];
    sprintf(newFileName,"%s",dialog->GetFileName());
    this->meshQualityInstance->SaveSelectionAsNewMeshFile(newFileName);
    dialog->Delete();
  } else {
		return;
  }
}




//----------------------------------------------------------------------------
void vtkKWMyWindow::SetThresholdFromScaleCallback(double value)
{
  value = value / 100.0;
  this->meshQualityInstance->SetThresholdValue(value);
  this->SavedThresholdValue = value;
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}



//----------------------------------------------------------------------------
void vtkKWMyWindow::SetElementSizeFromScaleCallback(double value)
{
  value = value / 100.0;
  this->meshQualityInstance->SetElementShrinkFactor(value);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::SetOutlineEnableFromButton(int state)
{
  cout << "outline button callback " << state << endl;
  this->meshQualityInstance->SetShowOutline(state);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::SetClippedOutlineEnableFromButton(int state)
{
  //cout << "clipped outline button callback " << endl;
  this->meshQualityInstance->SetShowClippedOutline(state);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}



//----------------------------------------------------------------------------
void vtkKWMyWindow::SetInteriorOutlineEnableFromButton(int state)
{
  //cout << "interior outline button callback " << endl;
  this->meshQualityInstance->SetShowInteriorOutlines(state);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::SetFilledElementEnableFromButton(int state)
{
  //cout << "filled element button callback " << endl;
  this->meshQualityInstance->SetShowFilledElements(state);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
  this->RenderWidget->Render();
  this->Update();
}  

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetHedgehogEnableFromButton(int state)
{
  //cout << "hedgehog button callback " << endl;
  this->meshQualityInstance->SetShowSurfaceHedgehog(state);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}  

//----------------------------------------------------------------------------
void vtkKWMyWindow::SetHedgehogScale(double value)
{
  //cout << "hedgehog scale callback " << endl;
  this->meshQualityInstance->SetHedgehogScale(value);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}  



//----------------------------------------------------------------------------
void vtkKWMyWindow::SetInvertSelectionFromButton(int state)
{
  //cout << "invert button callback " << endl;
  this->meshQualityInstance->SetInvertCuttingPlaneSelection(state);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::SetOrientationCueEnableFromButton(int state)
{
  //cout << "orientation cue button callback " << endl;
  this->SavedOrientationWidget->SetEnabled(state);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::SetOverrideColorRangeFromButton(int state)
{
	double range[2];
	
  //cout << "override color button callback " << endl;
  this->meshQualityInstance->SetOverrideMeshColorRange(state);
  
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
	  this->meshQualityInstance->UpdatePipeline();
	  this->RenderWidget->Render();
  }
}


void vtkKWMyWindow::SetHighlightCellsFromButton(int state)
{
	
  if (state)
  {
	  this->meshQualityInstance->HighlightFocusElements();
	  this->RenderWidget->Render();
  }
  else 
  {
	  // called to update the pipeline back to the default colors
	  this->meshQualityInstance->CancelElementHighlight();
	  this->RenderWidget->Render();
  }
}


void vtkKWMyWindow::SetRealTimeWarningUpdateFromButton(int state)
{
	  this->RealTimeWarningUpdate = state;
}


void vtkKWMyWindow::SetDecimalPrecisionCallback(char* charValue)
{
	if (strlen(charValue))
	{
	  //cout << "set display precision to"  << charValue << " places" << endl;
	  this->DecimalDisplayPrecision = atoi(charValue);
	  this->meshQualityInstance->SetDisplayPrecision(this->DecimalDisplayPrecision);
	  this->meshQualityInstance->UpdatePipeline();
	  this->RenderWidget->Render();
    }
}




//----------------------------------------------------------------------------
void vtkKWMyWindow::SetElementAlphaFromScaleCallback(double aFloatValue)
{
 
   aFloatValue = aFloatValue / 100.0;
  this->meshQualityInstance->SetElementOpacity(aFloatValue);
  this->meshQualityInstance->UpdatePipeline();
  this->RenderWidget->Render();
}

void vtkKWMyWindow::UpdateRenderingState(void)
{
  this->meshQualityInstance->SetThresholdValue(this->SavedThresholdValue);
  this->meshQualityInstance->UpdatePipeline();
  vtkCamera *thisCamera = this->RenderWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
  thisCamera->Modified();
  this->RenderWidget->GetRenderWindow()->Render();
}


//----------------------------------------------------------------------------
void vtkKWMyWindow::SetQualityMetricFromButton(void) {this->meshQualityInstance->SetQualityMeasure(0);this->UpdateRenderingState();}
void vtkKWMyWindow::SetQualityMetricFromButton1(void) {this->meshQualityInstance->SetQualityMeasure(1);this->UpdateRenderingState();}
void vtkKWMyWindow::SetQualityMetricFromButton2(void) {this->meshQualityInstance->SetQualityMeasure(2);this->UpdateRenderingState();}
void vtkKWMyWindow::SetQualityMetricFromButton3(void) {this->meshQualityInstance->SetQualityMeasure(3);this->UpdateRenderingState();}
void vtkKWMyWindow::SetQualityMetricFromButton4(void) {this->meshQualityInstance->SetQualityMeasure(4);this->UpdateRenderingState();}

void vtkKWMyWindow::TakeWindowSnapshot(void) 
{
    static int imagecount;
	vtkWindowToImageFilter *imageout = vtkWindowToImageFilter::New();
    imageout->SetInput(this->RenderWidget->GetRenderWindow());
    vtkJPEGWriter *imagewriter = vtkJPEGWriter::New();
    imagewriter->SetInput(imageout->GetOutput());
    char filename[256];
    sprintf(filename,"meshqual-%03d.jpg",imagecount);
	//cout << "writing snapshot to file: " << filename;
    imagewriter->SetFileName(filename);
    imagewriter->Write();
    imagecount++;
    //cout << "...done" << endl;
}

void vtkKWMyWindow::TakeWindowSnapshotWithFilename(void)
{
	
	vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this->RenderWidget->GetParentTopLevel()) ;
  dialog->Create();
  dialog->RetrieveLastPathFromRegistry("FEMeshDataPath");
  dialog->SetTitle ("Save Snapshot (JPEG)");
  dialog->SetFileTypes ("{{JPEG Image} {.jpg}}");
  dialog->SaveDialogOn();
  dialog->SetDefaultExtension (".jpg");
  
  if ( dialog->Invoke () == 0 )
  {
    return ;
  }
 
  vtkWindowToImageFilter *imageout = vtkWindowToImageFilter::New();
  imageout->SetInput(this->RenderWidget->GetRenderWindow());
  vtkJPEGWriter *imagewriter = vtkJPEGWriter::New();
  imagewriter->SetInput(imageout->GetOutput());
  imagewriter->SetFileName( dialog->GetFileName() );
  imagewriter->Write();
}
