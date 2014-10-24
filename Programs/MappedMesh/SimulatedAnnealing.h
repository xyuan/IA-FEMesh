/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: SimulatedAnnealing.h,v $
Language:  C++
Date:      $Date: 2006/09/07 13:28:53 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __SimulatedAnnealing_h
#define __SimulatedAnnealing_h

#include <vtkUnstructuredGrid.h>


double EnergyProbability(double dx, double rdx, double temperature, double meanLength);
void computeAverageLength( vtkUnstructuredGrid *mesh, double meanLength[3] );
void RefineMeshSimulatedAnnealing(vtkUnstructuredGrid *mesh, 
                                  double temperature, 
                                  int nLimit, 
                                  int numberOfTemps, 
                                  int randomSeed);



#endif /* __SimulatedAnnealing_h */
