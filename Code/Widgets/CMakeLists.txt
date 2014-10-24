PROJECT ( MIMXWidgets )

SET ( WIDGETS_SRCS 
    vtkMimxBoundingBoxWidget.cxx
    vtkMimxModPointWidget.cxx
    vtkMimxPolyDataWidget.cxx
    vtkMimxUnstructuredGridExtractWidget.cxx
    vtkMimxUnstructuredGridWidget.cxx
    vtkMimxExtractEdgeWidget.cxx
    vtkMimxExtractFaceWidget.cxx
    vtkMimxExtractCellWidget.cxx
    vtkMimxPlaceLocalAxesWidget.cxx
    vtkMimxTraceContourWidget.cxx
    vtkMimxExtractMultipleFaceWidget.cxx
    vtkMimxSelectCellsWidget.cxx
    vtkMimxSelectPointsWidget.cxx
    vtkMimxSelectCellsWidgetFEMesh.cxx
    vtkMimxCreateElementSetWidgetFEMesh.cxx
    vtkMimxCreateBuildingBlockFromPickWidget.cxx
#    vtkMimxColorCodeMeshSeedWidget.cxx
    vtkMimxViewMergePointsWidget.cxx
)

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter )


ADD_LIBRARY( mimxWidgets ${WIDGETS_SRCS} )

INSTALL_TARGETS(/lib/IaFeMesh mimxWidgets)
INSTALL_FILES(/include/IaFeMesh/Widgets "(\\.h|\\.txx)$")

