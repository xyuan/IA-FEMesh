PROJECT(CheckMeshQuality)

  
INCLUDE_DIRECTORIES( ${CheckMeshQuality_SOURCE_DIR} ${CheckMeshQuality_BINARY_DIR} )


 
ADD_EXECUTABLE(CheckMeshQuality CheckMeshQuality.cxx )

LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(CheckMeshQuality vtkIO vtkHybrid vtkFiltering vtkCommon)

INSTALL(TARGETS CheckMeshQuality RUNTIME DESTINATION bin)
