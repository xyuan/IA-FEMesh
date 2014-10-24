/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: AbaqusWriterModule.cxx,v $
Language:  C++
Date:      $Date: 2008/04/09 18:49:39 $
Version:   $Revision: 1.2 $

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


#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>

#include "vtkMimxAbaqusFileWriter.h"
#include "AbaqusWriterModuleCLP.h"





int main( int argc, char **argv)
{

  PARSE_ARGS;
  
  bool debug = true;
  if ( debug )
    {
    // Summarize the command line arguements for the user
    std::cout << "Abaqus Writer Parameters" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "\tInput Mesh: " << vtkMeshFileName << std::endl;
    std::cout << "\tUser Name: " << userName << std::endl;
    std::cout << "\tDepartment Name: " << departmentName << std::endl;
    std::cout << "\tAddress 1: " << addressLine1 << std::endl;
    std::cout << "\tAddress 2: " << addressLine2 << std::endl;
    std::cout << "\tMaterial Property Name: " << materialPropertyName << std::endl;
    std::cout << "\tHeading Name: " << headingName << std::endl;
    std::cout << "\tAbaqus FileName: " << abaqusFileName << std::endl;
    std::cout << "\tAbaqus Node FileName: " << abaqusNodeElementFileName << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
  }
  
  
  // Load the initial (coarse) template mesh
  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SetFileName( vtkMeshFileName.c_str() );
  meshReader->Update();
  vtkUnstructuredGrid *inputMesh = meshReader->GetOutput( );


  vtkMimxAbaqusFileWriter *abaqusWriter = vtkMimxAbaqusFileWriter::New( );
  abaqusWriter->SetInput( inputMesh );
  abaqusWriter->SetFileName( abaqusFileName );
  abaqusWriter->SetNodeElementFileName( abaqusNodeElementFileName );
  if (userName.length() > 0)
  {
    abaqusWriter->SetUserName( userName );
  }
  
  if (departmentName.length() > 0)
  {
    abaqusWriter->SetDepartmentName( departmentName );
  }
  
  if (addressLine1.length() > 0)
  {
    abaqusWriter->SetAddressLine1( addressLine1 );
  }
  
  if (addressLine2.length() > 0)
  {
    abaqusWriter->SetAddressLine1( addressLine2 );
  }
  
  if (materialPropertyName.length() > 0)
  {
    abaqusWriter->SetMaterialPropertyName( materialPropertyName );
  }
  
  if (headingName.length() > 0)
  {
    abaqusWriter->SetHeading( headingName );
  }
    
  abaqusWriter->Update( );
  
  return EXIT_SUCCESS;
}



