PROJECT(ItkSegment)

  
INCLUDE_DIRECTORIES( ${ItkSegment_SOURCE_DIR} ${ItkSegment_BINARY_DIR} )


 
ADD_EXECUTABLE(ItkFastMarching FastMarchingImageFilter.cxx )

#LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
#LINK_LIBRARIES ( itkzlib itksys )

TARGET_LINK_LIBRARIES(ItkFastMarching ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl itkzlib itksys )

INSTALL(TARGETS ItkFastMarching RUNTIME DESTINATION bin)
