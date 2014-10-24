PROJECT(vtk2itk2fem)

# Find ITK
#INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
#IF (USE_ITK_FILE)
#  INCLUDE(${USE_ITK_FILE})
#ENDIF(USE_ITK_FILE)

# Find VTK
#INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
#IF (USE_VTK_FILE)
#  INCLUDE(${USE_VTK_FILE})
#ENDIF (USE_VTK_FILE)

ADD_EXECUTABLE(vtk2itk2fem vtk2itk2fem.cxx)
TARGET_LINK_LIBRARIES(vtk2itk2fem ITKAlgorithms ITKIO ITKBasicFilters ITKFEM ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl vtkRendering vtkIO)


INSTALL(TARGETS vtk2itk2fem RUNTIME DESTINATION bin)
