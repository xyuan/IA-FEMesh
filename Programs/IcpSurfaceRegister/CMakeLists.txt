PROJECT(IcpRegisterSurface)


SET (IcpRegisterSurface_GUI_SRCS ICPRegistrationGUI.fl )

INCLUDE_DIRECTORIES( ${IcpRegisterSurface_SOURCE_DIR} 
                     ${IcpRegisterSurface_BINARY_DIR} )

IF(FLTK_FOUND)
  INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  LINK_LIBRARIES(${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY} ${OPENGL_LIBRARIES})
  FLTK_WRAP_UI( IcpRegisterSurfaceGui ${IcpRegisterSurface_GUI_SRCS} )
ENDIF(FLTK_FOUND)
 
 



ADD_EXECUTABLE(IcpRegisterSurface RegisterMesh.cxx )
ADD_EXECUTABLE(IcpRegisterSurfaceGui ${IcpRegisterSurfaceGui_FLTK_UI_SRCS}
                                     MainFunction.cxx 
				     ICPregister.cxx 
                                     ICPregister.h )


TARGET_LINK_LIBRARIES(IcpRegisterSurface vtkIO vtkHybrid vtkFiltering vtkCommon)
TARGET_LINK_LIBRARIES(IcpRegisterSurfaceGui vtkIO vtkHybrid vtkFiltering vtkCommon)


INSTALL(TARGETS IcpRegisterSurface IcpRegisterSurfaceGui RUNTIME DESTINATION bin)
