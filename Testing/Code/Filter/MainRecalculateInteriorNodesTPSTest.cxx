#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkMimxRecalculateInteriorNodesTPS.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyData.h"

int main(int argc, char * argv [])
{
	if( argc < 3 )
	{
		std::cerr << "vtkMimxAddUnstructuredHexahedronGridCell "
			<< " InputVtkUnstructuredGrid"
			<< std::endl;
		return EXIT_FAILURE;
	}
	vtkUnstructuredGridReader *reader1 = vtkUnstructuredGridReader::New();
	reader1->SetFileName(argv[1]);
	reader1->Update();
	vtkUnstructuredGridReader *reader2 = vtkUnstructuredGridReader::New();
	reader2->SetFileName(argv[2]);
	reader2->Update();
	
	vtkMimxRecalculateInteriorNodesTPS *RecalculateInteriorNodesTPS = 
		vtkMimxRecalculateInteriorNodesTPS::New();
	RecalculateInteriorNodesTPS->SetSource(reader1->GetOutput());
	RecalculateInteriorNodesTPS->SetTarget(reader2->GetOutput());
	RecalculateInteriorNodesTPS->Update();

	vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
	writer->SetInput(RecalculateInteriorNodesTPS->GetOutput());
	writer->SetFileName("bbox2tps.vtk");

	writer->Write();
	reader1->Delete();
	reader2->Delete();
	RecalculateInteriorNodesTPS->Delete();
	writer->Delete();
}

