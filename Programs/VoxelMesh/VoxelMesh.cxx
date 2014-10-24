/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: VoxelMesh.cxx,v $
Language:  C++
Date:      $Date: 2006/09/18 19:25:43 $
Version:   $Revision: 1.7 $

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

#include <math.h>
#include <time.h>
#include <itkArray.h>
#include <itkImage.h>
#include <itkMesh.h>
#include <itkVertexCell.h>
#include <itkDefaultStaticMeshTraits.h>
#include <itkImageFileReader.h>
#include <itkOrientImageFilter.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include <itkBrains2MaskImageIOFactory.h>
#include <itkMeshSpatialObject.h>
#include <itkSpatialObjectWriter.h>
#include <itkSpatialObjectReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <metaCommand.h>
#include <itksys/Directory.hxx>

#include "itkAbaqusMeshFileWriter.h"
#include "itkImageToHexahedronMeshFilter.h"
#include "itkRebinHexahedronMeshTraitDataInPlaceFilter.h"
#include "itkReadHistogramBinValuesFilter.h"
#include "itkMeshTovtkUnstructuredGrid.h"


/****************************************************************
Program: voxelMesh

Purpose: To generate a hexahedron mesh from an image. The
resulting mesh can be written in several formats including:
1) ITK Spatial Object
2) VTK Unstructured Grid
3) Abaqus Input Deck

The density of the mesh, and the number of modulus values
can be controlled by the user with a number of command 
line arguements. 

Usage : ./VoxelMesh
   [ -i <ImageFileName> ]
      = Input image File Name

   [ -m <MaskFileName> ]
      = Input mask File Name

   [ -o [AbaqusFileName] ]
      = Output mesh Abaqus filename

   [ -b <BinFileName> ]
      = Material Properties Bins File Name

   [ -s [SoFileName] ]
      = Output mesh spatial object filename

   [ -v [fileName] ]
      = Output mesh VTK filename

   [ -t <abaqusTitle> ]
      = Abaqus File Title

   [ -r [option] ]
      = Resample down size
        With: option (Default = 1)

   [ -os [offset] ]
      = Mesh Index Offset
        With: offset (Default = 0)

   [ -n [numberOfBins] ]
      = Number of Modulus Values Bins
        With: numberOfBins (Default = 0)

   [ -l [binLowerBound] ]
      = Lower Bound Value of Bin
        With: binLowerBound (Default = -1)

   [ -u [binUpperBound] ]
      = Upper Bound Value of Bin
        With: binUpperBound (Default = -1)

   [ -p [value] ]
      = Poisson's Ratio
        With: value (Default = 0.3)



****************************************************************/

int main( int argc, char * argv[] )
{
  itk::Brains2MaskImageIOFactory::RegisterOneFactory( );

  /** Handle the Command line arguments */
  MetaCommand command;

  command.SetOption("ImageFileName","i",false,"Input image File Name");
  command.AddOptionField("ImageFileName","ImageFileName",MetaCommand::STRING,true);

  command.SetOption("MaskFileName","m",false,"Input mask File Name");
  command.AddOptionField("MaskFileName","MaskFileName",MetaCommand::STRING,true);

  command.SetOption("AbaqusFileName","o",false,"Output mesh Abaqus filename");
  command.AddOptionField("AbaqusFileName","AbaqusFileName",MetaCommand::STRING,false, "");

  command.SetOption("BinFileName","b",false,"Material Properties Bins File Name");
  command.AddOptionField("BinFileName","BinFileName",MetaCommand::STRING,true);

  command.SetOption("SoFileName","s",false,"Output mesh spatial object filename");
  command.AddOptionField("SoFileName","SoFileName",MetaCommand::STRING,false, "");

  command.SetOption("VtkFileName","v",false,"Output mesh VTK filename");
  command.AddOptionField("VtkFileName","fileName",MetaCommand::STRING,false, "");

  command.SetOption("AbaqusTitle","t",false,"Abaqus File Title");
  command.AddOptionField("AbaqusTitle","abaqusTitle",MetaCommand::STRING,true);

  command.SetOption("ResampleSize","r",false,"Resample down size");
  command.AddOptionField("ResampleSize","option",MetaCommand::INT,false,"1");

  command.SetOption("MeshIndexOffset","os",false,"Mesh Index Offset");
  command.AddOptionField("MeshIndexOffset","offset",MetaCommand::INT,false,"0");

  command.SetOption("NumberOfBins","n",false,"Number of Modulus Values Bins");
  command.AddOptionField("NumberOfBins","numberOfBins",MetaCommand::INT,false,"0");

  command.SetOption("BinLowerBound","l",false,"Lower Bound Value of Bin");
  command.AddOptionField("BinLowerBound","binLowerBound",MetaCommand::STRING,false,"-1");

  command.SetOption("BinUpperBound","u",false,"Upper Bound Value of Bin");
  command.AddOptionField("BinUpperBound","binUpperBound",MetaCommand::STRING,false,"-1");

  command.SetOption("PoissonRatio","p",false,"Poisson's Ratio");
  command.AddOptionField("PoissonRatio","value",MetaCommand::FLOAT,false,"0.3");

  if ( !command.Parse(argc,argv) )
    {
    return 1;
    }
	
  const std::string ImageFileName(command.GetValueAsString("ImageFileName",
                                                            "ImageFileName"));
  const std::string MaskFileName(command.GetValueAsString("MaskFileName",
                                                            "MaskFileName"));
  const std::string AbaqusFileName(command.GetValueAsString("AbaqusFileName",
                                                            "AbaqusFileName"));
  const std::string BinFileName(command.GetValueAsString("BinFileName",
                                                            "BinFileName"));
  const std::string SoFileName(command.GetValueAsString("SoFileName", 
                                                            "SoFileName"));
  const std::string VtkFileName(command.GetValueAsString("VtkFileName", 
                                                            "fileName"));
  const std::string AbaqusTitle(command.GetValueAsString("AbaqusTitle",
                                                            "abaqusTitle"));
  int numberOfBins = command.GetValueAsInt("NumberOfBins", "numberOfBins");
 
  double binUpperBound = command.GetValueAsFloat("BinUpperBound", 
                                                  "binUpperBound");
  double binLowerBound = command.GetValueAsFloat("BinLowerBound", 
                                                  "binLowerBound");
  int resampleFactor = command.GetValueAsInt("ResampleSize", "option");
  int meshOffset = command.GetValueAsInt("MeshIndexOffset", "offset");
  double poissonRatio = command.GetValueAsFloat("PoissonRatio", "value");



  // Summarize the command line arguements for the user
  std::cout << "Voxel Mesh Parameters" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "\tInput image filename: " << ImageFileName << std::endl;
  std::cout << "\tMask filename: " << MaskFileName << std::endl;
  std::cout << "\tOutput Abaqus filename: " << AbaqusFileName << std::endl;
  std::cout << "\tAbaqus Title: " << AbaqusTitle << std::endl;
  std::cout << "\tOutput Spatial Object filename: " << SoFileName << std::endl;
  std::cout << "\tOutput VTK filename: " << VtkFileName << std::endl;
  std::cout << "\tModulus bin filename: " << BinFileName << std::endl;
  std::cout << "\tNumber of Bins: " << numberOfBins << std::endl;
  std::cout << "\tBin Upper Bound: " << binUpperBound << std::endl;
  std::cout << "\tBin Lower Bound: " << binLowerBound << std::endl;
  std::cout << "\tResample Factor: " << resampleFactor << std::endl;
  std::cout << "\tMesh Offset: " << meshOffset << std::endl;
  std::cout << "\tPoisson Ratio: " << poissonRatio << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;


  
  typedef    signed short    InputPixelType;
  typedef    signed short    MaskPixelType;
  typedef    signed short    OutputPixelType;
  typedef    double          CellPixelType;
  typedef    double          TraitType;
  typedef itk::Image< InputPixelType,  3 >   InputImageType;
  typedef itk::Image< MaskPixelType,   3 >   MaskImageType;
  typedef itk::Image< OutputPixelType, 3 >   OutputImageType;
  typedef InputImageType::PointType          InputImagePointType;
  typedef itk::Array< TraitType >            ArrayType;
  ArrayType binValue;


  /** Read the Input Image */
  typedef itk::ImageFileReader< InputImageType >  ReaderType;
  ReaderType::Pointer imageReader = ReaderType::New( );
  imageReader->SetFileName( ImageFileName.c_str( ) );

  /** Orient the Input Image */
  typedef itk::OrientImageFilter<InputImageType,InputImageType> 
                                                        OrientFilterType;
  OrientFilterType::Pointer OrientImageFilter = OrientFilterType::New( );
  OrientImageFilter->UseImageDirectionOn( );
  //OrientImageFilter->SetDesiredCoordinateOrientationToCoronal( );
  OrientImageFilter->SetDesiredCoordinateOrientation( itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP );
  OrientImageFilter->SetInput( imageReader->GetOutput( ) );
  OrientImageFilter->Update( );

  InputImageType::SpacingType imageSpacing =
    OrientImageFilter->GetOutput( )->GetSpacing( );
  InputImageType::SizeType imageSize =
    OrientImageFilter->GetOutput( )->GetLargestPossibleRegion( ).GetSize( );

  /** Reading the Input Mask */
  ReaderType::Pointer maskReader = ReaderType::New( );
  OrientFilterType::Pointer OrientMaskFilter = OrientFilterType::New( );

  if ( ! MaskFileName.empty() )
    {
    maskReader->SetFileName( MaskFileName.c_str( ) );
    OrientMaskFilter->UseImageDirectionOn( );
    //OrientMaskFilter->SetDesiredCoordinateOrientationToCoronal( );
    OrientMaskFilter->SetDesiredCoordinateOrientation( itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP );
    OrientMaskFilter->SetInput( maskReader->GetOutput( ) );
    OrientMaskFilter->Update( );

    /** Check the Image/Mask Spacing and Resolution - Currently they must match */

    MaskImageType::SpacingType maskSpacing = 
      OrientMaskFilter->GetOutput( )->GetSpacing( );
    MaskImageType::SizeType maskSize =
      OrientMaskFilter->GetOutput( )->GetLargestPossibleRegion( ).GetSize( );

    for ( int i = 0; i < 3; i++ )
      {
      if ( maskSize[i] != imageSize[i] )
        {
        std::cerr << "Image size (" << imageSize[i] <<
          ") does not match mask (" << maskSize[i] << 
          ")" << std::endl;
        exit(1);
        }

      if ( fabs(static_cast<double>( maskSpacing[i]-imageSpacing[i] )) > 0.001 )
        {
        std::cerr << "Image spacing (" << imageSpacing[i] <<
          ") does not match mask (" << maskSpacing[i] << 
          "). Tolerance is currently 0.001." << std::endl;
        exit(1);
        }
      }
    }


  /** Resample the Image and Mask - Used to control mesh density */

  InputImageType::SpacingType resampledSpacing;
  InputImageType::SizeType resampledSize;
  typedef itk::ResampleImageFilter< MaskImageType, MaskImageType > 
    ResampleMaskType;
  ResampleMaskType::Pointer resampleMaskFilter = ResampleMaskType::New( );
  for ( int i = 0; i < 3; i++ )
    {
    resampledSpacing[i] = imageSpacing[i] * 
      static_cast<float>( resampleFactor );
    resampledSize[i] = static_cast<unsigned int>(
      static_cast<float>( imageSize[i] )/
      static_cast<float>( resampleFactor ));
    if ( imageSize[i] % resampleFactor > 0 ) { resampledSize[i]++ ; }
    }

  typedef double  CoordRepType;
  typedef itk::LinearInterpolateImageFunction< InputImageType,CoordRepType >
                                                          InterpolatorType;

  InterpolatorType::Pointer imageInterpolator = InterpolatorType::New( );
  imageInterpolator->SetInputImage( OrientImageFilter->GetOutput( ) );

  typedef itk::IdentityTransform< double >  IdentityTransformType;
  IdentityTransformType::Pointer imageIdentityTransform = 
                                    IdentityTransformType::New( );

  typedef itk::ResampleImageFilter< InputImageType, InputImageType> 
                                    ResampleImageType;
  ResampleImageType::Pointer resampleImageFilter = 
                                    ResampleImageType::New( );
  resampleImageFilter->SetInput( OrientImageFilter->GetOutput( ) );
  resampleImageFilter->SetTransform( imageIdentityTransform );
  resampleImageFilter->SetInterpolator( imageInterpolator );
  resampleImageFilter->SetOutputSpacing( resampledSpacing );
  resampleImageFilter->SetOutputOrigin( 
                              OrientImageFilter->GetOutput( )->GetOrigin( ) );
  resampleImageFilter->SetSize( resampledSize );
  resampleImageFilter->Update( );

  if ( ! MaskFileName.empty() )
    {
    InterpolatorType::Pointer maskInterpolator = InterpolatorType::New( );
    maskInterpolator->SetInputImage( OrientMaskFilter->GetOutput( ) );

    IdentityTransformType::Pointer maskIdentityTransform = 
      IdentityTransformType::New( );

    resampleMaskFilter->SetInput( OrientMaskFilter->GetOutput( ) );
    resampleMaskFilter->SetTransform( maskIdentityTransform );
    resampleMaskFilter->SetInterpolator( maskInterpolator );
    resampleMaskFilter->SetOutputSpacing( resampledSpacing );
    resampleMaskFilter->SetOutputOrigin( 
              OrientMaskFilter->GetOutput( )->GetOrigin( ) );
    resampleMaskFilter->SetSize( resampledSize );
    resampleMaskFilter->Update( );
    }

  /* This is a temporary fix to handle shifts in the origin due to reorientation */ 
  /*     Note: This should be fixed in the future */
  InputImageType::Pointer fixedImage = resampleImageFilter->GetOutput( );
  InputImagePointType  fixedOrigin;
  fixedOrigin.Fill(0);
  fixedImage->SetOrigin(fixedOrigin);

  /** Now Generate the Mesh */

  typedef itk::DefaultStaticMeshTraits< unsigned long , 3, 3, double,
                                        double, double > MeshTraitType;
  typedef itk::Mesh< unsigned long, 3, MeshTraitType > MeshType;  
  typedef itk::ImageToHexahedronMeshFilter< InputImageType, MaskImageType,
                                            MeshType > ImageToMeshFilterType;

  ImageToMeshFilterType::Pointer imageToHexMeshFilter = 
                                                ImageToMeshFilterType::New( );

  imageToHexMeshFilter->SetInput( fixedImage );
  if ( MaskFileName != "")
    {
    InputImageType::Pointer fixedMask = resampleImageFilter->GetOutput( );
    fixedMask->SetOrigin( fixedOrigin );
    imageToHexMeshFilter->SetMaskImage( fixedMask );  
    }
  imageToHexMeshFilter->SetMeshIndexOffset( meshOffset );
  imageToHexMeshFilter->SetComputeMeshPropertiesOff( );
  imageToHexMeshFilter->SetUseCtPropertiesOn( );
  imageToHexMeshFilter->SetComputeMeshNodeNumberingOff( );  
  imageToHexMeshFilter->Update( );

  MeshType::Pointer finalMesh = imageToHexMeshFilter->GetOutput( );

  typedef itk::RebinHexahedronMeshTraitDataInPlaceFilter<MeshType, MeshType> 
    RebinHexahedronMeshTraitDataInPlaceFilterType;
  RebinHexahedronMeshTraitDataInPlaceFilterType:: 
    Pointer rebinHexahedronMeshTraitDataInPlaceFilter = 
    RebinHexahedronMeshTraitDataInPlaceFilterType::New( );

  /** Re-Bin the Material Properties of Voxels */
  if ( numberOfBins != 0 || ! BinFileName.empty() )
    {
    rebinHexahedronMeshTraitDataInPlaceFilter->SetInput( finalMesh );
    if ( ! BinFileName.empty() )
      {
      typedef itk::ReadHistogramBinValuesFilter<double> ReadHistogramBinValuesFilterType;
      ReadHistogramBinValuesFilterType::Pointer readBinValuesFilter =
                                     ReadHistogramBinValuesFilterType::New( );
      readBinValuesFilter->SetBinFileName( BinFileName.c_str( ) );
      readBinValuesFilter->Update(  );  
      numberOfBins = readBinValuesFilter->GetNumberOfBins( );
      
      rebinHexahedronMeshTraitDataInPlaceFilter->SetNumberOfHistogramBins( 
                                                                numberOfBins);
      rebinHexahedronMeshTraitDataInPlaceFilter->SetHistogramBins( 
                                   readBinValuesFilter->GetHistogramBins( ) );
      }
    else
      {
      rebinHexahedronMeshTraitDataInPlaceFilter->SetNumberOfHistogramBins( 
                                                               numberOfBins );
      if (binLowerBound != -1)
        {
        rebinHexahedronMeshTraitDataInPlaceFilter->SetComputeMinBin( false );
        rebinHexahedronMeshTraitDataInPlaceFilter->SetBinLowerBound( 
                                                              binLowerBound );
        }
      if (binUpperBound != -1)
        {
        rebinHexahedronMeshTraitDataInPlaceFilter->SetComputeMaxBin( false );
        rebinHexahedronMeshTraitDataInPlaceFilter->SetBinUpperBound( 
                                                              binUpperBound );
        }
      }
    rebinHexahedronMeshTraitDataInPlaceFilter->Update( );
    }
    
    
  /** Write out Mesh as an Abaqus Input Deck */
  if ( ! AbaqusFileName.empty()  )
    {
    time_t cseconds = time( NULL );
    char timeStr [9];
    

    std::string header = "";
    header += "****************************************************************************\n"; 
    header += "**                                                                        **\n"; 
    header += "**                                                                        **\n"; 
    header += "**                                                                        **\n"; 
    header += "**  Voxel-Based FE Model                                                  **\n";
    header += "**  Created by VoxelMesh                                                  **\n";  
    header += "**  Center for Computer Aided Design                                      **\n"; 
    header += "**  The University of Iowa                                                **\n"; 
    header += "**  Iowa City, IA  52242                                                  **\n"; 
    header += "**                                                                        **\n"; 
    header += "**                                                                        **\n"; 
    header += "**  Project Title: " + AbaqusTitle;           
    header += "\n**  Date: ";
    header += ctime( &cseconds );   
    header += "\n**                                                                      **\n"; 
    header += "**                                                                        **\n"; 
    header += "****************************************************************************";

    typedef itk::AbaqusMeshFileWriter< MeshType > MeshWriterType;  
    MeshWriterType::Pointer AbqWriter = MeshWriterType::New( );
    AbqWriter->SetFileName( AbaqusFileName.c_str() );
    AbqWriter->SetTitle( AbaqusTitle.c_str() );
    AbqWriter->SetAbaqusHeader( header.c_str() );
    AbqWriter->SetInput( finalMesh );
    AbqWriter->SetMeshIndexOffset( meshOffset );
    AbqWriter->SetPoissonRatio( poissonRatio );
    if ( numberOfBins != 0 || !BinFileName.empty())
      {
      AbqWriter->SetRebinningOn( );  
      AbqWriter->SetNumberOfHistogramBins( numberOfBins );
      AbqWriter->SetHistogramBins( 
              rebinHexahedronMeshTraitDataInPlaceFilter->GetHistogramBins( ));
      }
    AbqWriter->Update( );
    }


  /** Write the Mesh to a Spatial Object */
  if ( ! SoFileName.empty()  )
    {
    typedef itk::MeshSpatialObject< MeshType > MeshSpatialObjectType;  
    MeshSpatialObjectType::Pointer meshSpatialObject = 
                                        MeshSpatialObjectType::New();
    meshSpatialObject->SetMesh( finalMesh );

    typedef itk::SpatialObjectWriter<3, unsigned long, MeshTraitType>WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( meshSpatialObject );
    writer->SetFileName( SoFileName.c_str() );
    
    try
      {
      writer->Update( );
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << ex << std::endl;
      }
    }

  if ( ! VtkFileName.empty()  )
    {
    /* Transform the Mesh to VTK and write out */
    typedef itkMeshTovtkUnstructuredGrid<MeshType> MeshToVtkType;
    MeshToVtkType *toVtkFilter = new MeshToVtkType;
    toVtkFilter->SetInput( finalMesh );
    vtkUnstructuredGrid *hexGrid = toVtkFilter->GetOutput();

    vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
    writer->SetInput( hexGrid );
    writer->SetFileName( VtkFileName.c_str() );
    writer->SetFileTypeToASCII( );
    writer->Update();
    }
  
  return EXIT_SUCCESS;
}
