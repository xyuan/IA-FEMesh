#ifndef _GENERATENORMALS_H_
#define _GENERATENORMALS_H_


#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkImageMarchingCubes.h>
#include <itkImage.h>
#include <vtkImageData.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>


//  Register two surface meshes using the Iterative Closest Point Algorithm


int GenerateNormals( const char *inputSurfaceFile, const char *outputSurfaceFile);
#endif
