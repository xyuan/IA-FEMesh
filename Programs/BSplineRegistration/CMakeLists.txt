PROJECT(MimxCoregister)

  
INCLUDE_DIRECTORIES( ${MimxCoregister_SOURCE_DIR} 
                     ${MimxCoregister_BINARY_DIR} )


LINK_LIBRARIES( ITKAlgorithms ITKIO ITKBasicFilters ITKCommon ITKNumerics ITKStatistics itkvnl_algo itkvnl )
LINK_LIBRARIES ( itkzlib itksys )
 
ADD_EXECUTABLE( mimxCoRegisterRigid mimxCoRegisterRigid.cxx )
ADD_EXECUTABLE( mimxCoRegisterBSpline mimxCoRegisterBSpline.cxx )
ADD_EXECUTABLE( mimxResampleImage mimxResampleImage.cxx )
ADD_EXECUTABLE( mimxResample8bitImage mimxResample8bitImage.cxx )
ADD_EXECUTABLE( mimxResampleFloatImage mimxResampleFloatImage.cxx )
