/*=========================================================================
 
  Program:   Insight Segmentation & Registration Toolkit 
  Module:    $RCSfile: MappedMeshFemRegister.cxx,v $
  Language:  C++
  Date:      $Date: 2006/09/07 13:28:53 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itkImageFileReader.h" 
#include "itkImageFileWriter.h" 
#include "itkRescaleIntensityImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkFEM.h"
#include "itkFEMRegistrationFilter.h"
#include "itkDeformationFieldJacobianDeterminantFilter.h"
#include "itkVector.h"
#include "itkAddImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkWarpImageFilter.h"
#include "itkCheckerBoardImageFilter.h"


int main(int argc, char *argv[])
{
  std::cout << "FEM Mapped Mesh Program." << std::endl;

  const unsigned int ImageDimension = 3;
   
  typedef itk::Image<unsigned char, ImageDimension>                       ImageType;
  typedef itk::Image<double, ImageDimension>                              RealImageType;
  typedef itk::fem::FEMRegistrationFilter<RealImageType, RealImageType, 
                                                            ImageType>    RegistrationType;
  typedef RegistrationType::PDEDeformableMetricLoadType                   ImageLoadType;

  typedef itk::fem::Element3DC0LinearHexahedronMembrane                   ElementType;
  typedef ElementType::LoadImplementationFunctionPointer                  LoadImpFP;
  typedef ElementType::LoadType                                           ElementLoadType;
  typedef itk::fem::VisitorDispatcher<ElementType, ElementLoadType, LoadImpFP>   
                                                                          DispatcherType;

  RegistrationType::Pointer registrationFilter = RegistrationType::New(); 

  ElementType::LoadImplementationFunctionPointer fp = 
    &itk::fem::ImageMetricLoadImplementation<ImageLoadType>::ImplementImageMetricLoad;
  DispatcherType::RegisterVisitor((ImageLoadType*)0, fp);
 
  typedef itk::ImageFileReader<ImageType>      FileSourceType;
  typedef RealImageType::PixelType PixType;

//  typedef itk::ImageFileReader<RegistrationType::MaskImageType> MaskImageReaderType;   

  /* Load the Moving Image */
  FileSourceType::Pointer movingfilter = FileSourceType::New();
  movingfilter->SetFileName( SignedDistanceImageFile.c_str() );
  movingfilter->Update();

  /* Load the Fixed Image */
  FileSourceType::Pointer fixedfilter = FileSourceType::New();
  fixedfilter->SetFileName( SignedDistanceImageFile.c_str() );
  fixedfilter->Update();

  // Rescale the image intensities so that they fall between 0 and 255

  typedef itk::RescaleIntensityImageFilter<ImageType,RealImageType> FilterType;
  FilterType::Pointer movingrescalefilter = FilterType::New();
  FilterType::Pointer fixedrescalefilter = FilterType::New();

  movingrescalefilter->SetInput(movingfilter->GetOutput());
  fixedrescalefilter->SetInput(fixedfilter->GetOutput());

  const double desiredMinimum =  0.0;
  const double desiredMaximum =  255.0;

  movingrescalefilter->SetOutputMinimum( desiredMinimum );
  movingrescalefilter->SetOutputMaximum( desiredMaximum );
  movingrescalefilter->UpdateLargestPossibleRegion();
  fixedrescalefilter->SetOutputMinimum( desiredMinimum );
  fixedrescalefilter->SetOutputMaximum( desiredMaximum );
  fixedrescalefilter->UpdateLargestPossibleRegion();

  registrationFilter->DebugOn();
  registrationFilter->SetMovingImageInput(movingrescalefilter->GetOutput());
  registrationFilter->SetFixedImageInput(fixedrescalefilter->GetOutput());

//  Software Guide : BeginCodeSnippet 
  // Create the material properties  

  itk::fem::MaterialLinearElasticity::Pointer m;
  m = itk::fem::MaterialLinearElasticity::New();
  m->GN = 0;                  // Global number of the material
  m->A = 1.0;                 // Cross-sectional area
  m->h = 1.0;                 // Thickness
  m->I = 1.0;                 // Moment of inertia
  m->nu = 0.0;                // Poisson's ratio -- DONT CHOOSE 1.0!!
  m->RhoC = 1.0;              // Density
  m->E = 1e5;

  ElementType::Pointer e1 = ElementType::New();
  e1->m_mat=dynamic_cast<itk::fem::MaterialLinearElasticity*>( m );
  registrationFilter->SetElement(e1);

  registrationFilter->SetMaterial(m);
  registrationFilter->SetElasticity(m->E);

  registrationFilter->SetTimeStep( 0.5 );
  registrationFilter->SetNumberOfLevels( 3 );
  registrationFilter->SetMeshResolution( 32, 0 ); 
  registrationFilter->SetMeshResolution( 64, 1 ); 
  registrationFilter->SetMeshResolution( 128, 2 ); 
  registrationFilter->SetMaximumNumberOfIterations( 20 );
  registrationFilter->SetNumberOfIntegrationPoints( 4 );

/*
Linear triangle:  1 point
Quadratic triangle: 3 points
Linear quadrilateral: 4 points
Quadratic quadrilateral: 4 points
Linear brick: 8 points
Quadratic brick: 27 points
*/

//  typedef itk::MattesMutualInformationImageToImageMetric
//  typedef itk::MeanSquaresImageToImageMetric
//                      <RealImageType, RealImageType> ImageToImageMetricType;  
//  ImageToImageMetricType::Pointer ImageToImageMetric = ImageToImageMetricType::New(); 

//  ImageToImageMetric->UseAllPixelsOn();
//  ImageToImageMetric->DebugOn();

//  registrationFilter->SetImageToImageMetric(ImageToImageMetric);
//  registrationFilter->SetMaximizeMetric(false);
  
 
  try 
  {
    registrationFilter->Update();
  }
  catch (itk::ExceptionObject & exp)
  {
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << exp << std::endl; 
  }  
  
  // Write the checkerboard image
   
  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("FEMresults.hdr");
  writer->SetInput(registrationFilter->GetWarpedImage());
  writer->Update();    

/*
  typedef itk::CheckerBoardImageFilter<ImageType> CheckerFilterType; 
  CheckerFilterType::Pointer checker = CheckerFilterType::New();
  checker->SetInput1( registrationFilter->GetWarpedImage() );
  checker->SetInput2( fixedfilter->GetOutput() );
  CheckerFilterType::PatternArrayType pattern;
  pattern[0] = 10;
  pattern[1] = 10;
  checker->SetCheckerPattern( pattern );  
  checker->Update();
  
  writer->SetFileName("checker.hdr");
  writer->SetInput( checker->GetOutput() );
  writer->Update();    

  // Write the jacobian image of the displacement field

  typedef RegistrationType::VectorType  VectorType;
  typedef RegistrationType::DeformationFieldType  DeformationFieldType;
  typedef double RealType;
  typedef itk::DeformationFieldJacobianDeterminantFilter
     <DeformationFieldType, RealType> JacobianFilterType;
  JacobianFilterType::Pointer JacobianFilter = JacobianFilterType::New();
  JacobianFilter->SetInput(registrationFilter->GetDeformationField());
  RealImageType::Pointer Ones = RealImageType::New();
  Ones->SetRegions(registrationFilter->GetWarpedImage()->GetLargestPossibleRegion());
  Ones->Allocate();
  Ones->FillBuffer(1.0);
  typedef itk::AddImageFilter<RealImageType, RealImageType, RealImageType> AddFilterType;
  AddFilterType::Pointer AddFilter = AddFilterType::New();
  AddFilter->SetInput1(Ones);
  AddFilter->SetInput2(JacobianFilter->GetOutput());
  AddFilter->Update();  
 
  typedef itk::ImageFileWriter<RegistrationType::RealImageType> RealWriterType;
  RealWriterType::Pointer realwriter = RealWriterType::New();
  realwriter->SetFileName("jacobian.hdr");
  realwriter->SetInput(AddFilter->GetOutput()); 
  realwriter->Update();

  // Write the warped grid image

  FileSourceType::Pointer grid = FileSourceType::New();
  grid->SetFileName( "Images/zerogrid.hdr" );
  grid->Update();

  typedef itk::WarpImageFilter<ImageType, 
                          ImageType, 
                          RegistrationType::DeformationFieldType> WarperType;
  WarperType::Pointer warper = WarperType::New();
  warper->SetInput( grid->GetOutput() );
  warper->SetDeformationField( registrationFilter->GetDeformationField() );
  warper->Update();

  writer->SetFileName( "warpedgrid.hdr" );
  writer->SetInput( warper->GetOutput() );
  writer->Update();
*/
  return 0;
}


