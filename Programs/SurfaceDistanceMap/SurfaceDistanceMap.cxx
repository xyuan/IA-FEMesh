/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: SurfaceDistanceMap.cxx,v $
  Language:  C++
  Date:      $Date: 2008/03/18 01:38:38 $
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

#include <string>

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkOBBTree.h>
#include <vtkPoints.h>
#include <vtkPointSet.h>
#include <vtkPolyDataNormals.h>
#include <vtkPointLocator.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkMath.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>

#include <metaCommand.h>

#include <vtkMimxComputeNormalsFromPolydataFilter.h>


#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif


double EucledianDistance (double point1[3], double point2[3]);
bool IntersectPointNormalWithSurface(double *pointLocation, double *pointNormal, double *distance,
                                     vtkOBBTree *obb, double *projectedPoint);




int main( int argc, char * argv[] )
{

  MetaCommand command;

  command.SetOption("SourceSurface","s",false,"Source Surface File");
  command.AddOptionField("SourceSurface","filename",MetaCommand::STRING,true);

  command.SetOption("TargetSurface","t",false,"Target Surface File");
  command.AddOptionField("TargetSurface","filename",MetaCommand::STRING,true);

  command.SetOption("DistanceMap","o",false,"Distance Map Surface Filename");
  command.AddOptionField("DistanceMap","filename",MetaCommand::STRING,true);

  command.SetOption("DataFile","d",false,"Distance Map Data Filename");
  command.AddOptionField("DataFile","filename",MetaCommand::STRING,true);
  
  if (!command.Parse(argc,argv))
    {
    return 1;
    }
    
    
  const std::string SourceSurfaceFilename (command.GetValueAsString("SourceSurface","filename"));
  const std::string TargetSurfaceFilename (command.GetValueAsString("TargetSurface","filename"));
  const std::string OutputSurfaceFilename (command.GetValueAsString("DistanceMap","filename"));  
  const std::string dataFilename (command.GetValueAsString("DataFile","filename"));  
      
  std::cout << "Source Surface: " <<  SourceSurfaceFilename << std::endl; 
  std::cout << "Target Surface: " <<  TargetSurfaceFilename << std::endl; 
  std::cout << "Output Image: " <<  OutputSurfaceFilename << std::endl; 

  
  /*** Read the Source Mesh For Distance Map ***/
  vtkPolyData *sourcePolyData;
  if ( (strstr(SourceSurfaceFilename.c_str(), ".stl") != NULL) || 
       (strstr(SourceSurfaceFilename.c_str(), ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName(SourceSurfaceFilename.c_str());
    fixedReader->Update( );
    sourcePolyData = fixedReader->GetOutput();
    }
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName( SourceSurfaceFilename.c_str() );
    polyReader->Update( );	
    sourcePolyData = polyReader->GetOutput();
    }

  /*** Read the Target Mesh For Distance Map ***/
  vtkPolyData *targetPolyData;
  if ( (strstr(TargetSurfaceFilename.c_str(), ".stl") != NULL) || 
       (strstr(TargetSurfaceFilename.c_str(), ".STL")!= NULL) )
    {
    vtkSTLReader *fixedReader = vtkSTLReader::New();
    fixedReader->SetFileName( TargetSurfaceFilename.c_str() );
    fixedReader->Update( );
    targetPolyData = fixedReader->GetOutput();
    }
  else
    {
    vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New();
    polyReader->SetFileName( TargetSurfaceFilename.c_str() );
    polyReader->Update( );	
    targetPolyData = polyReader->GetOutput();
    }
  
  vtkMimxComputeNormalsFromPolydataFilter *computeNormals =  vtkMimxComputeNormalsFromPolydataFilter::New();
  computeNormals->SetInput( sourcePolyData );
  computeNormals->Update();
  vtkPointSet *normalsPointSet = computeNormals->GetOutput( );
  vtkPoints *finalNormals = normalsPointSet->GetPoints();

  /* Point Locator for Fast Closest Point Calculations */
  vtkPointLocator *PLocator = vtkPointLocator::New();
  PLocator->SetDataSet( targetPolyData );
  PLocator->Update();

  /* Point Locator for Fast Normal Intersection */
  vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet( targetPolyData );
  obb->SetMaxLevel( 999999 );
  obb->SetNumberOfCellsPerBucket( 999999 );
  obb->SetTolerance( 0.000001 );
  obb->BuildLocator( );

  int numberOfSourcePoints = sourcePolyData->GetNumberOfPoints();

  /* Float Array to hold distance Map */
  vtkDoubleArray *distanceMap = vtkDoubleArray::New();
  distanceMap->SetName("Normal-Distance");
  distanceMap->SetNumberOfComponents( 1 );
  distanceMap->SetNumberOfTuples( numberOfSourcePoints );
  distanceMap->Allocate( numberOfSourcePoints );

  vtkDoubleArray *closestPointDistanceMap = vtkDoubleArray::New();
  closestPointDistanceMap->SetName("Closest-Point-Distance");
  closestPointDistanceMap->SetNumberOfComponents( 1 );
  closestPointDistanceMap->SetNumberOfTuples( numberOfSourcePoints );
  closestPointDistanceMap->Allocate( numberOfSourcePoints );
  
  bool writeFieldDataCsv = false;
  ofstream out; 

  if (dataFilename.length() > 0 )
    {
    writeFieldDataCsv = true;
    out.open( dataFilename.c_str() ); 
    out << "Normal Distance, Closest Point Distance" << std::endl;
    }
    

  double sumNormalDistance = 0.0;
  double maxNormalDistance = 0.0;
  double sumCPDistance = 0.0;
  double maxCPDistance = 0.0;
  double sumCPDistance2 = 0.0;

  for (unsigned int i=0;i<numberOfSourcePoints; i++)
    {
    vtkFloatingPointType sourcePoint[3];
    vtkFloatingPointType targetPoint[3];
    vtkFloatingPointType pointNormal[3];

    sourcePolyData->GetPoint(i, sourcePoint);
    finalNormals->GetPoint(i , pointNormal);

    vtkIdType ptId = PLocator->FindClosestPoint( sourcePoint ); // Getting closest point on the surface
    targetPolyData->GetPoint( ptId, targetPoint );
    double distanceCP = EucledianDistance( sourcePoint, targetPoint );
    closestPointDistanceMap->InsertNextValue( distanceCP );
    sumCPDistance += distanceCP;
    if ( maxCPDistance < distanceCP ) maxCPDistance = distanceCP;
    sumCPDistance2 += distanceCP * distanceCP;

    double distanceNormal = distanceCP;
    IntersectPointNormalWithSurface(sourcePoint, pointNormal, &distanceNormal, obb, targetPoint);
    sumNormalDistance += distanceNormal;
    if ( maxNormalDistance < distanceNormal ) maxNormalDistance = distanceNormal;

    distanceMap->InsertNextValue( distanceNormal );

    if ( writeFieldDataCsv )
      {
      out << distanceNormal << ", " << distanceCP << std::endl;
      }
    }
  if ( writeFieldDataCsv )
    {
    out.close(); 
    }

  //vtkFieldData *fieldData = vtkFieldData::New();
  //fieldData->AddArray( distanceMap );

  sourcePolyData->GetPointData( )->AddArray( distanceMap );
  sourcePolyData->GetPointData( )->AddArray( closestPointDistanceMap );
  sourcePolyData->GetPointData()->SetActiveAttribute( "Normal-Distance", vtkDataSetAttributes::SCALARS );

 
  /* Write out the Distance Map Image */
  vtkPolyDataWriter *polyWriter = vtkPolyDataWriter::New();
  polyWriter->SetFileName( OutputSurfaceFilename.c_str() );
  polyWriter->SetInput( sourcePolyData );
  polyWriter->Update( );	

  std::cout << "Average Normal Distance:        " << sumNormalDistance/static_cast<double>(numberOfSourcePoints) << std::endl;
  std::cout << "Maximum Normal Distance:        " << maxNormalDistance << std::endl;
  std::cout << "Average Closest Point Distance: " << sumCPDistance/static_cast<double>(numberOfSourcePoints) << std::endl;
  std::cout << "Maximum Closest Point Distance: " << maxCPDistance << std::endl;
  std::cout << "Average Closest Point Distance2: " << sumCPDistance2/static_cast<double>(numberOfSourcePoints) << std::endl;

  return 0;
}

double EucledianDistance (vtkFloatingPointType point1[3], vtkFloatingPointType point2[3])
{
  double d = sqrt   ((point1[0] - point2[0]) * (point1[0] - point2[0]) +
                     (point1[1] - point2[1]) * (point1[1] - point2[1]) +
                     (point1[2] - point2[2]) * (point1[2] - point2[2]));

  return d;
}


bool IntersectPointNormalWithSurface(double *pointLocation, double *pointNormal, double *distance,
                                     vtkOBBTree *obb, double *projectedPoint)
{
  double a0[3];
  double a1[3];


  a0[0] = pointLocation[0] - 2 * (*distance) * pointNormal[0];
  a0[1] = pointLocation[1] - 2 * (*distance) * pointNormal[1];
  a0[2] = pointLocation[2] - 2 * (*distance) * pointNormal[2];

  a1[0] = pointLocation[0] + 2 * (*distance) * pointNormal[0];
  a1[1] = pointLocation[1] + 2 * (*distance) * pointNormal[1];
  a1[2] = pointLocation[2] + 2 * (*distance) * pointNormal[2];


  vtkIdList *intersectionIds = vtkIdList::New();
  vtkPoints *intersectionPoints = vtkPoints::New();
  vtkMath *math = vtkMath::New(); 

  int lineIntersects = obb->IntersectWithLine(a0, a1, intersectionPoints, intersectionIds);
  if ( lineIntersects != 0 )
  {
    double minDistance = 10e20;
    vtkIdType minPointId = 0;
    for (int i=0;i<intersectionPoints->GetNumberOfPoints();i++)
    {
      double currentDistance = math->Distance2BetweenPoints( pointLocation, intersectionPoints->GetPoint(i) ); 
      if ( currentDistance < minDistance)
      {
        minDistance = currentDistance;
        minPointId = i;
      }
    }
    
    projectedPoint[0] = intersectionPoints->GetPoint(minPointId)[0];
    projectedPoint[1] = intersectionPoints->GetPoint(minPointId)[1];
    projectedPoint[2] = intersectionPoints->GetPoint(minPointId)[2];

    *distance = minDistance;
    return true;
  }
  else
  {
    return false;
  }
  
  
}


