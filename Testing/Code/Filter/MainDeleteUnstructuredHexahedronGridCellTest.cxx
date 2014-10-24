/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: MainDeleteUnstructuredHexahedronGridCellTest.cxx,v $
Language:  C++
Date:      $Date: 2007/05/17 17:09:46 $
Version:   $Revision: 1.3 $

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

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "vtkMimxDeleteUnstructuredHexahedronGridCell.h"

#include "vtkDataSetMapper.h"
#include "vtkIdList.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"

int main(int argc, char * argv [])
{
  if( argc < 2 )
    {
    std::cerr << "MainDeleteUnstructuredHexahedronGridCellTest "
              << " InputVtkUnstructuredGrid"
              << std::endl;
    return EXIT_FAILURE;
    }


  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName( argv[1] );
  vtkMimxDeleteUnstructuredHexahedronGridCell *Delete1 = vtkMimxDeleteUnstructuredHexahedronGridCell::New();
  Delete1->SetInput(reader->GetOutput());
  Delete1->SetCellNum(6);
  Delete1->Update( );
	std::cout << "Test 1 - Number of Cells: " << Delete1->GetOutput()->GetNumberOfCells() << std::endl;
  if ( Delete1->GetOutput()->GetNumberOfCells() != 6 )
    {
    std::cout << "Error Test 1 - Invalid number of cells - Delete Cell 6" << std::endl;
    return EXIT_FAILURE;
    }

  vtkMimxDeleteUnstructuredHexahedronGridCell *Delete2 = 
    vtkMimxDeleteUnstructuredHexahedronGridCell::New();
  Delete2->SetInput(Delete1->GetOutput());
  Delete2->SetCellNum(3);
  Delete2->Update( );
  std::cout << "Test 2 - Number of Cells: " << Delete2->GetOutput()->GetNumberOfCells() << std::endl;
  if ( Delete2->GetOutput()->GetNumberOfCells() != 5 )
    {
    std::cout << "Error Test 2 - Invalid number of cells - Delete Cell 3" << std::endl;
    return EXIT_FAILURE;
    }


  /* Turn off the Graphical User Interface testing
  *************************************************
	vtkRenderer* ren = vtkRenderer::New();
	vtkDataSetMapper *mapper = vtkDataSetMapper::New();
	mapper->SetInput(Delete1->GetOutput());
	vtkActor *actor = vtkActor::New();
	actor->SetMapper(mapper);
	ren->AddActor(actor);
	vtkRenderWindow* renwin = vtkRenderWindow::New();
	renwin->AddRenderer(ren);
	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renwin);
	renwin->Render();
	iren->Start();
	reader->Delete();
	Delete1->Delete();
	actor->Delete();
	mapper->Delete();
	ren->Delete();
	renwin->Delete();
	iren->Delete();
  *************************************************/

  return EXIT_SUCCESS;
}
