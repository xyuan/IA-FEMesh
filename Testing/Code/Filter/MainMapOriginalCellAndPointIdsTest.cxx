#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkMimxMapOriginalCellAndPointIds.h"

int main(int argc, char * argv [])
{
	//if( argc < 3 )
	//{
	//	std::cerr << "vtkMimxMapOriginalCellAndPointIds "
	//		<< " InputVtkUnstructuredGrid"
	//		<< std::endl;
	//	return EXIT_FAILURE;
	//}
	vtkUnstructuredGridReader *reader1 = vtkUnstructuredGridReader::New();
	//reader1->SetFileName(argv[1]);
	reader1->SetFileName("D:/OpenSource/IAFEMESH/IaFeMesh/Testing/Data/FullMesh.vtk");
	reader1->Update();
	vtkUnstructuredGridReader *reader2 = vtkUnstructuredGridReader::New();
	//reader2->SetFileName(argv[2]);
	reader2->SetFileName("D:/OpenSource/IAFEMESH/IaFeMesh/Testing/Data/PartialMesh.vtk");
	reader2->Update();
	
	vtkMimxMapOriginalCellAndPointIds *MapOriginalCellAndPointIds = 
		vtkMimxMapOriginalCellAndPointIds::New();
	MapOriginalCellAndPointIds->SetCompleteMesh(reader1->GetOutput());
	MapOriginalCellAndPointIds->SetPartialMesh(reader2->GetOutput());
	MapOriginalCellAndPointIds->Update();

	vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
	writer->SetInput(reader2->GetOutput());
	writer->SetFileName("PartialMeshWithIds.vtk");

	writer->Write();
	reader1->Delete();
	reader2->Delete();
	MapOriginalCellAndPointIds->Delete();
	writer->Delete();
}

