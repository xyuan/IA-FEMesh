//

//
//  Created by Curtis Lisle on 12/12/06.
//  Copyright KnowledgeVis, LLC 2007. 

#include "vtkKWMeshViewerApplication.h"
#include "vtkKWMyWindow.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>

#include "vtkKWRenderWidget.h"
#include "vtkRenderer.h"

#include "vtkSlicerTheme.h"

#include "vtkMeshQualityClass.h"

// We define several classes in this example, and we want to be able to use
// their C++ methods as callbacks for our user interface. To do so, we created
// a library and wrapped it automatically for the Tcl language, which is used
// as a bridge between C++ objects at run-time. An initialization function is
// automatically created in this library to allow classes and C++ methods to
// be used as commands and callbacks inside the Tcl interpreter; let's *not* 
// forget to declare *and* call this function right after we initialize the Tcl
// interpreter in our application. The name of this function is built from the
// library name in lower-case (except for the first letter) and suffixed with
// "_Init" (for example: KWCallbacksExampleLib => Kwcallbacksexamplelib_Init).
// This whole process is required to use C++ methods as callbacks; it is not
// needed if you use VTK's C++ command/observer pattern directly, which is
// demonstrated in KWWidgets's C++ 'Callbacks' example.

extern "C" int Vtkkwmeshqualityviewerlib_Init(Tcl_Interp *interp);


//--------------------------------------------------------------------------------------------
int my_main(int argc, char* argv[])
{

   vtkMeshQualityClass *mq; 
  
   // Initialize Tcl

  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);
  if (!interp)
    {
    cerr << "Error: InitializeTcl failed" << endl ;
    return 1;
    }
    
      Vtkkwmeshqualityviewerlib_Init(interp);
    
  // Process some command-line arguments
  // The --test option here is used to run this example as a non-interactive 
  // test for software quality purposes. You can ignore it.

  int option_test = 0;
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  args.AddArgument(
    "--test", vtksys::CommandLineArguments::NO_ARGUMENT, &option_test, "");
  args.Parse();
    
    
  // Create the application
  // If --test was provided, ignore all registry settings, and exit silently
  // Restore the settings that have been saved to the registry, like
  // the geometry of the user interface so far.

  vtkKWMeshViewerApplication *app = vtkKWMeshViewerApplication::New();
  //vtkKWApplication *app = vtkKWApplication::New();
  app->InstallDefaultTheme();
  app->SetName("Hexahedral Mesh Quality Viewer");  
  if (option_test)
    {
    app->SetRegistryLevel(0);
    app->PromptBeforeExitOff();
    }

  //app->RestoreApplicationSettingsFromRegistry();

  // Set a help link. Can be a remote link (URL), or a local file
  app->SetHelpDialogStartingPage("http://www.ccad.uiowa.edu/mimx");

  

  // Add a window
  // Set 'SupportHelp' to automatically add a menu entry for the help link

  vtkKWMyWindow *win = (vtkKWMyWindow*)vtkKWMyWindow::New();
  win->SetTitle("Hexahedral Mesh Quality Viewer");
  win->SupportHelpOn();
  app->AddWindow(win);
  win->Create();

  win->SecondaryPanelVisibilityOff();

  
  // Start the application
  // If --test was provided, do not enter the event loop and run this example
  // as a non-interactive test for software quality purposes.

  int ret = 0;
  win->Display();
  win->SetGUIState();
  if (!option_test)
    {
    //cout << "app sstarting" << endl;
    app->Start(argc, argv);

    ret = app->GetExitStatus();
    }
  win->Close();

}

#ifdef _WIN32
#include <windows.h>
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
  int argc;
  char **argv;
  vtksys::SystemTools::ConvertWindowsCommandLineToUnixArguments(
    lpCmdLine, &argc, &argv);
  int ret = my_main(argc, argv);
  for (int i = 0; i < argc; i++) { delete [] argv[i]; }
  delete [] argv;
  return ret;
}
#else
int main(int argc, char *argv[])
{
  return my_main(argc, argv);
}
#endif


