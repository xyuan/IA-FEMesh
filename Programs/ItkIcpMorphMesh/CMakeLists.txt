PROJECT(ItkIcpMorphMesh)

  
INCLUDE_DIRECTORIES( ${ItkIcpMorphMesh_SOURCE_DIR} ${ItkIcpMorphMesh_BINARY_DIR} )


 
ADD_EXECUTABLE(ItkIcpMorphMesh ItkIcpMorphMesh.cxx )

#LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
#LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(ItkIcpMorphMesh ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl itkzlib itksys vtkIO vtkHybrid vtkFiltering vtkCommon)


INSTALL(TARGETS ItkIcpMorphMesh RUNTIME DESTINATION bin)
