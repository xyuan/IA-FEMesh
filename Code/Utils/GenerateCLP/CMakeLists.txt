PROJECT(GenerateCLP)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#FIND_PACKAGE(ModuleDescriptionParser REQUIRED)
SET(TCLAP_DIR ${IAFEMMESH_BINARY_DIR}/Code/Utils/tclap)
SET(ModuleDescriptionParser_DIR ${IAFEMMESH_BINARY_DIR}/Code/Utils/ModuleDescriptionParser)
LINK_DIRECTORIES(${IAFEMMESH_BINARY_DIR}/Code/Utils/ModuleDescriptionParser)


#INCLUDE(${ITK_USE_FILE})
INCLUDE(UseGenerateCLP.cmake)

CONFIGURE_FILE(
  ${GenerateCLP_SOURCE_DIR}/GenerateCLPConfig.cmake.in 
  ${GenerateCLP_BINARY_DIR}/GenerateCLPConfig.cmake 
)

SET (GENERATECLP_SOURCE GenerateCLP.cxx)
#LINK_DIRECTORIES(${ModuleDescriptionParser_LIB_DIR})
ADD_EXECUTABLE ( GenerateCLP ${GENERATECLP_SOURCE})
TARGET_LINK_LIBRARIES ( GenerateCLP
  ModuleDescriptionParser
  itksys
  ITKEXPAT)

IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)

