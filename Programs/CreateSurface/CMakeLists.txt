PROJECT(CreateSurface)

  
SET (CreateSurface_GUI_SRCS CreateSurfaceGui.fl )
SET (SurfaceNormal_GUI_SRCS GenerateSurfaceNormalsGui.fl )

INCLUDE_DIRECTORIES( ${CreateSurface_SOURCE_DIR} ${CreateSurface_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )


IF(FLTK_FOUND)
  INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  LINK_LIBRARIES(${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY} ${OPENGL_LIBRARIES})
  FLTK_WRAP_UI( CreateSurfaceGui ${CreateSurface_GUI_SRCS} )
  FLTK_WRAP_UI( SurfaceNormalGui ${SurfaceNormal_GUI_SRCS} )
ENDIF(FLTK_FOUND)
 
 

#ADD_EXECUTABLE(CreateSurface GenerateSurfaceFromMask.cxx CreateSurface.cxx)
ADD_EXECUTABLE(CreateSurfaceGui ${CreateSurfaceGui_FLTK_UI_SRCS} 
               CreateSurfaceMainFunction.cxx 
				       CreateSurface.cxx )


#TARGET_LINK_LIBRARIES(OrientSurface vtkIO vtkHybrid vtkFiltering vtkCommon)
TARGET_LINK_LIBRARIES(CreateSurfaceGui  vtkIO vtkHybrid vtkFiltering vtkCommon)

ADD_EXECUTABLE(SurfaceNormalGui ${SurfaceNormalGui_FLTK_UI_SRCS} 
               SurfaceNormalMainFunction.cxx 
			   GenerateNormals.cxx )


TARGET_LINK_LIBRARIES(SurfaceNormalGui  vtkIO vtkHybrid vtkFiltering vtkCommon)

INSTALL(TARGETS SurfaceNormalGui CreateSurfaceGui RUNTIME DESTINATION bin)

