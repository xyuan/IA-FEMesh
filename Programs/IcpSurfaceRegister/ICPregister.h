#ifndef _ICPREGISTER_H_
#define _ICPREGISTER_H_


#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>


//  Register two surface meshes using the Iterative Closest Point Algorithm


int ICPregistration( const char *fixedSurface, 
					 const char *movingSurface, 
					 const char *registeredSurface, 
					 const int numberOfIterations,
					 const int maximumNumberPoints,
					 const float maximumDistance);
#endif
