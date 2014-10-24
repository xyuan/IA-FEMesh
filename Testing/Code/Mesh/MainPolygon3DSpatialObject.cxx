/*=========================================================================

  Program:   IaFeMesh
  Module:    $RCSfile: MainPolygon3DSpatialObject.cxx,v $
  Language:  C++
  Date:      $Date: 2006/10/15 23:56:43 $
  Version:   $Revision: 1.2 $

  Copyright (c) Nicole Grosland. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkPolygon3DSpatialObject.h"

int main(int argc, char * argv [])
{
  if( argc != 2 )
    {
    std::cerr << argv[0]
              << " curve"
              << std::endl;
    return EXIT_FAILURE;
    }

	
	typedef itk::Polygon3DSpatialObject<3>	CurveType3D;	
	CurveType3D::Pointer  Curve1 = CurveType3D::New();

	std::ifstream FileInput1;
  try 
    {
    FileInput1.open(argv[1], std::ios::in);
    }
  catch (...)
    {
    std::cout << "Error: Failed to open Curve 1 file " << argv[1] << std::endl;
    return EXIT_FAILURE;
    }

	itk::Point<double,3> pointtype;

	do {
		if(FileInput1 >> pointtype[0]>>pointtype[1]>>pointtype[2])
			Curve1->AddPoint(pointtype);
	} while(!FileInput1.eof());
  FileInput1.close( );


	CurveType3D::Pointer  CurvePointer3DSeed = CurveType3D::New();
	CurvePointer3DSeed->MeshSeed(2.0,Curve1->GetPoints(),0.1);

  return EXIT_SUCCESS;
}
