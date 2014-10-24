/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SurfaceToBinaryImage.cxx,v $
  Language:  C++
  Date:      $Date: 2007/07/10 19:44:56 $
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
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

const unsigned int imageDimension = 3;
typedef itk::Image<unsigned char, imageDimension>  BinaryImageType;
typedef BinaryImageType::PointType       ImagePointType;




int main( int argc, char * argv[] )
{

  MetaCommand command;

  command.SetOption("InputSurface","i",false,"Input Surface File");
  command.AddOptionField("InputSurface","filename",MetaCommand::STRING,true);
  
  command.SetOption("BinaryImage","o",false,"Binary Image Filename");
  command.AddOptionField("BinaryImage","filename",MetaCommand::STRING,true);
  
  command.SetOption("size","s",false,"Image Size Pad [20 20 20]");
  command.AddOptionField("size","xsize",MetaCommand::INT,false,"20");
  command.AddOptionField("size","ysize",MetaCommand::INT,false,"20");
  command.AddOptionField("size","zsize",MetaCommand::INT,false,"20");
  
  command.SetOption("resolution","r",false,"Image Resolution [1.0 1.0 1.0]");
  command.AddOptionField("resolution","xsize",MetaCommand::FLOAT,false,"1.0");
  command.AddOptionField("resolution","ysize",MetaCommand::FLOAT,false,"1.0");
  command.AddOptionField("resolution","zsize",MetaCommand::FLOAT,false,"1.0");
  
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string InputSurfaceFilename (command.GetValueAsString("InputSurface","filename"));
  const std::string OutputImageFilename (command.GetValueAsString("BinaryImage","filename"));  
      
  int ImageXPad = command.GetValueAsInt("size","xsize");
  int ImageYPad = command.GetValueAsInt("size","ysize");
  int ImageZPad = command.GetValueAsInt("size","zsize");
  float ImageXRes = command.GetValueAsFloat("resolution","xsize");
  float ImageYRes = command.GetValueAsFloat("resolution","ysize");
  float ImageZRes = command.GetValueAsFloat("resolution","zsize");
  
  std::cout << "Input Surface: " <<  InputSurfaceFilename << std::endl; 
  std::cout << "Output Image: " <<  OutputImageFilename << std::endl; 
  std::cout << "Image X Pad: " << ImageXPad <<std::endl;
  std::cout << "Image Y Pad: " << ImageYPad <<std::endl;
  std::cout << "Image Z Pad: " << ImageZPad <<std::endl;
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
  
  double bounds[6];
  originalPolyData->GetBounds ( bounds );

  int ImageXSize = static_cast<int> ((bounds[1] - bounds[0]) / ImageXRes) + 1;
  int ImageYSize = static_cast<int> ((bounds[3] - bounds[2]) / ImageYRes) + 1;
  int ImageZSize = static_cast<int> ((bounds[5] - bounds[4]) / ImageZRes) + 1;

  ImageXSize += 2 * ImageXPad;
  ImageYSize += 2 * ImageYPad;
  ImageZSize += 2 * ImageZPad;

  /* Allocate an ITK Image */
  typedef itk::Index<imageDimension>         ImageIndexType;
  typedef itk::Size<imageDimension>          ImageSizeType;
  typedef itk::ImageRegion<imageDimension>   ImageRegionType;
  typedef BinaryImageType::SpacingType       ImageSpacingType;
  BinaryImageType::Pointer binaryImage  = BinaryImageType::New();
   
  /* Define the Image Size and Spacing */
  ImageSizeType size;
  size[0] = ImageXSize;
  size[1] = ImageYSize;
  size[2] = ImageZSize;

  ImageIndexType start;
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  ImageRegionType region;
  region.SetIndex( start );
  region.SetSize( size );

  ImageSpacingType spacing;
  spacing[0] = ImageXRes;
  spacing[1] = ImageYRes;
  spacing[2] = ImageZRes;
  binaryImage->SetSpacing( spacing );

  /* Set the Image Origin */
  ImagePointType origin;
  origin[0] = bounds[0]-ImageXPad*ImageXRes;
  origin[1] = bounds[2]-ImageYPad*ImageYRes;
  origin[2] = bounds[4]-ImageZPad*ImageZRes;
  binaryImage->SetOrigin( origin );

  /* Initialize the Binary Image */
  binaryImage->SetLargestPossibleRegion( region );
  binaryImage->SetBufferedRegion( region );
  binaryImage->SetRequestedRegion( region );
  binaryImage->Allocate();
  binaryImage->FillBuffer( 0 );
  std::cout << binaryImage << std::endl;

  /* Create An Iterator for the Binary Image */ 
  typedef itk::ImageRegionIteratorWithIndex<BinaryImageType>  IteratorType;  
  IteratorType it( binaryImage, binaryImage->GetBufferedRegion() );

  vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet( originalPolyData );
  obb->SetMaxLevel( 99999 );
  obb->SetTolerance(0.0001);
  obb->BuildLocator();

  while( !it.IsAtEnd() ) 
    {
    ImagePointType indexLoc;
    binaryImage->TransformIndexToPhysicalPoint(it.GetIndex( ), indexLoc);
    vtkFloatingPointType dp[3];
    dp[0] = indexLoc[0]; dp[1] = indexLoc[1]; dp[2] = indexLoc[2];
    if (obb->InsideOrOutside( dp ) < 0 ) 
      {
      it.Set( 255 );
      }
    ++it;
    }


  /***************** Write Binary Image *****************/
  typedef itk::ImageFileWriter<BinaryImageType> FileWriterType;
  FileWriterType::Pointer writer = FileWriterType::New();
  writer->SetInput( binaryImage );
  writer->SetFileName( OutputImageFilename.c_str() );
  writer->Update();
  std::cout << "Done!" << std::endl;

  return 0;
}



