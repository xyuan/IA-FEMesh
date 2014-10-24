PROJECT(VoxelMeshModule)

SET(TCLAP_DIR ${IAFEMMESH_BINARY_DIR}/Code/Utils/tclap)
SET(ModuleDescriptionParser_DIR ${IAFEMMESH_BINARY_DIR}/Code/Utils/ModuleDescriptionParser)
SET(GenerateCLP_DIR ${IAFEMMESH_BINARY_DIR}/Code/Utils/GenerateCLP)

INCLUDE(${GenerateCLP_SOURCE_DIR}/UseGenerateCLP.cmake)

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common  ${IAFEMMESH_BINARY_DIR}/Code/Common ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter  ${IAFEMMESH_BINARY_DIR}/Code/Filter ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets ) 
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/IO      ${IAFEMMESH_BINARY_DIR}/Code/IO )  
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Mesh    ${IAFEMMESH_BINARY_DIR}/Code/Mesh )  

SET ( VoxelMesh_SOURCE VoxelMesh.cxx)

GENERATECLP( VoxelMesh_SOURCE VoxelMesh.xml ${VoxelMeshModule_SOURCE_DIR}/Resources/mimxLogo.h)

ADD_EXECUTABLE ( VoxelMeshModule ${VoxelMesh_SOURCE} )
TARGET_LINK_LIBRARIES ( VoxelMeshModule 
                          mimxFilter
                          mimxIO
                          ITKCommon 
                          ITKBasicFilters 
                          ITKIO 
                          vtkIO 
                          vtkHybrid 
                          vtkFiltering 
                          vtkCommon )


INSTALL(TARGETS VoxelMeshModule RUNTIME DESTINATION bin)
