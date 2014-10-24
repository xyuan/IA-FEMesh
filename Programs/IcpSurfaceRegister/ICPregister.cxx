#include "ICPregister.h"

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkPolyDataReader.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>


//  Register two surface meshes using the Iterative Closest Point Algorithm


int ICPregistration( const char *fixedSurface, 
					 const char *movingSurface, 
					 const char *registeredSurface, 
					 const int numberOfIterations,
					 const int maximumNumberPoints,
					 const float maximumDistance)
{

  
  /*** Read the Meshes to Register ***/
  vtkPolyData *fixedPolyData;
  if ((strstr(fixedSurface, ".stl") != NULL) || (strstr(fixedSurface, ".STL")!= NULL) )
    {
    vtkSTLReader *stlReader = vtkSTLReader::New();
    stlReader->SetFileName(fixedSurface);
	  stlReader->Update( );
	  fixedPolyData = stlReader->GetOutput();
	}
  else
    {
    vtkPolyDataReader *polyReader = vtkPolyDataReader::New();
    polyReader->SetFileName(fixedSurface);
	  polyReader->Update( );	
    fixedPolyData = polyReader->GetOutput();
    }

  vtkPolyData *movingPolyData;
  if ((strstr(fixedSurface, ".stl") != NULL) || (strstr(fixedSurface, ".STL")!= NULL) )
    {
    vtkSTLReader *stlReader = vtkSTLReader::New();
    stlReader->SetFileName(movingSurface);
	  stlReader->Update( );
	  movingPolyData = stlReader->GetOutput();
	}
  else
    {
    vtkPolyDataReader *polyReader = vtkPolyDataReader::New();
    polyReader->SetFileName(movingSurface);
	  polyReader->Update( );	
    movingPolyData = polyReader->GetOutput();
    }
	
	
  vtkIterativeClosestPointTransform *icpTransform = vtkIterativeClosestPointTransform::New();
  icpTransform->SetSource( movingPolyData );
	icpTransform->SetTarget( fixedPolyData );
	icpTransform->StartByMatchingCentroidsOn( );
	icpTransform->GetLandmarkTransform( )->SetModeToRigidBody( );

	icpTransform->SetMaximumNumberOfIterations( numberOfIterations );
	icpTransform->CheckMeanDistanceOn();
	icpTransform->SetMaximumMeanDistance( maximumDistance );
	icpTransform->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
  icpTransform->SetMaximumNumberOfLandmarks( maximumNumberPoints );
	icpTransform->Update( );


  vtkTransformPolyDataFilter *resampleMeshFilter = vtkTransformPolyDataFilter::New();
  resampleMeshFilter->SetInput( movingPolyData );
	resampleMeshFilter->SetTransform( icpTransform );
	resampleMeshFilter->Update( );


  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
  meshWriter->SetFileName( registeredSurface );
	meshWriter->SetFileTypeToASCII();
	meshWriter->SetInput( resampleMeshFilter->GetOutput( ) );
	meshWriter->Update( );


  return 0;
}
