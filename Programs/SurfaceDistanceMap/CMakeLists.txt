PROJECT( SurfaceDistanceMap )

  
INCLUDE_DIRECTORIES( ${DistanceMap_SOURCE_DIR} ${DistanceMap_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter )


 
#ADD_EXECUTABLE(DistanceMap DistanceMap.cxx )
#ADD_EXECUTABLE(SurfaceToImage SurfaceToImage.cxx )
#ADD_EXECUTABLE(SurfaceToBinaryImage SurfaceToBinaryImage.cxx )
ADD_EXECUTABLE(SurfaceDistanceMap SurfaceDistanceMap.cxx )

LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )

#TARGET_LINK_LIBRARIES(DistanceMap vtkIO vtkHybrid vtkFiltering vtkCommon)
#TARGET_LINK_LIBRARIES(SurfaceToImage 
#                       iamAdapter vtkIO vtkHybrid vtkFiltering vtkCommon 
#                       ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics 
#                       ITKStatistics itkvnl_algo itkvnl)
#TARGET_LINK_LIBRARIES(SurfaceToBinaryImage 
#                       iamAdapter vtkIO vtkHybrid vtkFiltering vtkCommon 
#                      ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics 
#                      ITKStatistics itkvnl_algo itkvnl)
TARGET_LINK_LIBRARIES(SurfaceDistanceMap 
                       iamAdapter 
                       mimxFilter 
		       vtkIO 
                       vtkHybrid 
                       vtkFiltering 
                       vtkCommon 
                       ITKAlgorithms 
                       ITKIO 
                       ITKBasicFilters 
                       ITKCommon 
                       ITKNumerics 
                       ITKStatistics 
                       itkvnl_algo 
                       itkvnl)

INSTALL(TARGETS SurfaceDistanceMap RUNTIME DESTINATION bin)
