PROJECT(TestAdapter)

INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Mesh ${IAFEMMESH_BINARY_DIR}/Code/Mesh )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/IO ${IAFEMMESH_BINARY_DIR}/Code/IO )
INCLUDE_DIRECTORIES( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )

SET(TEMP ${TestAdapter_BINARY_DIR}/Temporary)
FILE(MAKE_DIRECTORY ${TEMP})

IF( NOT DISABLE_CXX_TESTING )

SET(TestMeshToGrid ${CXX_TEST_PATH}/TestMeshToGrid)
ADD_TEST(TestMeshToGrid ${EXECUTABLE_OUTPUT_PATH}/TestMeshToGrid ${TEMP})

SET(TestGridToMesh ${CXX_TEST_PATH}/TestGridToMesh)
ADD_TEST(TestGridToMesh ${EXECUTABLE_OUTPUT_PATH}/TestGridToMesh ${TEMP})

SET(TestMeshToPolyData ${CXX_TEST_PATH}/TestMeshToPolyData)
ADD_TEST(TestMeshToPolyData ${EXECUTABLE_OUTPUT_PATH}/TestMeshToPolyData)

SET(TestPolyDataToMesh ${CXX_TEST_PATH}/TestPolyDataToMesh)
ADD_TEST(TestPolyDataToMesh ${EXECUTABLE_OUTPUT_PATH}/TestPolyDataToMesh)

SET(TestImageToVtk ${CXX_TEST_PATH}/TestImageToVtk)
ADD_TEST(TestImageToVtk ${EXECUTABLE_OUTPUT_PATH}/TestImageToVtk)

ADD_EXECUTABLE(TestMeshToGrid TestMeshToGrid.cxx )
TARGET_LINK_LIBRARIES(TestMeshToGrid iamAdapter ITKCommon ITKBasicFilters ITKIO vtkIO vtkHybrid vtkFiltering vtkCommon)

ADD_EXECUTABLE(TestGridToMesh TestGridToMesh.cxx )
TARGET_LINK_LIBRARIES(TestGridToMesh iamAdapter ITKCommon ITKBasicFilters ITKIO itkvnl_algo itkvnl vtkIO vtkHybrid vtkFiltering vtkCommon)

ADD_EXECUTABLE(TestMeshToPolyData TestMeshToPolyData.cxx )
TARGET_LINK_LIBRARIES(TestMeshToPolyData iamAdapter ITKCommon ITKBasicFilters ITKIO itkvnl_algo itkvnl vtkIO vtkHybrid vtkFiltering vtkCommon)

ADD_EXECUTABLE(TestPolyDataToMesh TestPolyDataToMesh.cxx )
TARGET_LINK_LIBRARIES(TestPolyDataToMesh iamAdapter ITKCommon ITKBasicFilters ITKIO itkvnl_algo itkvnl vtkIO vtkHybrid vtkFiltering vtkCommon)

ADD_EXECUTABLE(TestImageToVtk TestImageToVtk.cxx )
TARGET_LINK_LIBRARIES(TestImageToVtk iamAdapter ITKCommon ITKBasicFilters ITKIO itkvnl_algo itkvnl vtkIO vtkHybrid vtkFiltering vtkCommon)


ENDIF( NOT DISABLE_CXX_TESTING )


