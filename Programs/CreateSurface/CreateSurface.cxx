#include "CreateSurface.h"

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkImageMarchingCubes.h>
#include <itkImage.h>
#include <vtkImageData.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkOrientImageFilter.h>
#include <itkBrains2MaskImageIO.h>
#include <itkBrains2MaskImageIOFactory.h>



#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

//  Register two surface meshes using the Iterative Closest Point Algorithm


int CreateSurface( const char *inputImageFile, 
		    const char *outputSurfaceFile, 
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
			const bool nonManifoldSmoothing
			)
{

	std::cout << "Input Image : " << inputImageFile << std::endl;
  std::cout << "Output Surface: " << outputSurfaceFile << std::endl;

	std::cout << "OrientImage: " << OrientImage << std::endl;
	std::cout << "Surface Value: " << SurfaceValue << std::endl;

	std::cout << "Decimate Surface: " << DecimateSurface << std::endl;
	std::cout << "\tReduction Factor: " << ReductionFactor << std::endl;
	std::cout << "Smooth Surface: " << SmoothSurface << std::endl;
	std::cout << "\tLaplacian Smoothing: " << laplacianSmoothing << std::endl;
	std::cout << "\t\tNumber Of Iterations: " << NumberOfIterations << std::endl;
	std::cout << "\t\tRelaxation Factor: " << RelaxationFactor << std::endl;
	std::cout << "\tWindowed Sinc Smoothing: " << windowedSincSmoothing << std::endl;
	std::cout << "\t\tNumber Of Iterations: " << windowedSincIteration << std::endl;
	std::cout << "\t\tPass Band: " << passBand << std::endl;
	std::cout << "\t\tBoundary Smoothing: " << boundarySmoothing << std::endl;
	std::cout << "\t\tNormalize Coordinates: " << normalizeCoordinates << std::endl;
	std::cout << "\t\tNon-Manifold Smoothing: " << nonManifoldSmoothing << std::endl;
	
  
  /*** Read the Meshes to Register ***/
  typedef signed short PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::ImageFileReader<ImageType> FileReaderType;


  itk::Brains2MaskImageIOFactory::RegisterOneFactory();
  FileReaderType::Pointer reader = FileReaderType::New();
  std::cout << "Loading image " << inputImageFile << " ...." <<std::endl;
  reader->SetFileName( inputImageFile );
  reader->Update();

  ImageType::Pointer inputImage;
  if ( OrientImage )
  {
	  typedef itk::OrientImageFilter<ImageType, ImageType> OrientFilterType;
	  OrientFilterType::Pointer orientImageFilter =  OrientFilterType::New();
	  orientImageFilter->SetInput( reader->GetOutput() );
	  orientImageFilter->SetDesiredCoordinateOrientationToCoronal();
	  orientImageFilter->Update();
	  inputImage = orientImageFilter->GetOutput();
  }
  else
  {
	  inputImage = reader->GetOutput();
  }

  //ITK to VTK Adapter
  typedef itk::ImageToVTKImageFilter<ImageType> ImageToVtkType;
  
  ImageToVtkType::Pointer toVtkFilter = ImageToVtkType::New();
  toVtkFilter->SetInput( inputImage );
  toVtkFilter->Update();
  
  vtkImageData *vtkImage = toVtkFilter->GetOutput();
  
  std::cout << "Creating Surface ... " << std::endl;

  vtkImageMarchingCubes *surfaceFilter = vtkImageMarchingCubes::New();
  surfaceFilter->SetInput( vtkImage );
  surfaceFilter->SetValue( 0, SurfaceValue );
  surfaceFilter->Update();

  vtkPolyData *originalPolyData;
  if ( DecimateSurface )
  {
    std::cout << "Decimating Surface ... " << std::endl;
	  vtkQuadricDecimation  *decimateFilter = vtkQuadricDecimation::New();
	  decimateFilter->SetInput( surfaceFilter->GetOutput() );
	  decimateFilter->SetTargetReduction( ReductionFactor );
    decimateFilter->Update( );
	  originalPolyData = decimateFilter->GetOutput();
  }
  else
  {
	  originalPolyData = surfaceFilter->GetOutput();
  }

  vtkPolyData *finalPolyData;
  if ( SmoothSurface )
  {
	  std::cout << "Smoothing Surface ... " << std::endl;
	  if ( laplacianSmoothing )
	  {
      std::cout << "Laplacian Smoothing ... " << std::endl;
      vtkSmoothPolyDataFilter *smoothSurfaceFilter = vtkSmoothPolyDataFilter::New();
      smoothSurfaceFilter->SetInput( originalPolyData );
      smoothSurfaceFilter->SetNumberOfIterations( NumberOfIterations );
      smoothSurfaceFilter->SetRelaxationFactor( RelaxationFactor );
      smoothSurfaceFilter->SetConvergence( 0.0 );
      smoothSurfaceFilter->Update( );
      finalPolyData = smoothSurfaceFilter->GetOutput();
	  }
	  else
	  {
      std::cout << "Windowed Sinc Smoothing ... " << std::endl;
      vtkWindowedSincPolyDataFilter *smoothSurfaceFilter = vtkWindowedSincPolyDataFilter::New();
      smoothSurfaceFilter->SetInput( originalPolyData );
      smoothSurfaceFilter->SetNumberOfIterations( windowedSincIteration );
      smoothSurfaceFilter->SetPassBand( passBand );
      smoothSurfaceFilter->SetBoundarySmoothing( boundarySmoothing );
      smoothSurfaceFilter->SetNormalizeCoordinates( normalizeCoordinates );
      smoothSurfaceFilter->SetNonManifoldSmoothing( nonManifoldSmoothing );
      smoothSurfaceFilter->Update( );
      finalPolyData = smoothSurfaceFilter->GetOutput();
	  }
  }
  else
  {
	  finalPolyData = originalPolyData;
  }

  std::cout << "Writing Surface: " << outputSurfaceFile << " ..." << std::endl;	
  vtkSTLWriter *meshWriter = vtkSTLWriter::New();
  meshWriter->SetFileName( outputSurfaceFile );
  meshWriter->SetFileTypeToASCII();
  meshWriter->SetInput( finalPolyData );
  meshWriter->Update( );


  return 0;
}
