PROJECT(DeformableFeRegistration)



ADD_EXECUTABLE( DeformableFeRegistration 
                DeformableRegistration.cxx
                vtkComputeNormalsFromPolydataFilter.cxx 
                vtkUnstructuredGridToitkMesh.cxx)


TARGET_LINK_LIBRARIES(DeformableFeRegistration ITKCommon ITKBasicFilters 
                      ITKIO ITKFEM ITKNumerics vtkIO vtkFiltering 
                      vtkCommon vtkGraphics)



ADD_EXECUTABLE( DeformImageFeRegistration
                DeformImageFeRegistration.cxx)


TARGET_LINK_LIBRARIES(DeformImageFeRegistration ITKCommon ITKBasicFilters 
                      ITKIO ITKFEM ITKNumerics)


INSTALL(TARGETS DeformImageFeRegistration RUNTIME DESTINATION bin)

