SET ( ADAPTER_SRCS 
    vtkUnstructuredGridToitkMesh.cxx 
    itkMeshTovtkPolyData.cxx  
    vtkPolyDataToitkMesh.cxx
	)

ADD_LIBRARY( iamAdapter ${ADAPTER_SRCS} )

INSTALL_TARGETS(/lib/IaFeMesh iamAdapter)
INSTALL_FILES(/include/IaFeMesh/Adapter "(\\.h|\\.txx)$")

