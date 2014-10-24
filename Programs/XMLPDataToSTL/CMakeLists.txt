PROJECT(XMLPDataToSTL)


ADD_EXECUTABLE( XMLPDataToSTL XMLPDataToSTL.cxx)


TARGET_LINK_LIBRARIES( XMLPDataToSTL vtkHybrid vtkRendering  vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon vtkWidgets ${OPENGL_glu_LIBRARY} )


INSTALL(TARGETS XMLPDataToSTL RUNTIME DESTINATION bin)

