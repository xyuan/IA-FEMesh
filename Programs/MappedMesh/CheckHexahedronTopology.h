/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: CheckHexahedronTopology.h,v $
Language:  C++
Date:      $Date: 2006/09/24 19:44:52 $
Version:   $Revision: 1.1 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __CheckHexahedronTopology_h
#define __CheckHexahedronTopology_h

#include <vtkMath.h>


bool CheckHexahedronTopology( double h1[3], double h2[3], double h3[3], double h4[3],
                              double h5[3], double h6[3], double h7[3], double h8[3]);

#endif  // __CheckHexahedronTopology_h
