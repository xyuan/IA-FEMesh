#ifndef _CREATESURFACE_H_
#define _CREATESURFACE_H_


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


int CreateSurface( const char *inputImage, 
		    const char *outputSurface, 
		    const bool OrientImage, 
			const bool DecimateSurface, 
			const bool SmoothSurface,
			const bool laplacianSmoothing, 
			const bool windowedSincSmoothing,
		    const double SurfaceValue, 
			const double ReductionFactor, 
			const int NumberOfIterations,
			const double RelaxationFactor, 
			const double passBand, 
			const int windowedSincIteration,
            const bool boundarySmoothing, 
			const bool normalizeCoordinates, 
			const bool nonManifoldSmoothing);
#endif
