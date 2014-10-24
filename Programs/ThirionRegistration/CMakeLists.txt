PROJECT(ThirionRegistration)

  
INCLUDE_DIRECTORIES( ${ThirionRegistration_SOURCE_DIR} 
                     ${ThirionRegistration_BINARY_DIR} )


LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )
 
ADD_EXECUTABLE( mimxThirionRegistration mimxThirionRegistration.cxx
                mimxThirionRegistrationPrimary.cxx )

ADD_EXECUTABLE( mimxWarpImage mimxWarpImage.cxx )

ADD_EXECUTABLE( mimxCreateLandmarkTransform mimxCreateLandmarkTransform.cxx )
