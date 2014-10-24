#ifndef __vtkKWMyWindow_h
#define __vtkKWMyWindow_h

#include "vtkKWWindow.h"

// include definition of custom class for mesh quality widget

#include "vtkMeshQualityClass.h"
#include "vtkPlaneWidget.h"
#include "vtkActor2D.h"
#include "vtkUnstructuredGridReader.h"



class vtkKWRenderWidget;
class vtkKWScale;
class vtkKWCheckButton;
class vtkKWRadioButton;
class vtkKWRadioButtonSet;
class vtkKWPushButtonSet;
class vtkKWListBoxWithScrollbars;
class vtkKWThumbWheel;
class vtkKWRange;
class vtkKWCheckButtonWithLabel;
class vtkKWText;
class vtkKWComboBoxWithLabel;
class vtkOrientationMarkerWidget;
class vtkKWLoadSaveButton;


class vtkKWMyWindow : public vtkKWWindow
{
public:
  static vtkKWMyWindow* New();
  vtkTypeRevisionMacro(vtkKWMyWindow,vtkKWWindow);
 
  // after the GUI is instantiated, set the sliders and buttons
  // to a known initial state
  void SetGUIState(void);
  
  
  // initialize with an initial file when the window is instantiated.  Use this call
  // before or after instantiating the window to load a dataset.  This can be used by the
  // application to provide a command line argument which is a mesh name. 
  
  void InitializeInterfaceFromMeshFile(char* filename);
  
  // Description:
  // Callbacks
  virtual void SetThresholdFromScaleCallback(double value);
  virtual void SetElementSizeFromScaleCallback(double value);
  virtual void SetOutlineEnableFromButton(int state);
  virtual void SetClippedOutlineEnableFromButton(int state);
  virtual void SetInteriorOutlineEnableFromButton(int state);
  virtual void SetFilledElementEnableFromButton(int state);
  virtual void SetHedgehogEnableFromButton(int state);
  virtual void SetQualityMetricFromButton(void);
  virtual void SetQualityMetricFromButton1(void);
  virtual void SetQualityMetricFromButton2(void);
  virtual void SetQualityMetricFromButton3(void);
  virtual void SetQualityMetricFromButton4(void);
  virtual void SetElementAlphaFromScaleCallback(double aFloatValue);
  virtual void SetHedgehogScale(double value);
   
  virtual void ClearSelectionPlaneCallback();
  virtual void EnableSelectionPlaneCallback();
  virtual void SaveSelectionMeshCallback();
  virtual void LoadMeshCallback();
  virtual void SetInvertSelectionFromButton(int state);
  virtual void SetColorRangeCallback(double low, double high);
  virtual void SetOverrideColorRangeFromButton(int state);
  virtual void SetHighlightCellsFromButton(int state);
  virtual void SetDecimalPrecisionCallback(char* charValue);
  virtual void SetOrientationCueEnableFromButton(int state);
  virtual void SetRealTimeWarningUpdateFromButton(int state);
  virtual void TakeWindowSnapshot(void);
  virtual void TakeWindowSnapshotWithFilename(void);

  void UpdateRenderingState(void);
  
  int	RealTimeWarningUpdate;    // plane movement updates display

  // needs to be visible to the plane callback
  vtkKWCheckButtonWithLabel		 *HighlightCellsButton;
  vtkMeshQualityClass 			 *meshQualityInstance;
  
protected:
  vtkKWMyWindow();
  ~vtkKWMyWindow();
  
  // create a moveable plane widget and a corresponding implicit function which
  // can be passed to the quality class to update rendered information
  
  vtkPlaneWidget* SavedPlaneWidget;
  vtkPlane* SavedImplicitPlane; 
  int CuttingPlaneEnabled;
  
  // we have to keep a persistent pointer to the actors returned from the mesh 
  // quality class if we want to reload new meshes during the course of execution
  // When we reload from a new mesh, the actors change, so we need to keep pointers
  // here to delete the old ones and add the new ones (see the LoadMeshCallback method)
  
  vtkActor *SavedElementActor;
  vtkActor *SavedOutlineActor;
  vtkActor *SavedClippedOutlineActor;
  vtkActor *SavedInteriorOutlinesActor;
  vtkActor *SavedSurfaceHedgehogActor;      
  vtkActor2D *SavedColorLegendActor;
  vtkUnstructuredGridReader *SavedGridReader;
  
  // Description:
  // Create the widget.
  virtual void CreateWidget();
 

  vtkKWScale                     *SliceScale;
  vtkKWScale					 *ThresholdScale;
  vtkKWRenderWidget              *RenderWidget;
  vtkKWScale					 *ElementOpacity;
  vtkKWScale					 *CuttingPlaneOpacityScale;
  vtkKWRadioButtonSet			 *QualityButtons;
  vtkShrinkFilter				 *myshrink;
  vtkKWCheckButton		 		 *OutlineSelector;
  vtkKWCheckButton				 *ClippedOutlineSelector;
  vtkKWCheckButton				 *InteriorOutlineSelector;
  vtkKWCheckButton				 *FilledElementSelector;
  vtkKWThumbWheel				 *HedgehogScale;
  vtkKWCheckButton				 *HedgehogSelector;
  vtkKWCheckButton		 		 *InvertSelector;

  vtkKWRadioButtonSet			 *selectionControls;
  vtkKWPushButtonSet			 *meshControls;
  vtkKWRange					 *ElementColorRange;
  vtkKWCheckButtonWithLabel		 *OverrideColorButton;

  vtkKWText						 *BottomPanelText;
  vtkKWComboBoxWithLabel		 *DecimalDisplayPrecisionWidget;
  vtkKWCheckButton				 *OrientationCueSelector;
  vtkOrientationMarkerWidget	 *SavedOrientationWidget;
  vtkKWCheckButtonWithLabel		 *RealTimeWarningButton;
  vtkKWLoadSaveButton			 *SavedLoadSaveButton;
  
  int 							 RadioButtonValue;
  float 						 SavedThresholdValue;
  int							 DecimalDisplayPrecision;
 

private:
  vtkKWMyWindow(const vtkKWMyWindow&);   // Not implemented.
  void operator=(const vtkKWMyWindow&);  // Not implemented.
  

  void AddOrientationWidget(void);
  
};

#endif
