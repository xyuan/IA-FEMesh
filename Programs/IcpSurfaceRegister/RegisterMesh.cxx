/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: RegisterMesh.cxx,v $
  Language:  C++
  Date:      $Date: 2006/09/18 19:25:43 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif


#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>


//  Register two surface meshes using the Iterative Closest Point Algorithm


int main( int argc, char * argv[] )
{

  
  if( argc != 7 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  Fixed-STL-Surface  Moving-STL-Surface Output-STL-Mesh #-of-Iterations Max-Distance #-of-Landmarks" << std::endl;
    return EXIT_FAILURE;
    }

  
  /*** Read the Meshes to Register ***/
  vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(argv[1]);
	fixedReader->Update( );

  vtkSTLReader *movingReader = vtkSTLReader::New();
    movingReader->SetFileName(argv[2]);
	movingReader->Update( );
	
	
  vtkIterativeClosestPointTransform *icpTransform = vtkIterativeClosestPointTransform::New();
    icpTransform->SetSource( movingReader->GetOutput( ) );
	icpTransform->SetTarget( fixedReader->GetOutput( ) );
	icpTransform->StartByMatchingCentroidsOn( );
	icpTransform->GetLandmarkTransform( )->SetModeToRigidBody( );

	icpTransform->SetMaximumNumberOfIterations( atoi(argv[4]) );
	icpTransform->CheckMeanDistanceOn();
	icpTransform->SetMaximumMeanDistance( atof(argv[5]) );
	icpTransform->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
    icpTransform->SetMaximumNumberOfLandmarks( atoi(argv[6]) );
	icpTransform->Update( );


  vtkTransformPolyDataFilter *resampleMeshFilter = vtkTransformPolyDataFilter::New();
    resampleMeshFilter->SetInput( movingReader->GetOutput( ) );
	resampleMeshFilter->SetTransform( icpTransform );
	resampleMeshFilter->Update( );


  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
    meshWriter->SetFileName(argv[3]);
	meshWriter->SetFileTypeToASCII();
	meshWriter->SetInput( resampleMeshFilter->GetOutput( ) );
	//meshWriter->SetHeader("ICP Registration of Surface");
	meshWriter->Update( );


  return 0;
}

