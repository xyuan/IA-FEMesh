vtkImageImport *importImage = vtkImageImport::New();
    importImage->SetDataScalarTypeToShort();
   	importImage->SetDataExtent(0,9,0,9,0,9);
	importImage->SetDataSpacing(1.0,1.0,1.0);
	importImage->SetImportVoidPointer( (void *)(image->GetBufferPointer()));
	
  typedef itk::Mesh< double > MeshType;  
  typedef itk::ImageToHexahedronMeshSource<ImageType, 
											MeshType> ImageToMeshSourceType;
  
  ImageToMeshSourceType::Pointer meshSource = ImageToMeshSourceType::New();

  meshSource->SetInput( image );
  meshSource->Update( );
  std::cout << "\t Create Mesh" << std::endl;
  MeshType::Pointer myMesh = meshSource->GetOutput( );

  // Transform to VTK and write out
  typedef itkMeshTovtkUnstructuredGrid MeshToVtkType;
  MeshToVtkType *toVtkFilter = new MeshToVtkType;
  
  toVtkFilter->SetInput(myMesh);
  
  vtkUnstructuredGrid *grid = toVtkFilter->GetOutput();
  
  vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
  writer->SetInput( grid );
  writer->SetFileName( "/tmp/testItkMeshToVtkUgrid.vtk" );
  writer->SetFileTypeToASCII( );
  writer->Update();
  
  typedef itk::AutomaticTopologyMeshSource<MeshType> AutoMeshType;
  AutoMeshType::Pointer tetSource = AutoMeshType::New();
		  
  tetSource->AddTetrahedron (   	
		  tetSource->AddPoint(0.0, 0.0, 0.0),
		  tetSource->AddPoint(1.0, 0.0, 0.0),
		  tetSource->AddPoint(1.0, 1.0, 0.0),
		  tetSource->AddPoint(1.0, 1.0, 1.0));
  
  std::cout << "\t Create Tet Mesh" << std::endl;
  
  MeshToVtkType *tetToVtkFilter = new MeshToVtkType;
  tetToVtkFilter->SetInput(tetSource->GetOutput());
  
  std::cout << "\t Convert to VTK" << std::endl;
  
  vtkUnstructuredGrid *tetGrid = tetToVtkFilter->GetOutput();
  //tetGrid->Print(std::cout);
  
  vtkUnstructuredGridWriter *tetWriter = vtkUnstructuredGridWriter::New();
  
  tetWriter->SetInput( tetGrid );
  tetWriter->SetFileName( "/tmp/testItkMeshToVtkUgrid-Tet.vtk" );
  tetWriter->SetFileTypeToASCII( );
  tetWriter->Update();
  
