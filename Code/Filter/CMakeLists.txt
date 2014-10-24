PROJECT( MIMXFilter )

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )
INCLUDE_DIRECTORIES( ${VTK_SOURCE_DIR}/Utilities/verdict/include ${VTK_KITS_DIR}/verdict )

SET ( FILTER_SRCS 
    vtkMimxEdgeToStructuredGrid.cxx            
    vtkMimxPlanarTransfiniteInterpolation.cxx  
    vtkMimxSubdivideCurve.cxx
    vtkMimxBoundingBoxSurfaceMesh.cxx    
    vtkMimxEditUnstructuredHexahedronGrid.cxx  
    vtkMimxSmoothUnstructuredGridFilter.cxx
    vtkMimxClipImageFromBox.cxx          
    vtkMimxGenerateHexahedronMesh.cxx          
    vtkMimxSolidTransfiniteInterpolation.cxx
    vtkMimxCylindricalMesh.cxx
    vtkMimxEdgeToPlanarStructuredGrid.cxx
    vtkMimxStructuredPlanarToStructuredSolidGrid.cxx
    vtkMimxSolidTrilinearInterpolation.cxx
    vtkMimxAddUnstructuredHexahedronGridCell.cxx
    vtkMimxDeleteUnstructuredHexahedronGridCell.cxx
    vtkMimxMirrorUnstructuredHexahedronGridCell.cxx
    vtkMimxSplitUnstructuredHexahedronGridCell.cxx
    vtkMimxPatchSurfaceMesh.cxx
    vtkMimxUnstructuredGridFromFourPoints.cxx
    vtkMimxPolyDataSingleSourceShortestPath.cxx
    vtkMimxFourPointsToStructuredPlanarMesh.cxx
    vtkMimxBoundingBoxToStructuredGrids.cxx
    vtkMimxComputeNormalsFromPolydataFilter.cxx
    vtkMimxPlanarEllipticalInterpolation.cxx
    vtkMimxSolidEllipticalInterpolation.cxx
    vtkMimxPlanarTrilinearInterpolation.cxx
    vtkMimxSelectSurface.cxx
    vtkMimxExtrudePolyData.cxx
    vtkMimxAttachBoundingBoxMesh.cxx
    vtkMimxSetStructuredGridEdge.cxx
    vtkMimxExtractStructuredGridEdge.cxx
    vtkMimxMorphStructuredGrid.cxx
    vtkMimxSetStructuredGridFace.cxx
    vtkMimxExtractStructuredGridFace.cxx
    vtkMimxConstrainedSmoothPolyDataFilter.cxx
    vtkMimxRecalculateInteriorNodes.cxx
    vtkMimxUnstructuredGridFromBoundingBox.cxx
    vtkMimxButterflyMeshFromBounds.cxx
    vtkMimxGenerateIntervertebralDisc.cxx
    vtkMimxUnstructuredToStructuredGrid.cxx
    vtkMimxRebinMaterialProperty.cxx
    vtkMimxMeshSeed.cxx
    vtkMimxBoundingBoxFromBounds.cxx
    vtkMimxApplyImageBasedMaterialProperties.cxx
    vtkMimxExtractElementNumbersOfaFaceFilter.cxx
    vtkMimxSubdivideBoundingBox.cxx
    vtkMimxRecalculateInteriorNodesTPS.cxx
    vtkMimxBoundingBoxFromUnstructuredGrid.cxx
    vtkMimxMapOriginalCellAndPointIds.cxx
    vtkMimxEquivalancePoints.cxx
    vtkMeshQualityExtended.cxx
    vtkMimxMergePoints.cxx
    vtkFaceHedgeHog.cxx
    vtkMimxExtractSurface.cxx
    vtkMimxGeometryFilter.cxx
)


ADD_LIBRARY( mimxFilter ${FILTER_SRCS} )

INSTALL_TARGETS(/lib/IaFeMesh mimxFilter)
INSTALL_FILES(/include/IaFeMesh/Filter "(\\.h|\\.txx)$")

