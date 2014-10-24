PROJECT(MIMXCommon)

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter )

SET ( COMMON_SRCS 
    mimxRegisterVersorRigidFilter.cxx  
    mimxRegisterBSplineFilter.cxx 
    mimxTransformIO.cxx 
    mimxCheckTypeOfFile.cxx
    vtkContainer.cxx
    vtkLinkedListWrapper.cxx
    vtkMimxCurve.cxx
    vtkMimxSTLSource.cxx
    vtkMimxUGrid.cxx
    vtkMimxBoundingBoxSource.cxx 
    vtkLocalAxesActor.cxx
    vtkPVAxesActor.cxx  
    vtkMimxSolidStructuredGridSource.cxx
    vtkMimxPlanarStructuredGridSource.cxx
    vtkMeshQualityClass.cxx
    vtkMimxApplyNodeElementNumbers.cxx
    vtkMimxCreateIntervertebralDiscElementSets.cxx
    vtkMimxMergeNodesPointAndCellData.cxx
    vtkMimxMeshSeed.cxx
    vtkMimxActorBase.cxx
    vtkMimxSurfacePolyDataActor.cxx
    vtkMimxUnstructuredGridActor.cxx
    vtkMimxErrorCallback.cxx
    vtkMimxImageActor.cxx
#    vtkMimxInteractorStyleUser.cxx
    vtkLinkedListWrapperTree.cxx  
    vtkMimxColorCodeMeshSeedActor.cxx
    vtkMimxMeshActor.cxx
    vtkMimxEntryValueChangedCallback.cxx
    vtkMimxBoundaryConditionActor.cxx
)

ADD_LIBRARY( mimxCommon ${COMMON_SRCS} )

INSTALL_TARGETS(/lib/IaFeMesh mimxCommon)
INSTALL_FILES(/include/IaFeMesh/Common "(\\.h|\\.txx)$")


