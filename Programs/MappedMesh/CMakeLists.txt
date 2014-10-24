PROJECT(MappedMesh)

  
INCLUDE_DIRECTORIES( ${MappedMesh_SOURCE_DIR} ${MappedMesh_BINARY_DIR} )


 
ADD_EXECUTABLE(ICPInitMappedMesh ICPInitMappedMesh.cxx )
ADD_EXECUTABLE(ExtractMaskRegion ExtractMaskRegion.cxx )
ADD_EXECUTABLE(AffineRegisterRegions AffineRegisterRegions.cxx )
ADD_EXECUTABLE(MappedMesh MappedMesh.cxx )
ADD_EXECUTABLE(testTriLineIntersect testTriLineIntersect.cxx TriLineIntersect.cxx)
ADD_EXECUTABLE(ProjectMappedMesh ProjectMappedMesh.cxx TriLineIntersect.cxx SimulatedAnnealing.cxx)
ADD_EXECUTABLE(ProjectMappedMeshNew ProjectMappedMeshNew.cxx TriLineIntersect.cxx SimulatedAnnealing.cxx CheckHexahedronTopology.cxx)
ADD_EXECUTABLE(testHex testHex.cxx CheckHexahedronTopology.cxx )

LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(ICPInitMappedMesh vtkIO vtkHybrid vtkFiltering vtkCommon)
TARGET_LINK_LIBRARIES(ExtractMaskRegion ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl)
TARGET_LINK_LIBRARIES(AffineRegisterRegions ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl)
TARGET_LINK_LIBRARIES(MappedMesh vtkIO vtkHybrid vtkFiltering vtkCommon)
TARGET_LINK_LIBRARIES(testTriLineIntersect vtkIO vtkHybrid vtkFiltering vtkCommon ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl)
TARGET_LINK_LIBRARIES(ProjectMappedMesh vtkIO vtkHybrid vtkFiltering vtkCommon ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl)
TARGET_LINK_LIBRARIES(testHex vtkIO vtkHybrid vtkFiltering vtkCommon )
TARGET_LINK_LIBRARIES(ProjectMappedMeshNew vtkIO vtkHybrid vtkFiltering vtkCommon ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl)


INSTALL(TARGETS ICPInitMappedMesh
        ExtractMaskRegion
        AffineRegisterRegions
        MappedMesh
        testTriLineIntersect
        ProjectMappedMesh
        ProjectMappedMeshNew
        testHex RUNTIME DESTINATION bin)
