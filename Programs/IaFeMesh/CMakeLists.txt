PROJECT(IaFeMeshGui)   
SET (IaFeMesh_GUI_SRCS IaFeMesh.fl )
SET (R21_GUI_SRCS R21.fl )
INCLUDE_DIRECTORIES( ${IaFeMeshGui_SOURCE_DIR} ${IaFeMeshGui_BINARY_DIR} ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/IO ${IAFEMMESH_BINARY_DIR}/Code/IO )  
IF(FLTK_FOUND)   
INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})   
LINK_LIBRARIES(${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY} ${OPENGL_LIBRARIES})   
FLTK_WRAP_UI( IaFeMesh ${IaFeMesh_GUI_SRCS} )
#FLTK_WRAP_UI( R21 ${R21_GUI_SRCS} )  
ENDIF(FLTK_FOUND)    
ADD_EXECUTABLE( IaFeMesh    ${IaFeMesh_FLTK_UI_SRCS}   Main.cxx   vtkFlRenderWindowInteractor.cxx   vtkInteractorStyleSelect.cxx )    
TARGET_LINK_LIBRARIES( IaFeMesh      mimxWidgets mimxFilter      mimxCommon      mimxIO vtkWidgets
    vtkHybrid      vtkRendering       vtkGraphics      vtkImaging      vtkIO      vtkFiltering      vtkCommon          ITKBasicFilters     ITKIO
    ITKCommon
    itkvnl_algo
    itkvnl )     
###Remove this old FLTK APPLICATION
#ADD_EXECUTABLE( R21    ${R21_FLTK_UI_SRCS}   Main.cxx   vtkFlRenderWindowInteractor.cxx   vtkInteractorStyleSelect.cxx )    
#TARGET_LINK_LIBRARIES( R21      mimxWidgets mimxFilter      mimxCommon      mimxIO vtkWidgets
#    vtkHybrid      vtkRendering       vtkGraphics      vtkImaging      vtkIO      vtkFiltering      vtkCommon          ITKBasicFilters     ITKIO
#    ITKCommon
#    itkvnl_algo
#    itkvnl )     
####
INSTALL(TARGETS IaFeMesh RUNTIME DESTINATION bin)
