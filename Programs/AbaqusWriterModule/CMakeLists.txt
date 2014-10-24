PROJECT(AbaqusWriterModule)

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
INCLUDE_DIRECTORIES( ${AbaqusWriterModule_SOURCE_DIR}     ${AbaqusWriterModule_BINARY_DIR} )

SET ( AbaqusWriterModule_SOURCE AbaqusWriterModule.cxx)

GENERATECLP( AbaqusWriterModule_SOURCE AbaqusWriterModule.xml ${AbaqusWriterModule_SOURCE_DIR}/Resources/mimxLogo.h)

ADD_EXECUTABLE ( AbaqusWriterModule ${AbaqusWriterModule_SOURCE} )
TARGET_LINK_LIBRARIES ( AbaqusWriterModule 
                          mimxFilter
                          mimxIO
                          vtkIO 
                          vtkHybrid
                          vtkGraphics 
                          vtkFiltering 
                          vtkCommon
                          ITKCommon 
                          ITKBasicFilters 
                          ITKIO  )

SET ( AssignMeshMaterialPropertiesModule_SOURCE AssignMeshMaterialPropertiesModule.cxx)

GENERATECLP( AssignMeshMaterialPropertiesModule_SOURCE  AssignMeshMaterialPropertiesModule.xml ${AbaqusWriterModule_SOURCE_DIR}/Resources/mimxLogo.h)

ADD_EXECUTABLE ( AssignMeshMaterialPropertiesModule ${AssignMeshMaterialPropertiesModule_SOURCE} )
TARGET_LINK_LIBRARIES ( AssignMeshMaterialPropertiesModule 
                          mimxFilter
                          mimxIO
                          vtkIO 
                          vtkHybrid
                          vtkGraphics 
                          vtkFiltering 
                          vtkCommon )



INSTALL(TARGETS AbaqusWriterModule AssignMeshMaterialPropertiesModule RUNTIME DESTINATION bin)

