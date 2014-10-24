PROJECT ( MIMXIO )

SET ( IO_SRCS 
    vtkMimxAbaqusFileWriter.cxx
    vtkMimxIntervertebralDiscAbaqusWriter.cxx
    vtkPluginFilterWatcher.cxx
    ModuleProcessInformation.cxx
)

ADD_LIBRARY( mimxIO ${IO_SRCS} )
INSTALL_TARGETS(/lib/IaFeMesh mimxIO)
INSTALL_FILES(/include/IaFeMesh/IO "(\\.h|\\.txx)$")
