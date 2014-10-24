/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: MainPatchSurfaceMeshTest.cxx,v $
Language:  C++
Date:      $Date: 2007/05/17 17:09:46 $
Version:   $Revision: 1.6 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkMimxPatchSurfaceMesh.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkCellArray.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyDataReader.h"
#include "vtkDelaunay2D.h"
#include "vtkPolyData.h"

#include "vtkSTLWriter.h"

#include "vtkSTLWriter.h"

int main(int argc, char * argv [])
{
  if( argc < 2 )
  {
    std::cerr << "MainPatchSurfaceMeshTest "
              << " InputVtkUnstructuredGrid"
              << std::endl;
    return EXIT_FAILURE;
  }
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkMimxPatchSurfaceMesh *patch = vtkMimxPatchSurfaceMesh::New();
  patch->SetInput(reader->GetOutput());
  patch->Update();

  vtkSTLWriter *writer = vtkSTLWriter::New();
  writer->SetInput(patch->GetOutput());
  writer->SetFileName("Vert_Cut.stl");
  writer->Write();
  return EXIT_SUCCESS;
	

	/**************************************************************
	vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
	coneMapper->SetInput(patch->GetOutput());	
	vtkActor *coneActor = vtkActor::New();
	coneActor->SetMapper( coneMapper );
	vtkRenderer *ren1= vtkRenderer::New();
	ren1->AddActor( coneActor );
	ren1->SetBackground( 0.1, 0.2, 0.4 );
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer( ren1 );
	renWin->SetSize( 300, 300 );
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);
	renWin->Render();
	iren->Start();
	coneMapper->Delete();
	coneActor->Delete();
	ren1->Delete();
	renWin->Delete();
	iren->Delete();
	************************************************************************/
}
