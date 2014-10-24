/*=========================================================================
 
  Program:   Insight Segmentation & Registration Toolkit 
  Module:    $RCSfile: testTriLineIntersect.cxx,v $
  Language:  C++
  Date:      $Date: 2006/09/07 13:28:53 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "TriLineIntersect.h" 
#include "itkVector.h" 



int main(int argc, char *argv[])
{
  
  Triangle myTri;
  Ray myLine;
  Point pt;
  int result;
  bool test;
  
  myTri.V1[0] = -1.0; myTri.V1[1] = -1.0;  myTri.V1[2] = 1.0;
  myTri.V0[0] =  1.0; myTri.V0[1] =  1.0;  myTri.V0[2] = 1.0;
  myTri.V2[0] = -1.0; myTri.V2[1] =  1.0;  myTri.V2[2] = 1.0;

  myLine.P0[0] = 0.0; myLine.P0[1] = 0.0; myLine.P0[2] = 0.0;
  myLine.P1[0] = 0.0; myLine.P1[1] = 0.1; myLine.P1[2] = 0.7;
//  result = intersect_RayTriangle(myLine, myTri, &pt);
//  std::cout << "Result " << result << " - Point: " << pt << std::endl;

  test = LineFacet(myLine, myTri, &pt);
  std::cout << "Test " << test << " - Point: " << pt << std::endl;



  myLine.P1[0] = -0.5; myLine.P1[1] = 0.5; myLine.P1[2] = 1.0;
//  result = intersect_RayTriangle(myLine, myTri, &pt);
//  std::cout << "Result " << result << " - Point: " << pt << std::endl;
  test = LineFacet(myLine, myTri, &pt);
  std::cout << "Test " << test << " - Point: " << pt << std::endl;


  myLine.P1[0] = 0.0; myLine.P1[1] = 0.0; myLine.P1[2] = -1.0;
//  result = intersect_RayTriangle(myLine, myTri, &pt);
//  std::cout << "Result " << result << " - Point: " << pt << std::endl;
  test = LineFacet(myLine, myTri, &pt);
  std::cout << "Test " << test << " - Point: " << pt << std::endl;


  return 0;
}


