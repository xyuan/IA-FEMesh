/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SurfaceToImage.cxx,v $
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

#include <string>

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkOBBTree.h>
#include <vtkPoints.h>
#include <vtkPolyDataNormals.h>

#include <metaCommand.h>
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkSpatialObjectToImageFilter.h>
#include <itkDefaultDynamicMeshTraits.h>
#include <itkMesh.h>
#include <itkMeshSpatialObject.h>


#include <vtkPolyDataToitkMesh.h>


#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

const unsigned int imageDimension = 3;
typedef itk::Image<unsigned char, imageDimension>  OutputImageType;



//  Convert a VTK Surface to a binary Image


int main( int argc, char * argv[] )
{

  MetaCommand command;

  command.SetOption("InputSurface","i",false,"Input Surface File");
  command.AddOptionField("InputSurface","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputImage","o",false,"Output Image Filename");
  command.AddOptionField("OutputImage","filename",MetaCommand::STRING,true);
  
  command.SetOption("size","s",false,"Image Size");
  command.AddOptionField("size","xsize",MetaCommand::INT,false,"64");
  command.AddOptionField("size","ysize",MetaCommand::INT,false,"64");
  command.AddOptionField("size","zsize",MetaCommand::INT,false,"64");
  
  command.SetOption("resolution","r",false,"Image Resolution [1.0 1.0 1.0]");
  command.AddOptionField("resolution","xsize",MetaCommand::FLOAT,false,"1.0");
  command.AddOptionField("resolution","ysize",MetaCommand::FLOAT,false,"1.0");
  command.AddOptionField("resolution","zsize",MetaCommand::FLOAT,false,"1.0");
  
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string InputSurfaceFilename (command.GetValueAsString("InputSurface","filename"));
  const std::string OutputImageFilename (command.GetValueAsString("OutputImage","filename"));  
      
  int ImageXSize = command.GetValueAsInt("size","xsize");
  int ImageYSize = command.GetValueAsInt("size","ysize");
  int ImageZSize = command.GetValueAsInt("size","zsize");
  float ImageXRes = command.GetValueAsFloat("resolution","xsize");
  float ImageYRes = command.GetValueAsFloat("resolution","ysize");
  float ImageZRes = command.GetValueAsFloat("resolution","zsize");
  
  std::cout << "Input Surface: " <<  InputSurfaceFilename << std::endl; 
  std::cout << "Output Image: " <<  OutputImageFilename << std::endl; 
  std::cout << "Image X Pad: " << ImageXSize <<std::endl;
  std::cout << "Image Y Pad: " << ImageYSize <<std::endl;
  std::cout << "Image Z Pad: " << ImageZSize <<std::endl;
  std::cout << "Image X Resolution: " << ImageXRes <<std::endl;
  std::cout << "Image Y Resolution: " << ImageYRes <<std::endl;
  std::cout << "Image Z Resolution: " << ImageZRes <<std::endl;
   

  
  /*** Read the Mesh For Distance Map ***/
  vtkPolyData *originalPolyData;
  if ( (strstr(InputSurfaceFilename.c_str(), ".stl") != NULL) || 
       (strstr(InputSurfaceFilename.c_str(), ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(InputSurfaceFilename.c_str());
    fixedReader->Update( );
    originalPolyData = fixedReader->GetOutput();
    }
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName(argv[1]);
    polyReader->Update( );	
    originalPolyData = polyReader->GetOutput();
    }
  
  vtkFloatingPointType center[3];
  originalPolyData->GetCenter( &(center[0]) );


  /* Convert VTK Surface to ITK Spatial Object */
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> TriangleMeshTraits;
  typedef itk::Mesh<double,3, TriangleMeshTraits> TriangleMeshType;
  vtkPolyDataToitkMesh *adapterFilter = new vtkPolyDataToitkMesh;
    adapterFilter->SetInput( originalPolyData );

  
  typedef itk::MeshSpatialObject<TriangleMeshType> MeshSpatialObjectType;
  MeshSpatialObjectType::Pointer meshSpatialObject = MeshSpatialObjectType::New();
    meshSpatialObject->SetMesh( adapterFilter->GetOutput( ) );

  typedef itk::SpatialObjectToImageFilter<MeshSpatialObjectType, OutputImageType> SpatialObjectToImageFilterType;
  SpatialObjectToImageFilterType::Pointer meshToImageFilter = SpatialObjectToImageFilterType::New();
  
   /* Define the Image Size and Spacing */
  typedef itk::Size<imageDimension>          ImageSizeType;
  ImageSizeType size;
  size[0] = ImageXSize;
  size[1] = ImageYSize;
  size[2] = ImageZSize;

  typedef OutputImageType::SpacingType     ImageSpacingType;
  ImageSpacingType spacing;
  spacing[0] = ImageXRes;
  spacing[1] = ImageYRes;
  spacing[2] = ImageZRes;

  meshToImageFilter->SetInput( meshSpatialObject );
    meshToImageFilter->SetSpacing ( spacing );
    meshToImageFilter->SetSize ( size );
    meshToImageFilter->SetInsideValue( 1 );
    meshToImageFilter->SetOutsideValue ( 0 );
    meshToImageFilter->Update ( );


  typedef itk::ImageFileWriter<OutputImageType> FileWriterType;
  FileWriterType::Pointer writer = FileWriterType::New();
  writer->SetInput( meshToImageFilter->GetOutput( ) );
  writer->SetFileName( OutputImageFilename.c_str() );
  writer->Update();
  
  return 0;
}

