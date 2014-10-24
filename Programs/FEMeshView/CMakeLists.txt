PROJECT(MimxMeshQualityViewer)


#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
KWWidgets
VTK
)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
                    ${IAFEMMESH_SOURCE_DIR}/Code/Filter
                    ${VTK_SOURCE_DIR}/Utilities/verdict/include 
                    ${VTK_KITS_DIR}/verdict)


#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project
SET(CurrentExe "KWMeshQualityViewer")
SET(LIB_NAME "vtkKWMeshQualityViewerLib")
SET(LIB_SRCS 
  vtkKWMyWindow.cxx
  vtkKWMeshViewerApplication.cxx
  vtkMeshQualityClass.cxx
  vtkFaceHedgeHog.cxx
  vtkSlicerTheme.cxx
  vtkSlicerColor.cxx
  vtkSlicerFont.cxx
  vtkSlicerCheckRadioButtonIcons.cxx
  vtkSlicerIcons.cxx
)
	

INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
KWWidgets_WRAP_TCL(${LIB_NAME} LIB_TCL_SRCS "${LIB_SRCS}" "")

# Create the library (in static mode for convenience).
ADD_LIBRARY(${LIB_NAME} STATIC ${LIB_TCL_SRCS} ${LIB_SRCS})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${KWWidgets_LIBRARIES})

# The name of our executable and the corresponding source file.

SET(EXE_SRCS "${CurrentExe}.cxx")

# This example uses some files from the KWWidgets distribution tree.
# Let's configure KWWidgets's vtkKWWidgetsPaths.h.in into our
# own header file so that we can find the paths to KWWidgets files.

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(
  ${KWWidgets_TEMPLATES_DIR}/vtkKWWidgetsPaths.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkKWWidgetsPaths.h)

# Create the executable, and link it against the KWWidgets libraries and our
# own library.

ADD_EXECUTABLE(${CurrentExe} WIN32 ${EXE_SRCS}) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${LIB_NAME} ${KWWidgets_LIBRARIES} 
                      mimxFilter
                      vtkRendering vtkGraphics vtkImaging vtkIO 
                      vtkWidgets vtkFiltering  vtkCommon )


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
  SET(LAUNCHER_EXE_NAME "${CurrentExe}Launcher")
  KWWidgets_GENERATE_SETUP_PATHS_LAUNCHER(
    "${CMAKE_CURRENT_BINARY_DIR}" "${LAUNCHER_EXE_NAME}" "" "${CurrentExe}")
ENDIF(NOT KWWidgets_SOURCE_DIR)

# If needed, copy the Tcl/Tk support files required at run-time 
# to initialize Tcl/Tk. This is only triggered if VTK was built
# against a Tcl/Tk static library.

INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsTclTkMacros.cmake")
IF(NOT KWWidgets_SOURCE_DIR)
  KWWidgets_COPY_TCL_TK_SUPPORT_FILES("${PROJECT_BINARY_DIR}/lib")
ENDIF(NOT KWWidgets_SOURCE_DIR)

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

#IF(BUILD_TESTING)
#  INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsTestingMacros.cmake")
#  KWWidgets_ADD_TEST_FROM_C_EXAMPLE(KWWidgets-${PROJECT_NAME} ${EXE_NAME})
#ENDIF(BUILD_TESTING)

INSTALL(TARGETS ${CurrentExe} ${CurrentExe}Launcher RUNTIME DESTINATION bin)

