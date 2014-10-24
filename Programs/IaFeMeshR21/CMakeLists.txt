PROJECT(IaFeMeshR21)


# The name of our targets (executable or libraries) will simply be based
# on the project name, with an extra prefix and suffix.

SET(TARGET_BASE_NAME "IA-FEMesh")

# We define several classes in this example, and we want to be able to use
# their C++ methods as callbacks for our user interface. To do so, we need to 
# create a library and wrap it automatically for the Tcl language, which
# is used as a bridge between C++ objects at run-time. Note that an
# initialization function is automatically created in this library to allow
# classes and C++ methods to be used as commands and callbacks inside the Tcl
# interpreter; do *not* forget to call this function right after you 
# initialize the Tcl interpreter in your application. The name of this 
# function is built from the library name in lower-case (except for the first
# letter) and suffixed with "_Init" (for example: 
# KWR21ExampleLib => KwR21examplelib_Init)
# This whole process is required to use C++ methods as callbacks; it is not
# needed if you use VTK's C++ command/observer pattern directly, which is
# demonstrated in KWWidgets's C++ 'Callbacks' example.

# directories for vtkMIMX
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/IO ${IAFEMMESH_BINARY_DIR}/Code/IO )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Mesh    ${IAFEMMESH_BINARY_DIR}/Code/Mesh )  
INCLUDE_DIRECTORIES( ${VTK_SOURCE_DIR}/Utilities/verdict/include ${VTK_KITS_DIR}/verdict )
                    
SET(LIB_NAME "IaFeMeshR21Lib")
SET(LIB_SRCS 
  vtkKWMimxApplication.cxx
  vtkKWMimxFEMeshMenuGroup.cxx
  vtkKWMimxImageMenuGroup.cxx
  vtkKWMimxMaterialPropertyMenuGroup.cxx
  vtkKWMimxGroupBase.cxx
  vtkKWMimxMainNotebook.cxx
  vtkKWMimxMainMenuGroup.cxx
#  vtkKWMimxMenuWindow.cxx
  vtkKWMimxSurfaceMenuGroup.cxx
  vtkKWMimxBBMenuGroup.cxx
#  vtkKWRenderWidget.cxx
  vtkKWMimxEvaluateMeshQualityGroup.cxx
  vtkKWMimxQualityMenuGroup.cxx
  vtkKWMimxCreateBBFromBoundsGroup.cxx
  vtkKWMimxCreateBBMeshSeedGroup.cxx
  vtkKWMimxEditBBGroup.cxx
  vtkKWMimxCreateFEMeshFromBBGroup.cxx
  vtkKWMimxViewProperties.cxx
  vtkKWMimxSaveSTLSurfaceGroup.cxx
  vtkKWMimxSaveVTKSurfaceGroup.cxx
  vtkKWMimxSaveVTKBBGroup.cxx
  vtkKWMimxEditBBMeshSeedGroup.cxx
  vtkKWMimxSaveVTKFEMeshGroup.cxx
  vtkKWMimxDeleteObjectGroup.cxx
  vtkKWMimxEditFEMeshLaplacianSmoothGroup.cxx
  vtkKWMimxCreateExtrudeFEMeshGroup.cxx
  vtkKWMimxCreateSurfaceFromContourGroup.cxx
  vtkKWMimxNodeElementNumbersGroup.cxx
  vtkKWMimxMergeMeshGroup.cxx
  vtkKWMimxExportAbaqusFEMeshGroup.cxx
  vtkKWMimxMergeBBGroup.cxx
  vtkKWMimxImageViewProperties.cxx
  vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.cxx
  vtkKWMimxReBinFEMeshMaterialPropertiesFromImageGroup.cxx
  vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup.cxx
  vtkKWMimxCreateElementSetPickFaceGroup.cxx
  vtkKWMimxMirrorBBGroup.cxx
  vtkKWMimxMainWindow.cxx
  vtkKWMimxCreateFEMeshNodeSetGroup.cxx
  vtkKWMimxCreateFEMeshElementSetGroup.cxx
  vtkKWMimxMainUserInterfacePanel.cxx
  vtkKWMimxViewPropertiesGroup.cxx
  vtkKWMimxDisplayPropertiesGroup.cxx
  vtkFaceHedgeHog.cxx
  vtkKWMimxConstMatPropElSetGroup.cxx
  vtkKWMimxBoundaryConditionsMenuGroup.cxx
  vtkKWMimxAssignBoundaryConditionsGroup.cxx
  vtkKWMimxDeleteBoundaryConditionsGroup.cxx
  vtkKWMimxEditElementSetNumbersGroup.cxx
  vtkKWMimxEditNodeSetNumbersGroup.cxx
  vtkSlicerTheme.cxx
  vtkSlicerColor.cxx
  vtkSlicerFont.cxx
  vtkSlicerCheckRadioButtonIcons.cxx
  vtkSlicerIcons.cxx
  vtkKWMimxApplicationSettingsInterface.cxx
  vtkKWMimxCreateBBFromRubberBandPickGroup.cxx
  vtkKWMimxDefineElSetGroup.cxx
  vtkKWMimxDisplayMatPropGroup.cxx
  vtkKWMimxViewPropertiesOptionGroup.cxx
  vtkKWMimxDefineNodeSetGroup.cxx
  )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
KWWidgets_WRAP_TCL(${LIB_NAME} LIB_TCL_SRCS "${LIB_SRCS}" "")

# Create the library. The library is built in static mode for convenience. 
# Check the 'Callbacks' example for more information about building it in
# shared mode, i.e. without the STATIC keyword (Win32 compilers requires
# an additional header file to setup DLL export symbols correctly).

ADD_LIBRARY(${LIB_NAME} STATIC ${LIB_TCL_SRCS} ${LIB_SRCS})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${KWWidgets_LIBRARIES})

# The name of our executable and the corresponding source file.

SET(EXE_NAME "${TARGET_BASE_NAME}")
#SET(EXE_NAME IaFeMeshR21)
SET(EXE_SRCS IaFeMeshR21.cxx)

# On Win32 platforms, let's create a .rc resource file to setup a decent
# application icon as well as some additional information in the "Version"
# tab of the properties panel.

IF(WIN32 AND NOT BORLAND AND NOT CYGWIN)
  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsResourceMacros.cmake")
  SET(RC_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.rc")
  KWWidgets_CREATE_RC_FILE(
    RC_FILENAME "${RC_FILENAME}"
    RC_APPLICATION_NAME "IA-FEMesh"
    RC_COMPANY_NAME "MIMX, The University of Iowa"
    RC_ICON_BASENAME ${IaFeMeshR21_SOURCE_DIR}/)
ENDIF(WIN32 AND NOT BORLAND AND NOT CYGWIN)

# This example uses some files from the KWWidgets distribution tree.
# Let's configure KWWidgets's vtkKWWidgetsPaths.h.in into our
# own header file so that we can find the paths to KWWidgets files.

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(
  ${KWWidgets_TEMPLATES_DIR}/vtkKWWidgetsPaths.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkKWWidgetsPaths.h)

# Create the executable, and link it against the KWWidgets libraries and our
# own library.

ADD_EXECUTABLE(${EXE_NAME} WIN32 ${EXE_SRCS} ${RC_FILENAME})
TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME} 
    mimxWidgets 
    mimxFilter      
	mimxCommon      
    mimxIO  
    ITKCommon
    ITKBasicFilters 
    ITKIO 
    ITKAlgorithms
    vtkWidgets
    vtkRendering
    vtkImaging
    vtkHybrid 
    vtkFiltering 
    vtkIO 
    vtkCommon)

# If we are building this example as a standalone external project:
# - Generate a few small scripts (.bat, .sh, .csh) that can be sourced to set
# the various environments variables (PATH, TCLLIBPATH, LD_LIBRARY_PATH, etc.) 
# required by this executable and its known third-party dependencies (VTK, ITK,
# SOV, KWWidgets, etc.).
# - Generate a lightweight C launcher for this *specific* executable: It sets
# the above environment variables before launching the executable itself.

IF(NOT KWWidgets_SOURCE_DIR)
  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsPathsMacros.cmake")
  KWWidgets_GENERATE_SETUP_PATHS_SCRIPTS("${CMAKE_CURRENT_BINARY_DIR}")
  SET(LAUNCHER_EXE_NAME "${EXE_NAME}Launcher")
  KWWidgets_GENERATE_SETUP_PATHS_LAUNCHER(
    "${CMAKE_CURRENT_BINARY_DIR}" "${LAUNCHER_EXE_NAME}" "" "${EXE_NAME}")
ENDIF(NOT KWWidgets_SOURCE_DIR)

# If needed, copy the Tcl/Tk support files required at run-time 
# to initialize Tcl/Tk. This is only triggered if VTK was built
# against a Tcl/Tk static library.

INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsTclTkMacros.cmake")
IF(NOT KWWidgets_SOURCE_DIR)
  KWWidgets_COPY_TCL_TK_SUPPORT_FILES("${PROJECT_BINARY_DIR}/lib")
ENDIF(NOT KWWidgets_SOURCE_DIR)
KWWidgets_INSTALL_TCL_TK_SUPPORT_FILES("/lib")

# Install the example target. If you are using this CMakeLists.txt file
# to create your own application based on KWWidgets, you can most likely 
# omit this section or change the installation directory, unless you want
# to install your application right where KWWidgets is already installed.

IF(KWWidgets_BUILD_EXAMPLES)
  INSTALL_TARGETS(${KWWidgets_INSTALL_BIN_DIR} ${EXE_NAME})
ENDIF(KWWidgets_BUILD_EXAMPLES)

# Register this example as a test. Our executable supports a --test
# configuration option so that it can be run non-interactively as a test.
# If you are using this CMakeLists.txt file to create your own application
# based on KWWidgets, you should omit this section, unless your application
# supports that feature too and you checked how the macro is working.

IF(BUILD_TESTING)
  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsTestingMacros.cmake")
  KWWidgets_ADD_TEST_FROM_C_EXAMPLE(KWWidgets-${PROJECT_NAME} ${EXE_NAME})
ENDIF(BUILD_TESTING)

INSTALL_FILES(/bin "(\\.png)$")
CONFIGURE_FILE(${IaFeMeshR21_SOURCE_DIR}/IA-FEMesh_splash.png ${EXECUTABLE_OUTPUT_PATH}/IA-FEMesh_splash.png COPYONLY IMMEDIATE)

INSTALL(TARGETS ${EXE_NAME} ${EXE_NAME}Launcher RUNTIME DESTINATION bin)

