/*=========================================================================
 
   Program:   Insight Segmentation & Registration Toolkit
   Module:    $RCSfile: TriLineIntersect.h,v $
   Language:  C++
   Date:      $Date: 2006/09/07 13:28:53 $
   Version:   $Revision: 1.1 $
 
   Copyright (c) Insight Software Consortium. All rights reserved.
   See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
 
      This software is distributed WITHOUT ANY WARRANTY; without even 
      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
      PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __TriLineIntersect_h
#define __TriLineIntersect_h

#include <itkVector.h>

#define SMALL_NUM  0.00000001 // anything that avoids division overflow
#define PI 3.141592653589793

class Ray
{
public:
  itk::Vector<double, 3> P0;
  itk::Vector<double, 3> P1;
};

class Triangle
{
public:
  itk::Vector<double, 3> V0;
  itk::Vector<double, 3> V1;
  itk::Vector<double, 3> V2;
};


typedef itk::Vector<double, 3> Point;
typedef itk::Vector<double, 3> Vector;



// dot product (3D) which allows vector operations in arguments
#define dotproduct(u,v)   ((u)[0] * (v)[0] + (u)[1] * (v)[1] + (u)[2] * (v)[2])

int intersect_RayTriangle( Ray R, Triangle T, Point* I );
bool LineFacet(Ray R, Triangle T, Point *p);

#endif

