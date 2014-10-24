/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: AssignMeshMaterialPropertiesModule.cxx,v $
Language:  C++
Date:      $Date: 2008/01/12 22:29:57 $
Version:   $Revision: 1.1 $

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
#include <itkOrientedImage.h>
#include <itkImageFileReader.h>
#include <itkSpatialOrientationAdapter.h>


#include "vtkMimxApplyImageBasedMaterialProperties.h"
#include "AssignMeshMaterialPropertiesModuleCLP.h"


template <class TImageType>
void AdaptOriginAndDirection( typename TImageType::Pointer image )
{
  typename TImageType::DirectionType imageDir = image->GetDirection( );
  typename TImageType::PointType origin = image->GetOrigin( );
  
  int dominantAxisRL = itk::Function::Max3(imageDir[0][0],imageDir[1][0],imageDir[2][0]);
  int signRL = itk::Function::Sign(imageDir[dominantAxisRL][0]);
  int dominantAxisAP = itk::Function::Max3(imageDir[0][1],imageDir[1][1],imageDir[2][1]);
  int signAP = itk::Function::Sign(imageDir[dominantAxisAP][1]);
  int dominantAxisSI = itk::Function::Max3(imageDir[0][2],imageDir[1][2],imageDir[2][2]);
  int signSI = itk::Function::Sign(imageDir[dominantAxisSI][2]); 

  /* This current  algorithm needs to be verified.
     I had previously though that it should be
     signRL == 1
     signAP == -1 
     signSI == 1
     This appears to be incorrect with the NRRD file format
     at least. Visually this appears to work
     signRL == 1
     signAP == 1 
     signSI == -1
  */
  typename TImageType::DirectionType DirectionToRAS;
  DirectionToRAS.SetIdentity( );
  if (signRL == 1)
    {
    DirectionToRAS[dominantAxisRL][dominantAxisRL] = -1.0; 
    origin[dominantAxisRL] *= -1.0;
    }
  if (signAP == 1)
    {
    DirectionToRAS[dominantAxisAP][dominantAxisAP] = -1.0; 
    origin[dominantAxisAP] *= -1.0;
    }
  if (signSI == 1)
    {
    DirectionToRAS[dominantAxisSI][dominantAxisSI] = -1.0; 
    origin[dominantAxisSI] *= -1.0;
    }
  imageDir *= DirectionToRAS;
  image->SetDirection( imageDir  );
  image->SetOrigin( origin );
}



int main( int argc, char **argv)
{

  PARSE_ARGS;
  
  bool debug = true;
  if ( debug )
    {
    // Summarize the command line arguements for the user
    std::cout << "Abaqus Writer Parameters" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "\tInput Mesh: " << vtkInputMeshFileName << std::endl;
    std::cout << "\tInput Image: " << imageFileName << std::endl;
    std::cout << "\tOutput Mesh: " << vtkOutputMeshFileName << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
  }
  
  
  // Load the VTK mesh
  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SetFileName( vtkInputMeshFileName.c_str() );
  meshReader->Update();
  vtkUnstructuredGrid *inputMesh = meshReader->GetOutput( );
  
  // Load in the ITK Image
  typedef    signed short                               InputPixelType;
  typedef    itk::OrientedImage< InputPixelType,  3 >   ImageType;
  typedef    ImageType::DirectionType                   DirectionType;
  typedef itk::ImageFileReader< ImageType >  ReaderType;
  ReaderType::Pointer imageReader = ReaderType::New( );
  imageReader->SetFileName( imageFileName.c_str( ) );
  imageReader->Update( );
  
  ImageType::Pointer inputImage = imageReader->GetOutput( ) ;
  if ( debug ) std::cout << "Original Image: " << inputImage << std::endl;
  AdaptOriginAndDirection<ImageType>( inputImage );
  if ( debug ) std::cout << "RAS Image: " << inputImage << std::endl;
  
  
  
  vtkMimxApplyImageBasedMaterialProperties *applyMaterialPropertyFilter = 
      vtkMimxApplyImageBasedMaterialProperties::New();
      
  applyMaterialPropertyFilter->SetInput( inputMesh );
  applyMaterialPropertyFilter->SetITKOrientedImage( inputImage );
  applyMaterialPropertyFilter->Update();  

  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New( );
  meshWriter->SetInput( applyMaterialPropertyFilter->GetOutput() );
  meshWriter->SetFileName( vtkOutputMeshFileName.c_str() );
  meshWriter->Update( );
  
  return EXIT_SUCCESS;
}



