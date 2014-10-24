/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: SimulatedAnnealing.cxx,v $
Language:  C++
Date:      $Date: 2006/09/07 13:28:53 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <math.h>

#include <vtkIdList.h>
#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkPointSet.h>
#include <vtkPoints.h>
#include <vtkExtractEdges.h>
#include <vtkCellArray.h>


#include <itkMersenneTwisterRandomVariateGenerator.h>

#include "SimulatedAnnealing.h"


//initialize temperature (reduction factor e.g. 0.75 or 0.5)
void RefineMeshSimulatedAnnealing(vtkUnstructuredGrid *mesh, 
                                  double temperature, 
                                  int nLimit, 
                                  int numberOfTemps, 
                                  int randomSeed)
{
  const unsigned int numberOfMeshPoints = mesh->GetNumberOfPoints(); 
  const unsigned int numberOfMeshCells = mesh->GetNumberOfCells( );

  // Compute the means 
  double meanLength[3];
  computeAverageLength(mesh, meanLength);


  double factor = 1.0;
  double lengthXScale = meanLength[0] / 2;
  double lengthYScale = meanLength[1] / 2;
  double lengthZScale = meanLength[2] / 2;


  typedef itk::Statistics::MersenneTwisterRandomVariateGenerator RandomGeneratorType;
  RandomGeneratorType::Pointer	randomGenerator = RandomGeneratorType::New();
  randomGenerator->Initialize( randomSeed );

  // VAM - Complete this Section and Test
  vtkIdList *cellPoints = vtkIdList::New();
  
  for (int n=0; n<numberOfTemps; n++)
    {
    factor *= temperature;
    for (int i=0; i<nLimit; i++)
      {
      for (int j=0; j<numberOfMeshPoints; j++)
        {
        // Get number of neighbors
        cellPoints->Initialize();
        mesh->GetPointCells(j, cellPoints);
        unsigned int numberOfPointCells = cellPoints->GetNumberOfIds();
        unsigned int numMatchEdges = 0;
        double dx = 0.0;
        double dy = 0.0;
        double dz = 0.0;

        if (numberOfPointCells == 8)
          {
          vtkFloatingPointType NodePos[3]; 
          vtkFloatingPointType NeighPos[3];        
          mesh->GetPoint(j, NodePos);
          vtkFloatingPointType edgePoint[8][3];
          int numMatchEdges = 0;

          for (int k=0; k<8; k++)
            {
            // VAM - Get neighbors
            vtkCell *currentCell = mesh->GetCell( cellPoints->GetId(k) );
            int numberOfEdges = currentCell->GetNumberOfEdges();
            vtkCell *currentEdge = currentCell->GetEdge( k );
            vtkPoints* edgePoints = currentEdge->GetPoints();

            vtkFloatingPointType p1[3];
            vtkFloatingPointType p2[3];
            edgePoints->GetPoint(0, p1);
            edgePoints->GetPoint(1, p2);

            bool found = false;
            if ((p1[0] == NodePos[0]) && (p1[1] == NodePos[1]) && (p1[2] == NodePos[2]))
              {        
              NeighPos[0] = p2[0]; NeighPos[1] = p2[1]; NeighPos[2] = p2[2];
              found = true;
              }
            else if ((p2[0] == NodePos[0]) && (p2[1] == NodePos[1]) && (p2[2] == NodePos[2]))
              {
              NeighPos[0] = p1[0]; NeighPos[1] = p1[1]; NeighPos[2] = p1[2];
              found = true;
              }
            
            if ( found )
              {
              bool edgeExists = false;
              for (int l=0; l<numMatchEdges;l++)
                {
                if ((edgePoint[l][0] == NeighPos[0]) && 
                    (edgePoint[l][1] == NeighPos[1]) &&
                    (edgePoint[l][2] == NeighPos[2]))
                  {
                  edgeExists = true;
                  }
                }
              if ( ! edgeExists )
                {
                edgePoint[numMatchEdges][0] = NeighPos[0];
                edgePoint[numMatchEdges][1] = NeighPos[1];
                edgePoint[numMatchEdges][2] = NeighPos[2];
                numMatchEdges++;

                dx += NodePos[0] - NeighPos[0]; 
                dy += NodePos[1] - NeighPos[1]; 
                dz += NodePos[2] - NeighPos[2];
                }
              }
            }
          if ( numMatchEdges != 6 )
           {
           std::cerr << "Warning: Expected 6 edges - found " << numMatchEdges << std::endl;
           }
          dx /= 2.0 * static_cast<double>(numMatchEdges);
          dy /= 2.0 * static_cast<double>(numMatchEdges);
          dz /= 2.0 * static_cast<double>(numMatchEdges); 
  
          /* VAM - Conceptual Issue 
           *  Should the updated position be based on random amounts 
           *  or should it simply adjust to a better estimate
           */
          // Evaluate X Position - Adjustment
          double rdx = randomGenerator->GetVariateWithOpenRange() * lengthXScale;
          double xDelta = EnergyProbability(dx, rdx, factor, lengthXScale);
          double xRandom = randomGenerator->GetVariateWithOpenRange( );
          if ( xDelta < xRandom )
            {                 
            NodePos[0] += dx;
            }

          // Evaluate Y Position - Adjustment
          double rdy = randomGenerator->GetVariateWithOpenRange() * lengthYScale;
          double yDelta = EnergyProbability(dy, rdy, factor, lengthYScale);
          double yRandom = randomGenerator->GetVariateWithOpenRange( );
          if ( yDelta < yRandom )
            {                 
            NodePos[1] += rdy;
            }

          // Evaluate Z Position - Adjustment
          double rdz = randomGenerator->GetVariateWithOpenRange() * lengthZScale;
          double zDelta = EnergyProbability(dz, rdz, factor, lengthZScale);
          double zRandom = randomGenerator->GetVariateWithOpenRange( );
          if ( zDelta < zRandom )
            {                 
            NodePos[2] += rdz;
            }
          // VAM - Update Node Position in the mesh
          vtkPoints *meshPoints = mesh->GetPoints();
          meshPoints->SetPoint(j, NodePos); 
          mesh->SetPoints( meshPoints );
          }
        }
      }
    }

  return;
}



double EnergyProbability(double dx, double rdx, double temperature, double meanLength)
{
  double delta = dx - rdx;
  double deltaMag = fabs( delta )/meanLength;
  double prob = exp (-deltaMag/temperature);
  return prob;
}

void computeAverageLength( vtkUnstructuredGrid *mesh, double meanLength[3] )
{
  vtkExtractEdges *edgeFilter = vtkExtractEdges::New();
    edgeFilter->SetInput( mesh );
    edgeFilter->Update( );

  vtkCellArray *edges = edgeFilter->GetOutput( )->GetLines();
  
  double distanceX = 0.0;
  double distanceY = 0.0;
  double distanceZ = 0.0;

  int numberOfEdges = edges->GetNumberOfCells( );
  vtkIdList *edgePoints = vtkIdList::New();

  for (int i=0;i<numberOfEdges;i++)
    {
    int npts;
    edgePoints->Initialize();
    edgeFilter->GetOutput( )->GetCellPoints(i, edgePoints);
    double pt1[3];
    edgeFilter->GetOutput( )->GetPoint(edgePoints->GetId( 0 ), pt1);
    double pt2[3];
    edgeFilter->GetOutput( )->GetPoint(edgePoints->GetId( 1 ), pt2);
    distanceX += fabs(pt1[0] - pt2[0]);
    distanceY += fabs(pt1[1] - pt2[1]);
    distanceZ += fabs(pt1[2] - pt2[2]);
    }
  meanLength[0] = distanceX / static_cast<double>(numberOfEdges); 
  meanLength[1] = distanceY / static_cast<double>(numberOfEdges); 
  meanLength[2] = distanceZ / static_cast<double>(numberOfEdges); 

  return;
}
