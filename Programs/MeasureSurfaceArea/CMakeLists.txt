PROJECT(MeasureSurfaceArea)

  
INCLUDE_DIRECTORIES( ${MeasureSurfaceArea_SOURCE_DIR} ${MeasureSurfaceArea_BINARY_DIR} )


 
ADD_EXECUTABLE(MeasureSurfaceArea MeasureSurfaceArea.cxx )

LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(MeasureSurfaceArea vtkIO vtkHybrid vtkFiltering vtkCommon)

INSTALL(TARGETS MeasureSurfaceArea RUNTIME DESTINATION bin)
