/*=========================================================================
 *
 *   Program:   IaFeMesh
 *   Module:    $RCSfile: TestImageToVtk.cxx,v $
 *   Language:  C++
 *   Date:      $Date: 2006/10/19 02:57:06 $
 *   Version:   $Revision: 1.3 $
 *
 *   Copyright (c) Nicole Grosland. All rights reserved.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notices for more information.
 *
 *=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "vtkPoints.h"
#include "vtkHexahedron.h"
#include "vtkTetra.h"
#include "itkImage.h"
#include "itkMesh.h"
#include "itkVertexCell.h"
#include "itkImageToHexahedronMeshSource.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "itkAutomaticTopologyMeshSource.h"

#include "itkImageToVTKImageFilter.h"



int main( int argc, char * argv[] )
{  

  std::cout << "TestImageToVtk:" << std::endl;
  
  
  typedef    signed short    PixelType;
  
  typedef itk::Image< PixelType,  3 >   ImageType;

  ImageType::Pointer image = ImageType::New();
  
  const ImageType::SizeType size = {{10,10,10}};
  const ImageType::IndexType index = {{0,0,0}};
  double imageSpacing[3];
  imageSpacing[0] = 2.0; imageSpacing[1] = 2.0; imageSpacing[2] = 2.0;
  
  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  image->SetLargestPossibleRegion( region );
  image->SetBufferedRegion( region );
  image->SetRequestedRegion( region );
  image->SetSpacing(imageSpacing);
  image->Allocate();
  
  for (int x=0;x<1;x++)
    {
    for (int y=0;y<2;y++)
      {
      for (int z=0;z<3;z++)
        {
        ImageType::IndexType tmpIndex;
        tmpIndex[0] = x; tmpIndex[1] = y; tmpIndex[2] = z;
        image->SetPixel(tmpIndex, 1);
        }
      }
    }
  
  typedef itk::ImageToVTKImageFilter<ImageType> ImageToVtkType;
  
  ImageToVtkType::Pointer toVtkFilter = ImageToVtkType::New();
  toVtkFilter->SetInput( image );
  toVtkFilter->Update();
  
  vtkImageData *vtkImage = toVtkFilter->GetOutput();
  int dims[3];
  vtkImage->GetDimensions(dims);
  if ((dims[0] != 10) || (dims[1] != 10) || (dims[2] != 10))
    {
    std::cout << "Error: Invalid VTK image dimensions" << std::endl;
    return EXIT_FAILURE;
    }
  
  double res[3];  
  vtkImage->GetSpacing(res);
  if ((res[0] != 2.0) || (res[1] != 2.0) || (res[2] != 2.0))
    {
    std::cout << "Error: Invalid VTK image spacing" << std::endl;
    return EXIT_FAILURE;
    }
    
  for (int x=0;x<10;x++)
    { 
    for (int y=0;y<10;y++)
      {
      for (int z=0;z<10;z++)
        {
	PixelType *ptr = (PixelType *) vtkImage->GetScalarPointer(x,y,z);
        PixelType result;
        if ((x < 1) && (y < 2) && (z < 3))
          {
          result = 1;
          }
        else
          {
          result = 0;
          }
        if (*ptr != result)
          {
          std::cout << "Error: Invalid VTK voxel data" << std::endl;
          return EXIT_FAILURE;
          }
        }
      }
    }
  
  
  return EXIT_SUCCESS;
}  
  
  
