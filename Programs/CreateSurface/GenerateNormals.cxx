#include "GenerateNormals.h"

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkPolyDataNormals.h>
#include <vtkXMLPolyDataReader.h>



#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

//  Register two surface meshes using the Iterative Closest Point Algorithm


int GenerateNormals( const char *inputSurfaceFile, const char *outputSurfaceFile )
{

  std::cout << "Input Surface : " << inputSurfaceFile << std::endl;
  std::cout << "Output Surface: " << outputSurfaceFile << std::endl;
	

   /*** Read the Meshes to Register ***/
  vtkPolyData *originalPolyData;

  vtkSTLReader *fixedReader = vtkSTLReader::New();
  if ((strstr(inputSurfaceFile, ".stl") != NULL) || (strstr(inputSurfaceFile, ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName( inputSurfaceFile );
    fixedReader->Update( );
    originalPolyData = fixedReader->GetOutput();
    }
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName(inputSurfaceFile);
    polyReader->Update( );	
    originalPolyData = polyReader->GetOutput();
    }
     
  std::cout << "Comouting Normals ... " << std::endl;
  vtkPolyDataNormals *normalFilter = vtkPolyDataNormals::New();
  normalFilter->SetInput( originalPolyData );
  normalFilter->ComputeCellNormalsOn( );
  normalFilter->Update( );
  vtkPolyData *finalPolyData = normalFilter->GetOutput();

  std::cout << "Writing Surface: " << outputSurfaceFile << " ..." << std::endl;	
  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
  meshWriter->SetFileName( outputSurfaceFile );
  meshWriter->SetFileTypeToASCII();
  meshWriter->SetInput( finalPolyData );
  meshWriter->Update( );


  return 0;
}
