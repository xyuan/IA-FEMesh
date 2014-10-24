INCLUDE_REGULAR_EXPRESSION("^.*$")

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Filter 
                     ${IAFEMMESH_BINARY_DIR}/Code/Filter 
                     ${IAFEMMESH_SOURCE_DIR}/Code/Mesh 
                     ${IAFEMMESH_BINARY_DIR}/Code/Mesh) 

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Common ${IAFEMMESH_BINARY_DIR}/Code/Common )

IF( NOT DISABLE_CXX_TESTING )
IF(ITK_BINARY_DIR)
  IF(TCL_TCLSH)
    ADD_TEST(PrintSelf-BasicFilters ${TCL_TCLSH}
             ${ITK_SOURCE_DIR}/Utilities/Dart/PrintSelfCheck.tcl  
             ${ITK_SOURCE_DIR}/Code/BasicFilters)
  ENDIF(TCL_TCLSH)
ENDIF(ITK_BINARY_DIR)


SET(MainAddUnstructuredHexahedronGridCellTest ${CXX_TEST_PATH}/MainAddUnstructuredHexahedronGridCellTest)
ADD_TEST(MainAddUnstructuredHexahedronGridCellTest ${EXECUTABLE_OUTPUT_PATH}/MainAddUnstructuredHexahedronGridCellTest ${IAFEMESH_DATA_ROOT}/BBox2.vtk)

#SET(MainDeleteUnstructuredHexahedronGridCellTest ${CXX_TEST_PATH}/MainDeleteUnstructuredHexahedronGridCellTest)
#ADD_TEST(MainDeleteUnstructuredHexahedronGridCellTest ${EXECUTABLE_OUTPUT_PATH}/MainDeleteUnstructuredHexahedronGridCellTest ${IAFEMESH_DATA_ROOT}/BBox6.vtk)

SET(MainMirrorUnstructuredHexahedronGridCellTest ${CXX_TEST_PATH}/MainMirrorUnstructuredHexahedronGridCellTest)
ADD_TEST(MainMirrorUnstructuredHexahedronGridCellTest ${EXECUTABLE_OUTPUT_PATH}/MainMirrorUnstructuredHexahedronGridCellTest ${IAFEMESH_DATA_ROOT}/BBox2.vtk)

SET(MainSplitUnstructuredHexahedronGridCellTest ${CXX_TEST_PATH}/MainSplitUnstructuredHexahedronGridCellTest)
ADD_TEST(MainSplitUnstructuredHexahedronGridCellTest ${EXECUTABLE_OUTPUT_PATH}/MainSplitUnstructuredHexahedronGridCellTest ${IAFEMESH_DATA_ROOT}/BBox2.vtk)

SET(MainPatchSurfaceMeshTest ${CXX_TEST_PATH}/MainPatchSurfaceMeshTest)
ADD_TEST(MainPatchSurfaceMeshTest ${EXECUTABLE_OUTPUT_PATH}/MainPatchSurfaceMeshTest ${IAFEMESH_DATA_ROOT}/surface2.vtk)

SET(MainRecalculateInteriorNodesTPSTest ${CXX_TEST_PATH}/MainRecalculateInteriorNodesTPSTest)
ADD_TEST(MainRecalculateInteriorNodesTPSTest ${EXECUTABLE_OUTPUT_PATH}/MainRecalculateInteriorNodesTPSTest ${IAFEMESH_DATA_ROOT}/bbox2_modified.vtk ${IAFEMESH_DATA_ROOT}/1P_4Box_Gaus05Hypercube.vtk)

SET(MainMapOriginalCellAndPointIdsTest ${CXX_TEST_PATH}/MainMapOriginalCellAndPointIdsTest)
ADD_TEST(MainMapOriginalCellAndPointIdsTest ${EXECUTABLE_OUTPUT_PATH}/MainMapOriginalCellAndPointIdsTest ${IAFEMESH_DATA_ROOT}/FullMesh.vtk ${IAFEMESH_DATA_ROOT}/PartialMesh.vtk)

#SET(vtkSolidTrilinearInterpolationTest ${CXX_TEST_PATH}/vtkSolidTrilinearInterpolationTest)
#ADD_TEST(vtkSolidTrilinearInterpolationTest ${EXECUTABLE_OUTPUT_PATH}/vtkSolidTrilinearInterpolationTest)


ADD_EXECUTABLE(MainAddUnstructuredHexahedronGridCellTest MainAddUnstructuredHexahedronGridCellTest.cxx)
TARGET_LINK_LIBRARIES(MainAddUnstructuredHexahedronGridCellTest mimxFilter mimxCommon 
                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

#ADD_EXECUTABLE(MainDeleteUnstructuredHexahedronGridCellTest MainDeleteUnstructuredHexahedronGridCellTest.cxx)
#TARGET_LINK_LIBRARIES(MainDeleteUnstructuredHexahedronGridCellTest mimxFilter mimxCommon 
#                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE(MainMirrorUnstructuredHexahedronGridCellTest MainMirrorUnstructuredHexahedronGridCellTest.cxx)
TARGET_LINK_LIBRARIES(MainMirrorUnstructuredHexahedronGridCellTest mimxFilter mimxCommon 
                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE(MainSplitUnstructuredHexahedronGridCellTest MainSplitUnstructuredHexahedronGridCellTest.cxx)
TARGET_LINK_LIBRARIES(MainSplitUnstructuredHexahedronGridCellTest mimxFilter mimxCommon 
                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

#ADD_EXECUTABLE(vtkSolidTrilinearInterpolationTest vtkSolidTrilinearInterpolationTest.cxx)
#TARGET_LINK_LIBRARIES(vtkSolidTrilinearInterpolationTest mimxFilter mimxCommon 
#                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE(MainPatchSurfaceMeshTest MainPatchSurfaceMeshTest.cxx)
TARGET_LINK_LIBRARIES(MainPatchSurfaceMeshTest mimxFilter mimxCommon 
                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE(vtkRebinMaterialPropertyTest vtkRebinMaterialPropertyTest.cxx)
TARGET_LINK_LIBRARIES(vtkRebinMaterialPropertyTest mimxFilter mimxCommon 
                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE(MainRecalculateInteriorNodesTPSTest MainRecalculateInteriorNodesTPSTest.cxx)
TARGET_LINK_LIBRARIES(MainRecalculateInteriorNodesTPSTest mimxFilter mimxCommon 
                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE(MainMapOriginalCellAndPointIdsTest MainMapOriginalCellAndPointIdsTest.cxx)
TARGET_LINK_LIBRARIES(MainMapOriginalCellAndPointIdsTest mimxFilter mimxCommon 
                      vtkHybrid vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon )

ADD_TEST(vtkRebinMaterialPropertyTest ${EXECUTABLE_OUTPUT_PATH}/vtkRebinMaterialPropertyTest)

ENDIF( NOT DISABLE_CXX_TESTING )
