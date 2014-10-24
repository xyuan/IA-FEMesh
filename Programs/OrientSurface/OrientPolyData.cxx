#include "OrientPolyData.h"

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>


#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

//  Register two surface meshes using the Iterative Closest Point Algorithm


int OrientPolyData( const char *fixedSurface, 
		    const char *registeredSurface, 
		    const int Xaxis, const int Yaxis, const int Zaxis,
		    const int flipX, const int flipY, const int flipZ)
{

	std::cout << "Surface: " << fixedSurface << std::endl;
    std::cout << "Result: " << registeredSurface << std::endl;

	std::cout << "X Axis: " << Xaxis << std::endl;
	std::cout << "Y Axis: " << Yaxis << std::endl;
	std::cout << "Z Axis: " << Zaxis << std::endl;

	std::cout << "X Flip: " << flipX << std::endl;
	std::cout << "Y Flip: " << flipY << std::endl;
	std::cout << "Z Flip: " << flipZ << std::endl;
  
  /*** Read the Meshes to Register ***/
  vtkPolyData *originalPolyData;

  vtkSTLReader *fixedReader = vtkSTLReader::New();
  if ((strstr(fixedSurface, ".stl") != NULL) || (strstr(fixedSurface, ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(fixedSurface);
	fixedReader->Update( );
	originalPolyData = fixedReader->GetOutput();
	}
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName(fixedSurface);
	polyReader->Update( );	
    originalPolyData = polyReader->GetOutput();
    }
  //vtkPolyData *originalPolyData = fixedReader->GetOutput();
  
  vtkFloatingPointType center[3];
  originalPolyData->GetCenter( &(center[0]) );
  std::cout << center[0] << " " << center[1] << " " << center[2] << std::endl;

  vtkTransform *centerTransform = vtkTransform::New();
    centerTransform->Translate(-center[0],-center[1],-center[2]);
	
  vtkTransformPolyDataFilter *resampleMeshFilter = vtkTransformPolyDataFilter::New();
    resampleMeshFilter->SetInput( originalPolyData );
    resampleMeshFilter->SetTransform( centerTransform );
	resampleMeshFilter->Update();


  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->Identity( );
	matrix->SetElement(0, 0, 0);
	matrix->SetElement(0, Xaxis, 1);
	matrix->SetElement(1, 1, 0);
	matrix->SetElement(1, Yaxis, 1);
	matrix->SetElement(2, 2, 0);
	matrix->SetElement(2, Zaxis, 1);
	//matrix->Print(std::cout);

  vtkTransform *swapAxisTransform = vtkTransform::New();
    swapAxisTransform->SetMatrix( matrix );
	
  vtkTransformPolyDataFilter *swapAxisMeshFilter = vtkTransformPolyDataFilter::New();
    swapAxisMeshFilter->SetInput( resampleMeshFilter->GetOutput( ) );
    swapAxisMeshFilter->SetTransform( swapAxisTransform );
	swapAxisMeshFilter->Update();

  vtkTransform *flipTransform = vtkTransform::New();
    matrix->Identity( );
	matrix->SetElement(0, 0, (double) flipX);
	matrix->SetElement(1, 1, (double) flipY);
	matrix->SetElement(2, 2, (double) flipZ);	
	//matrix->Print(std::cout);
	flipTransform->SetMatrix( matrix );
		
  vtkTransformPolyDataFilter *flipMeshFilter = vtkTransformPolyDataFilter::New();
    flipMeshFilter->SetInput( swapAxisMeshFilter->GetOutput( ) );
    flipMeshFilter->SetTransform( flipTransform );
	flipMeshFilter->Update();
	
  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
    meshWriter->SetFileName(registeredSurface);
	meshWriter->SetFileTypeToASCII();
	meshWriter->SetInput( flipMeshFilter->GetOutput( ) );
	meshWriter->Update( );


  return 0;
}
