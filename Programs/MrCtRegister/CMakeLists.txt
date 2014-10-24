PROJECT(MrCtRegister)
 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common )


ADD_EXECUTABLE(MrCtRigidRegister MrCtRigidRegister.cxx )
ADD_EXECUTABLE(MrCtBsplineRegister MrCtBsplineRegister.cxx  )
ADD_EXECUTABLE(iaFemResampleImage iaFemResampleImage.cxx  )


TARGET_LINK_LIBRARIES(MrCtRigidRegister iamCommon )
TARGET_LINK_LIBRARIES(MrCtBsplineRegister iamCommon )
TARGET_LINK_LIBRARIES(iaFemResampleImage iamCommon )

INSTALL(TARGETS MrCtRigidRegister
        MrCtBsplineRegister
        iaFemResampleImage  RUNTIME DESTINATION bin)
