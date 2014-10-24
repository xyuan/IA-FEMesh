/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: ExtractMaskRegion.cxx,v $
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
//  Software Guide : BeginCommandLineArgs
#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <math.h>
#include <time.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkIdentityTransform.h>
#include <itkResampleImageFilter.h>
#include <metaCommand.h>




int main( int argc, char * argv[] )
{
//  itk::Brains2MaskImageIOFactory::RegisterOneFactory( );

  /** Handle the Command line arguments */
  MetaCommand command;

  command.SetOption("MaskFileName","i",false,"Input Mask File Name");
  command.AddOptionField("MaskFileName","filename",MetaCommand::STRING,true);

  command.SetOption("OutputMaskFileName","o",false,"Output Mask filename");
  command.AddOptionField("OutputMaskFileName","filename",MetaCommand::STRING,false, "");

  command.SetOption("Pad","p",false,"Pad of image in each dimension");
  command.AddOptionField("Pad","xsize",MetaCommand::INT,false, "5");
  command.AddOptionField("Pad","ysize",MetaCommand::INT,false, "5");
  command.AddOptionField("Pad","zsize",MetaCommand::INT,false, "5");

  if ( !command.Parse(argc,argv) )
    {
    return 1;
    }
	
  const std::string InputFileName(command.GetValueAsString("MaskFileName",
                                                            "filename"));
  const std::string OutputFileName(command.GetValueAsString("OutputMaskFileName",
                                                            "filename"));
  int pad[3];
  pad[0] = command.GetValueAsInt("Pad", "xsize");
  pad[1] = command.GetValueAsInt("Pad", "ysize");
  pad[2] = command.GetValueAsInt("Pad", "zsize");
 

  typedef    unsigned char   InputPixelType;
  typedef itk::Image<InputPixelType, 3>  InputImageType;

  /** Read the Input Image */
  typedef itk::ImageFileReader< InputImageType >  ReaderType; 
  ReaderType::Pointer imageReader = ReaderType::New( );
  imageReader->SetFileName( InputFileName.c_str( ) );
  imageReader->Update( );


  InputImageType::SpacingType spacing = 
      imageReader->GetOutput( )->GetSpacing( );
  InputImageType::SizeType size =
      imageReader->GetOutput( )->GetLargestPossibleRegion( ).GetSize( );
  InputImageType::PointType upperBound = 
      imageReader->GetOutput()->GetOrigin();
  InputImageType::PointType lowerBound = 
      imageReader->GetOutput()->GetOrigin();
  
  for (int i=0;i<3;i++)
    {
    lowerBound[i] += size[i] * spacing[i];
    }


  typedef itk::ImageRegionIteratorWithIndex<InputImageType>  IteratorType;  
  IteratorType it( imageReader->GetOutput(), imageReader->GetOutput()->GetBufferedRegion() );

  while( !it.IsAtEnd() ) 
    {
    if ( it.Get() > 0 )
      {
      InputImageType::PointType indexPointLoc;
      imageReader->GetOutput()->TransformIndexToPhysicalPoint (it.GetIndex(), indexPointLoc);
      for (int i=0;i<3;i++)
        {
        if (lowerBound[i] > indexPointLoc[i]) lowerBound[i] = indexPointLoc[i];
        if (upperBound[i] < indexPointLoc[i]) upperBound[i] = indexPointLoc[i];
        }
      }
    ++it;
    }

  InputImageType::SizeType resampleSize;
  for (int i=0;i<3;i++)
    {
    lowerBound[i] -= pad[i] * spacing[i];
    upperBound[i] += pad[i] * spacing[i];
    resampleSize[i] = static_cast<unsigned int>((upperBound[i] - lowerBound[i]) / spacing[i]);
    }
  
  /*** Extract the Region of Interest ***/
  typedef double  CoordRepType;
  typedef itk::NearestNeighborInterpolateImageFunction< InputImageType,CoordRepType >
                                                          InterpolatorType;

  InterpolatorType::Pointer imageInterpolator = InterpolatorType::New( );
  imageInterpolator->SetInputImage( imageReader->GetOutput() );

  typedef itk::IdentityTransform< double, 3 >  IdentityTransformType;
  IdentityTransformType::Pointer imageIdentityTransform = 
                                    IdentityTransformType::New( );

  typedef itk::ResampleImageFilter< InputImageType, InputImageType> 
                                    ResampleImageFilterType;
  ResampleImageFilterType::Pointer resampleImageFilter = 
                                    ResampleImageFilterType::New( );
  resampleImageFilter->SetInput( imageReader->GetOutput() );
  resampleImageFilter->SetTransform( imageIdentityTransform );
  resampleImageFilter->SetInterpolator( imageInterpolator );
  resampleImageFilter->SetOutputSpacing( spacing );
  resampleImageFilter->SetOutputOrigin( lowerBound );
  resampleImageFilter->SetSize( resampleSize );
  resampleImageFilter->Update( );


  /** Write the Extracted Image */
  typedef itk::ImageFileWriter< InputImageType >  WriterType; 
  WriterType::Pointer imageWriter = WriterType::New( );
  imageWriter->SetInput( resampleImageFilter->GetOutput( ) );
  imageWriter->SetFileName( OutputFileName.c_str( ) );
  imageWriter->Update( );


  return EXIT_SUCCESS;
}
