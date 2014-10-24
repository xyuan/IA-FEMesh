PROJECT(FERegistrationModule)

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
INCLUDE_DIRECTORIES( ${DeformableFeRegistrationModule_SOURCE_DIR}    ${DeformableFeRegistrationModule_BINARY_DIR} )

SET ( DeformableFeRegistration_SOURCE DeformableFeRegistration.cxx)

GENERATECLP( DeformableFeRegistration_SOURCE DeformableFeRegistration.xml ${FERegistrationModule_SOURCE_DIR}/Resources/mimxLogo.h)

ADD_EXECUTABLE ( FERegistrationModule ${DeformableFeRegistration_SOURCE} )
TARGET_LINK_LIBRARIES ( FERegistrationModule 
                          mimxFilter
                          mimxIO
                          iamAdapter
                          ITKCommon 
                          ITKBasicFilters 
                          ITKIO
			  ITKFEM
			  ITKNumerics 
                          itkvnl_algo
                          itkvnl
                          vtkIO 
                          vtkHybrid
			  vtkGraphics 
                          vtkFiltering 
                          vtkCommon )


SET ( MappedMeshModule_SOURCE MappedMeshModule.cxx)

GENERATECLP( MappedMeshModule_SOURCE MappedMeshModule.xml
${FERegistrationModule_SOURCE_DIR}/Resources/mimxLogo.h)

ADD_EXECUTABLE ( MappedMeshModule ${MappedMeshModule_SOURCE} )
TARGET_LINK_LIBRARIES ( MappedMeshModule
                          mimxFilter
                          mimxIO
                          iamAdapter
                          ITKCommon
                          ITKBasicFilters
                          ITKIO
                          ITKFEM
                          ITKNumerics
                          itkvnl_algo
                          itkvnl
                          vtkIO
                          vtkHybrid
                          vtkGraphics
                          vtkFiltering
                          vtkCommon )

INSTALL(TARGETS FERegistrationModule RUNTIME DESTINATION bin)
INSTALL(TARGETS MappedMeshModule RUNTIME DESTINATION bin)
