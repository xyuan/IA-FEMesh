PROJECT(ClosestPoint)

INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)

IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ADD_EXECUTABLE( ClosestPoint ClosestPoint.cxx vtkComputeNormalsFromPolydataFilter.cxx vtkAbaqusFileWriter.cxx)


TARGET_LINK_LIBRARIES(ClosestPoint vtkHybrid vtkRendering  vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon vtkWidgets ${OPENGL_glu_LIBRARY} ITKCommon ITKBasicFilters ITKIO )

INSTALL(TARGETS ClosestPoint RUNTIME DESTINATION bin)



