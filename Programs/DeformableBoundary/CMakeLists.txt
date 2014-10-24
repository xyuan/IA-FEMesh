PROJECT(DeformableBoundary)

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )


ADD_EXECUTABLE(DeformableBoundary DeformableBoundary.cxx )
TARGET_LINK_LIBRARIES(DeformableBoundary iamAdapter ITKCommon ITKBasicFilters ITKIO vtkIO vtkHybrid vtkFiltering vtkCommon)

INSTALL(TARGETS DeformableBoundary RUNTIME DESTINATION bin)

