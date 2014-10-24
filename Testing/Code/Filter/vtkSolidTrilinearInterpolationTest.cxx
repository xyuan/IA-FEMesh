/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkSolidTrilinearInterpolationTest.cxx,v $
Language:  C++
Date:      $Date: 2007/05/17 17:09:46 $
Version:   $Revision: 1.2 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "vtkMimxSolidTrilinearInterpolation.h"

#include "vtkPoints.h"
#include "vtkPointSet.h"


int main(int argc, char * argv [])
{

  vtkPoints *gridPoints = vtkPoints::New();
  double meshPt[3];
  gridPoints->SetNumberOfPoints(4 * 8 * 16);  
  for (int i=0;i<4;i++)
    {
    for (int j=0;j<8;j++)
      {
      for (int k=0;k<16;k++)
        {
        meshPt[0] = static_cast<double> (i);
        meshPt[1] = static_cast<double> (j);
        meshPt[2] = static_cast<double> (k);
        gridPoints->SetPoint(i*8*16+j*16+k, meshPt);
        }
      }
    }

  vtkPointSet *gridSet = vtkPointSet::New();
  gridSet->SetPoints(gridPoints);

  vtkMimxSolidTrilinearInterpolation *interpFilter = vtkMimxSolidTrilinearInterpolation::New();
  interpFilter->SetInput( gridSet );
  interpFilter->SetKDiv( 4 );
  interpFilter->SetJDiv( 8 );
  interpFilter->SetIDiv( 16 );
  interpFilter->Update( );

  /* Check the Output */
  vtkPointSet *interpSet = interpFilter->GetOutput( );
  double interpPt[3];
  int count = 0;
  for (int i=0;i<4;i++)
    {
    for (int j=0;j<8;j++)
      {
      for (int k=0;k<16;k++)
        {
        gridSet->GetPoint(i*8*16+j*16+k, meshPt);
        interpSet->GetPoint(i*8*16+j*16+k, interpPt);
        double distance = (meshPt[0] - interpPt[0]) *  (meshPt[0] - interpPt[0]) +
                          (meshPt[1] - interpPt[1]) *  (meshPt[1] - interpPt[1]) +
                          (meshPt[2] - interpPt[2]) *  (meshPt[2] - interpPt[2]);
        if (sqrt(distance) > 0.001) count++;
        }
      }
    }

  if (count > 0)
    {
    std::cout << "Error: Found  " << count; 
    std::cout << " invalid points using trilinear solid interpolation" << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
