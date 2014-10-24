/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMainWindow.h,v $
Language:  C++
Date:      $Date: 2008/05/19 00:06:08 $
Version:   $Revision: 1.18 $

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

#ifndef __vtkKWMimxMainWindow_h
#define __vtkKWMimxMainWindow_h

#include "vtkKWWindow.h"

class vtkCallbackCommand;
class vtkRenderer;
class vtkPVAxesActor;
class vtkKWRenderWidget;
class vtkKWChangeColorButton;
class vtkMimxErrorCallback;
class vtkKWFrameWithScrollbar;
class vtkKWMenu;
//class vtkKWMimxMainNotebook;
class vtkKWMimxViewProperties;
class vtkLinkedListWrapperTree;
class vtkKWLoadSaveDialog;
class vtkKWMimxMainUserInterfacePanel;
class vtkKWMimxDisplayPropertiesGroup;
class vtkKWIcon;
//class vtkMimxUnstructuredGridWidget;

class vtkKWMimxMainWindow : public vtkKWWindow
{
public:
	static vtkKWMimxMainWindow* New();
	vtkTypeRevisionMacro(vtkKWMimxMainWindow,vtkKWWindow);
	vtkGetObjectMacro(RenderWidget, vtkKWRenderWidget);
  vtkGetObjectMacro(TraceRenderWidget, vtkKWRenderWidget);
	vtkGetObjectMacro(ErrorCallback, vtkMimxErrorCallback);
	vtkGetObjectMacro(ViewProperties, vtkKWMimxViewProperties);
	vtkGetObjectMacro(PVAxesActor, vtkPVAxesActor);
	vtkGetObjectMacro(
		MainUserInterfacePanel, vtkKWMimxMainUserInterfacePanel);
	vtkKWRenderWidget *RenderWidget;
	vtkKWRenderWidget *TraceRenderWidget;
	vtkRenderer *AxesRenderer;
	vtkPVAxesActor *PVAxesActor;
	vtkCallbackCommand *CallbackCommand;
	
	void DisplayPropertyCallback();
	void SetApplicationFontFamily( const char *font );
	void SetApplicationFontSize( const char *size );
	void AutoSaveCallback( );
	void EnableAutoSave( );
	void DisableAutoSave( );
	
	
	vtkKWIcon* GetApplyButtonIcon();
	vtkKWIcon* GetCancelButtonIcon();
	
	// Description:
  // Get the Application Settings Interface as well as the Application
  // Settings User Interface Manager.
  virtual vtkKWApplicationSettingsInterface *GetApplicationSettingsInterface();
	
protected:
	vtkKWMimxMainWindow();
	~vtkKWMimxMainWindow();
	virtual void CreateWidget();
	vtkKWChangeColorButton *ChangeColorButton;
	vtkMimxErrorCallback *ErrorCallback;
	vtkKWMenu *ViewMenu;
	vtkKWFrameWithScrollbar *MainNoteBookFrameScrollbar;
//	vtkKWMimxMainNotebook *MimxMainNotebook;
	vtkKWMimxViewProperties *ViewProperties;
	vtkLinkedListWrapperTree *DoUndoTree;
	vtkKWLoadSaveDialog *LoadSaveDialog;
	vtkKWMimxMainUserInterfacePanel *MainUserInterfacePanel;
	vtkKWMimxDisplayPropertiesGroup *DisplayPropertyDialog;
	
private:
	vtkKWMimxMainWindow(const vtkKWMimxMainWindow&);   // Not implemented.
	void operator=(const vtkKWMimxMainWindow&);  // Not implemented.
	
	vtkKWIcon *applyIcon;
	vtkKWIcon *cancelIcon; 
	
	char autoSaveEventId[256];                 
};
void updateAxis(vtkObject* caller, unsigned long , void* arg, void* );

#endif
