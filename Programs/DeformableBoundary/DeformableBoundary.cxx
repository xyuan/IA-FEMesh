/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DeformableBoundary.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/09 22:10:41 $
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



#include <iostream>
#include <itkBrains2MaskImageIO.h>
#include <itkBrains2MaskImageIOFactory.h>
#include "itkBinaryMask3DMeshSource.h"
#include "itkDeformableMesh3DFilter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkImage.h"
#include "itkMesh.h"
#include "itkCovariantVector.h"
#include "itkPointSetToImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSigmoidImageFilter.h"

#include "itkImageToVTKImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkSimplexMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkTriangleMeshToSimplexMeshFilter.h"
#include "itkVertexCell.h"
#include "itkCellInterfaceVisitor.h"
#include "itkCommand.h"

#include "itkDeformableSimplexMesh3DGradientConstraintForceFilter.h"
#include "itkDeformableSimplexMesh3DFilter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkSobelEdgeDetectionImageFilter.h"

#include "itkSimplexMeshToTriangleMeshFilter.h"
#include "itkTriangleMeshToBinaryImageFilter.h"

#include "vtkPolyDataToitkMesh.h"
#include "itkMeshTovtkPolyData.h"
#include "vtkImageMarchingCubes.h"
#include "vtkDecimatePro.h"

 

int main( int argc, char *argv[] )
{

  itk::Brains2MaskImageIOFactory::RegisterOneFactory();
  
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " InputImage  BinaryImage DeformedMaskImage" << std::endl;
    return 1;
    }
 
  /************************************************************
   * Set the Input Parameters
   ************************************************************/
  char *inputImageFile = argv[1];
  char *inputMaskFile  = argv[2];
  char *outputMaskFile = argv[3];
  float DecimateLevel  = atof( argv[4] ); /* Original 0.9 */
  float SigmaValue     = atof( argv[5] ); /* Original 0.5 */
  float sAlpha         = atof( argv[6] ); /* Original 230 */
  float sBeta          = atof( argv[7] ); /* Original 1300 */
  int   Iterations     = atoi( argv[8] );
  float dAlpha         = atof( argv[9] );   /* Original  0.8 | Range 0.01 < alpha < 0.3  */
  float dBeta          = atof( argv[10] );	/* Original  0.8 | Range 0.01 < beta < 1  */
  float Gamma          = atof( argv[11] );  /* Original  0.32 | Mesh quality < 0.03 regular mesh. Higher values  0.3 < gamma < 0.2 more verts on curves */
  int   Range          = atoi( argv[12] );  /* Original 2 | Smoothing Parameter 0-1 low smoothing 7-8 high smoothing */
  int   Rigidity       = atoi( argv[13] );  /* Original 0 - Regularization 0-10 */

  	
  const     unsigned int    Dimension = 3;
  typedef   float           PixelType;
  typedef itk::Image<PixelType, Dimension>         ImageType;
  typedef itk::Image< unsigned char, Dimension >   BinaryImageType;
  typedef itk::Image<unsigned char, 3>             MeshPixelType;
  
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> TriangleMeshTraits;
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double,double> SimplexMeshTraits;
  typedef itk::Mesh<double,3, TriangleMeshTraits> TriangleMeshType;
  typedef itk::SimplexMesh<double,3, SimplexMeshTraits> SimplexMeshType;
  typedef itk::TriangleMeshToSimplexMeshFilter<TriangleMeshType, SimplexMeshType> SimplexFilterType;
  typedef itk::SimplexMeshToTriangleMeshFilter<SimplexMeshType,TriangleMeshType>  TriangleFilterType;
  typedef itk::DeformableSimplexMesh3DGradientConstraintForceFilter<SimplexMeshType,SimplexMeshType> DeformFilterType;
  typedef DeformFilterType::GradientImageType       GradientImageType;
  
  
  typedef itk::ImageFileReader< ImageType >  ImageReaderType;
  ImageReaderType::Pointer imageReader    =  ImageReaderType::New();
  imageReader->SetFileName(  argv[1] );
  imageReader->Update();

    
  typedef itk::ImageFileReader< BinaryImageType >  BinaryReaderType;
  BinaryReaderType::Pointer maskReader    =  BinaryReaderType::New();
  maskReader->SetFileName(  argv[2] );
  maskReader->Update();

    
  typedef itk::ImageToVTKImageFilter < MeshPixelType > ImageToVTKImageType;
  ImageToVTKImageType::Pointer ImageToVTKImage = ImageToVTKImageType::New();
  ImageToVTKImage->SetInput(maskReader->GetOutput());
  ImageToVTKImage->Update();
  
  
  /************************************************************
   * Convert Mask to a Simplex Mesh
   ************************************************************/
  
  vtkImageMarchingCubes*  vmarchingcubes = vtkImageMarchingCubes::New();
  vmarchingcubes->SetInput(ImageToVTKImage->GetOutput());
  vmarchingcubes->SetValue(0, 0.5);
  vmarchingcubes->ComputeScalarsOff();
  vmarchingcubes->ComputeNormalsOff();
  vmarchingcubes->ComputeGradientsOff();
  vmarchingcubes->SetInputMemoryLimit(1000);
  vmarchingcubes->Update();
  
  vtkDecimatePro* vdecimate = vtkDecimatePro::New(); 
  vdecimate->SetInput(vmarchingcubes->GetOutput()); 
  vdecimate->SetTargetReduction( DecimateLevel ); //compression factor, closer to 1 higher compression)
  vdecimate->PreserveTopologyOn();
  vdecimate->Update();
  
  /***VAM Allocate Poly Data */
  vtkPolyDataToitkMesh *ItkPolyData = new vtkPolyDataToitkMesh;
  ItkPolyData->SetInput(vdecimate->GetOutput());
  
  
  SimplexFilterType::Pointer SimplexMeshFilter = SimplexFilterType::New();
  SimplexMeshFilter->SetInput( ItkPolyData->GetOutput() );
  SimplexMeshFilter->Update();
   
  SimplexMeshType::Pointer simplexMesh = SimplexMeshFilter->GetOutput();

  /* Cleanup Filters that are no longer required */
  simplexMesh->DisconnectPipeline();
  SimplexMeshFilter->Delete();  
  vmarchingcubes->Delete();
  vdecimate->Delete();
  
  
  /************************************************************
   * Compute Internal Forces
   ************************************************************/
  typedef itk::SigmoidImageFilter< ImageType, ImageType > SigmoidFilterType;
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter < ImageType, ImageType >  GradientMagnitudeType;
  typedef itk::GradientRecursiveGaussianImageFilter<ImageType,GradientImageType> GradientGaussianFilterType;
  
  GradientMagnitudeType::Pointer GradientMagnitudeFilter = GradientMagnitudeType::New();
  
  
  GradientMagnitudeFilter->SetInput( imageReader->GetOutput() );
  GradientMagnitudeFilter->SetSigma( SigmaValue );
  
  SigmoidFilterType::Pointer SigmoidFilter = SigmoidFilterType::New();
		  
  SigmoidFilter->SetInput( GradientMagnitudeFilter->GetOutput());
  SigmoidFilter->SetOutputMinimum(0);
  SigmoidFilter->SetOutputMaximum(1);
  SigmoidFilter->SetAlpha( (double) sAlpha ); //230
  SigmoidFilter->SetBeta( (double) sBeta ); //1300
  
  GradientGaussianFilterType::Pointer GradientGaussianFilter = GradientGaussianFilterType::New();
  
  GradientGaussianFilter->SetInput( SigmoidFilter->GetOutput());
  GradientGaussianFilter->SetSigma( SigmaValue ); //0.5
  GradientGaussianFilter->Update();
  
  
  /************************************************************
   * Deform Mesh
   ************************************************************/
  
  DeformFilterType::Pointer DeformFilter = DeformFilterType::New();
 
  
  DeformFilter->SetGradient( GradientGaussianFilter->GetOutput() );  
  DeformFilter->SetImage( imageReader->GetOutput( ) );        
  DeformFilter->SetInput( simplexMesh );
  DeformFilter->SetIterations( Iterations ); 
  DeformFilter->SetAlpha( (double) dAlpha );
  DeformFilter->SetBeta( (double) dBeta );	  
  DeformFilter->SetGamma(  (double) Gamma );
  DeformFilter->SetRange( Range );
  DeformFilter->SetRigidity( Rigidity );		  
  DeformFilter->Update();

  
  /************************************************************
   * Save Deformed Simplex Mesh as a Mask
   ************************************************************/
  double orgn[3];
  orgn[0] = imageReader->GetOutput()->GetOrigin()[0];
  orgn[1] = imageReader->GetOutput()->GetOrigin()[1];
  orgn[2] = imageReader->GetOutput()->GetOrigin()[2];

  std::cout << "Converting Simplex Mesh to Triangle Mesh . . ." << std::endl;
  
  TriangleFilterType::Pointer SimplexToTriangleFilter = TriangleFilterType::New();
  SimplexToTriangleFilter->SetInput(DeformFilter->GetOutput());
  SimplexToTriangleFilter->Update();
  
  TriangleMeshType::Pointer triangleMesh = SimplexToTriangleFilter->GetOutput();
  triangleMesh->DisconnectPipeline();
  SimplexToTriangleFilter->Delete();
  
  typedef itk::TriangleMeshToBinaryImageFilter<TriangleMeshType, MeshPixelType> TriangleMeshToBinaryImageFilterType;
  TriangleMeshToBinaryImageFilterType::Pointer TriangleToImage = TriangleMeshToBinaryImageFilterType::New();

  TriangleToImage->SetInput( triangleMesh );
  MeshPixelType::SizeType size;

  size[0] = imageReader->GetOutput()->GetBufferedRegion().GetSize()[0];
  size[1] = imageReader->GetOutput()->GetBufferedRegion().GetSize()[1];
  size[2] = imageReader->GetOutput()->GetBufferedRegion().GetSize()[2];
  TriangleToImage->SetSize(size);
  
 
  TriangleToImage->SetOrigin(orgn);
  // spacing remains (1,1,1) until we make a change to deformable model class
  /* This was code from an example - Not sure why this was required
  float spacing[3];
  spacing[0] = 1;
  spacing[1] = 1;
  spacing[2] = 1;
  */
  TriangleToImage->SetSpacing( imageReader->GetOutput()->GetSpacing() );

  TriangleToImage->Update();
  
  TriangleToImage->GetOutput()->SetMetaDataDictionary( imageReader->GetOutput()->GetMetaDataDictionary() );
  
  /* It would be great to write this as a mask */
  typedef itk::ImageFileWriter< BinaryImageType >  MaskWriterType;
  MaskWriterType::Pointer maskWriter = MaskWriterType::New();
  maskWriter->SetInput( TriangleToImage->GetOutput() );
  maskWriter->SetFileName( outputMaskFile );
  maskWriter->UseInputMetaDataDictionaryOn();
  maskWriter->Update();
  
  return EXIT_SUCCESS;
}
