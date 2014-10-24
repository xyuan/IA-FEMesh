/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DistanceMap.cxx,v $
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
typedef itk::Image<float, imageDimension>  DistanceImageType;
typedef itk::Image<unsigned char, imageDimension>  ThresholdImageType;
typedef DistanceImageType::PointType       ImagePointType;

float EucledianDistance (double point[3], ImagePointType imageLoc);


//  Register two surface meshes using the Iterative Closest Point Algorithm


int main( int argc, char * argv[] )
{

  MetaCommand command;

  command.SetOption("InputSurface","i",false,"Input Surface File");
  command.AddOptionField("InputSurface","filename",MetaCommand::STRING,true);
  
  command.SetOption("DistanceMap","o",false,"Distance Map Image Filename");
  command.AddOptionField("DistanceMap","filename",MetaCommand::STRING,true);
  
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
  const std::string OutputImageFilename (command.GetValueAsString("DistanceMap","filename"));  
      
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
  
  vtkFloatingPointType center[3];
  originalPolyData->GetCenter( &(center[0]) );
 
/* 
  vtkTransform *centerTransform = vtkTransform::New();
    centerTransform->Translate(-center[0],-center[1],-center[2]);
	
  vtkTransformPolyDataFilter *resampleMeshFilter = vtkTransformPolyDataFilter::New();
    resampleMeshFilter->SetInput( originalPolyData );
    resampleMeshFilter->SetTransform( centerTransform );
    resampleMeshFilter->Update();
*/
  vtkPolyDataNormals *meshNormals = vtkPolyDataNormals::New();
      meshNormals->SetInput( originalPolyData );
//      meshNormals->SetFeatureAngle(60.0);
      meshNormals->Update( );
  
  vtkPolyData *resampledMesh = meshNormals->GetOutput(); //resampleMeshFilter->GetOutput( );
  double bounds[6];
  resampledMesh->GetBounds ( bounds );

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
  typedef DistanceImageType::SpacingType     ImageSpacingType;
  DistanceImageType::Pointer distanceImage  = DistanceImageType::New();
   
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
  distanceImage->SetSpacing( spacing );

  /* Set the Image Origin */
  ImagePointType origin;
  origin[0] = bounds[0]-ImageXPad*ImageXRes;
  origin[1] = bounds[2]-ImageYPad*ImageYRes;
  origin[2] = bounds[4]-ImageZPad*ImageZRes;
  distanceImage->SetOrigin( origin );

  /* Initialize the Distance Map */
  distanceImage->SetLargestPossibleRegion( region );
  distanceImage->SetBufferedRegion( region );
  distanceImage->SetRequestedRegion( region );
  distanceImage->Allocate();

  std::cout << distanceImage << std::endl;

  /* Create the Signed Distance Map */ 
  typedef itk::ImageRegionIteratorWithIndex<DistanceImageType>  IteratorType;  
  IteratorType it( distanceImage, distanceImage->GetBufferedRegion() );

  int numberOfMeshPoints = resampledMesh->GetNumberOfPoints();
  vtkPoints* meshVertices = resampledMesh->GetPoints();
  vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet(resampledMesh);
  obb->SetMaxLevel(10);
  obb->SetTolerance(0.0001);
  obb->BuildLocator();

  std::cout << "Computing Distance" << std::endl;
  int count = 0;
  ImageIndexType comIndex; 
  comIndex[0] = 0; comIndex[1] = 0; comIndex[2] = 0;

  while( !it.IsAtEnd() ) 
    {
    //std::cout << "Index:" << it.GetIndex() << std::endl;

    float minDistance = 10e20;
    float signedDistance=1.0;
    ImagePointType indexLoc;
    distanceImage->TransformIndexToPhysicalPoint(it.GetIndex( ), indexLoc);
    for (int i=0;i<numberOfMeshPoints;i++)
      {
      //std::cout << "Vert:" << i << std::endl;
      double meshVert[3];
      meshVertices->GetPoint(i, meshVert);
      float distance=EucledianDistance( meshVert, indexLoc );
      if (distance < minDistance) 
        {
        minDistance = distance;
        double dp[3];
        dp[0] = static_cast<double>(indexLoc[0]);
        dp[1] = static_cast<double>(indexLoc[1]);
        dp[2] = static_cast<double>(indexLoc[2]);
        //std::cout << "Compute OBB:" << std::endl;
        if ( obb->InsideOrOutside( dp ) < 0 ) signedDistance = -1.0;
        }
      }
    it.Set( minDistance * signedDistance );
    if ( signedDistance < 0 )
      {
      comIndex[0] += it.GetIndex( )[0];
      comIndex[1] += it.GetIndex( )[1];
      comIndex[2] += it.GetIndex( )[2];
      count++;
      }
    ++it;
    }

  comIndex[0] /= count;
  comIndex[1] /= count;
  comIndex[2] /= count;

  std::cout << "Center of Mass:" << comIndex << std::endl;

  /* Threshold Image */
  typedef itk::BinaryThresholdImageFilter<DistanceImageType, ThresholdImageType> ThresholdFilterType;
  ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
  thresholdFilter->SetInput( distanceImage );
  thresholdFilter->SetOutsideValue( 0 );
  thresholdFilter->SetInsideValue( 100 );
  thresholdFilter->SetLowerThreshold( 0.0 );
  thresholdFilter->SetUpperThreshold( 10000.0 );
  thresholdFilter->Update();

  /***************** Used for Debugging *****************
  {
  typedef itk::ImageFileWriter<ThresholdImageType> FileWriterType;
  FileWriterType::Pointer writer = FileWriterType::New();
  writer->SetInput( thresholdFilter->GetOutput( ) );
  writer->SetFileName( "threshold.img" );
  writer->Update();
  }
  *******************************************************/

  /* Region Grow */
  typedef itk::ConnectedThresholdImageFilter<ThresholdImageType, ThresholdImageType> RegionGrowFilterType;
  RegionGrowFilterType::Pointer regionGrowFilter = RegionGrowFilterType::New();
  regionGrowFilter->SetInput( thresholdFilter->GetOutput( ) );
  regionGrowFilter->SetLower( 10 );
  regionGrowFilter->SetUpper( 100 );
  regionGrowFilter->SetReplaceValue( 100 );
  regionGrowFilter->SetSeed( comIndex );
  regionGrowFilter->Update( );


  /***************** Used for Debugging *****************
  {
  typedef itk::ImageFileWriter<ThresholdImageType> FileWriterType;
  FileWriterType::Pointer writer = FileWriterType::New();
  writer->SetInput( regionGrowFilter->GetOutput( ) );
  writer->SetFileName( "region.img" );
  writer->Update();
  }
  *******************************************************/

  typedef itk::ImageRegionIteratorWithIndex<ThresholdImageType>  BinaryIteratorType;
  BinaryIteratorType thIt( thresholdFilter->GetOutput(), thresholdFilter->GetOutput()->GetBufferedRegion() );
  BinaryIteratorType rgIt( regionGrowFilter->GetOutput(), regionGrowFilter->GetOutput()->GetBufferedRegion() );
  it.GoToBegin( );
  while( !it.IsAtEnd() ) 
    {
    if ((rgIt.Get() == 0) && ( thIt.Get() > 0 ))
      {
      it.Set(it.Get()*-1.0);
      }
    ++it;
    ++rgIt;
    ++thIt;
    }

 
 /* Write out the Distance Map Image */
  typedef itk::ImageFileWriter<DistanceImageType> FileWriterType;
  FileWriterType::Pointer writer = FileWriterType::New();
  std::cout << "Saving image " << OutputImageFilename << " ...." << std::endl;
  writer->SetInput( distanceImage );
  writer->SetFileName( OutputImageFilename.c_str() );
  writer->Update();
  std::cout << "Done!" << std::endl;

  return 0;
}

float EucledianDistance (double point[3], ImagePointType imageLoc)
{
  double d = sqrt   ((point[0] - imageLoc[0]) * (point[0] - imageLoc[0]) +
                     (point[1] - imageLoc[1]) * (point[1] - imageLoc[1]) +
                     (point[2] - imageLoc[2]) * (point[2] - imageLoc[2]));

  return static_cast<float>  (d);
}
