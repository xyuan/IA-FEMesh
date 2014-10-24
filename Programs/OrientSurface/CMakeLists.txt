PROJECT(OrientSurface)

  
SET (OrientSurface_GUI_SRCS CenterMeshGUI.fl )

INCLUDE_DIRECTORIES( ${OrientSurface_SOURCE_DIR} ${OrientSurface_BINARY_DIR} )


IF(FLTK_FOUND)
  INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  LINK_LIBRARIES(${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY} ${OPENGL_LIBRARIES})
  FLTK_WRAP_UI( OrientSurfaceGui ${OrientSurface_GUI_SRCS} )
ENDIF(FLTK_FOUND)
 
 

ADD_EXECUTABLE(OrientSurface CenterMesh.cxx )
ADD_EXECUTABLE(OrientSurfaceGui ${OrientSurfaceGui_FLTK_UI_SRCS} 
                                CenterMainFunction.cxx 
				OrientPolyData.cxx 
                                OrientPolyData.h )


TARGET_LINK_LIBRARIES(OrientSurface vtkIO vtkHybrid vtkFiltering vtkCommon)
TARGET_LINK_LIBRARIES(OrientSurfaceGui  vtkIO vtkHybrid vtkFiltering vtkCommon)

INSTALL(TARGETS OrientSurface OrientSurfaceGui RUNTIME DESTINATION bin)
