PROJECT(ScaleSTL)



ADD_EXECUTABLE( ScaleSTL ScaleSTL.cxx)


TARGET_LINK_LIBRARIES( ScaleSTL vtkHybrid vtkRendering  vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon vtkWidgets ${OPENGL_glu_LIBRARY} )


INSTALL(TARGETS ScaleSTL RUNTIME DESTINATION bin)


