PROJECT(SmoothMesh)

  
INCLUDE_DIRECTORIES( ${SmoothMesh_SOURCE_DIR} ${SmoothMesh_BINARY_DIR} )


 
ADD_EXECUTABLE(SmoothMesh SmoothMesh.cxx )

#LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
#LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(SmoothMesh ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl itkzlib itksys vtkIO vtkHybrid vtkFiltering vtkCommon)

INSTALL(TARGETS SmoothMesh RUNTIME DESTINATION bin)
