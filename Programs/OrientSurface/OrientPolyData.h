#ifndef _ORIENTPOLYDATA_H_
#define _ORIENTPOLYDATA_H_


#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>

//  Register two surface meshes using the Iterative Closest Point Algorithm


int OrientPolyData( const char *fixedSurface, 
		    const char *registeredSurface, 
		    const int Xaxis, const int Yaxis, const int Zaxis,
		    const int flipX, const int flipY, const int flipZ);
#endif
