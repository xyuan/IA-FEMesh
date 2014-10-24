PROJECT(DiffeomorphicDemonsRegistration)



LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )

ADD_EXECUTABLE(mimxDiffeomorphicRegistration DemonsRegistration.cxx) 


