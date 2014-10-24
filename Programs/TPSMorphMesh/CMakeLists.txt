PROJECT(TPSMorphMesh)

  
INCLUDE_DIRECTORIES( ${TPSMorphMesh_SOURCE_DIR} ${TPSMorphMesh_BINARY_DIR} )


 
ADD_EXECUTABLE(TPSMorphMesh TPSMorphMesh.cxx )

LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(TPSMorphMesh vtkIO vtkHybrid vtkFiltering vtkCommon)


INSTALL(TARGETS TPSMorphMesh RUNTIME DESTINATION bin)
