#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif


#include "CheckHexahedronTopology.h"



bool CheckHexahedronTopology( double h1[3], double h2[3], double h3[3], double h4[3],
                              double h5[3], double h6[3], double h7[3], double h8[3])
{

  double edge1[3];
  double edge2[3];
  double edge3[3];
  double jacobian; 

  /* Vertex 1 */
  edge1[0] = h5[0] - h1[0]; edge1[1] = h5[1] - h1[1]; edge1[2] = h5[2] - h1[2];
  edge2[0] = h2[0] - h1[0]; edge2[1] = h2[1] - h1[1]; edge2[2] = h2[2] - h1[2];
  edge3[0] = h4[0] - h1[0]; edge3[1] = h4[1] - h1[1]; edge3[2] = h4[2] - h1[2];
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 1 failed " << jacobian << std::endl;
#endif
    return false;
    }

  /* Vertex 2 */
  edge1[0] = h6[0] - h2[0]; edge1[1] = h6[1] - h2[1]; edge1[2] = h6[2] - h2[2];
  edge2[0] = h3[0] - h2[0]; edge2[1] = h3[1] - h2[1]; edge2[2] = h3[2] - h2[2];
  edge3[0] = h1[0] - h2[0]; edge3[1] = h1[1] - h2[1]; edge3[2] = h1[2] - h2[2];  
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 2 failed " << jacobian << std::endl;
#endif
    return false;
    }


  /* Vertex 3 */
  edge1[0] = h7[0] - h3[0]; edge1[1] = h7[1] - h3[1]; edge1[2] = h7[2] - h3[2];
  edge2[0] = h4[0] - h3[0]; edge2[1] = h4[1] - h3[1]; edge2[2] = h4[2] - h3[2];
  edge3[0] = h2[0] - h3[0]; edge3[1] = h2[1] - h3[1]; edge3[2] = h2[2] - h3[2];  
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 3 failed " << jacobian  << std::endl;
#endif
    return false;
    }
  

  /* Vertex 4 */
  edge1[0] = h8[0] - h4[0]; edge1[1] = h8[1] - h4[1]; edge1[2] = h8[2] - h4[2];
  edge2[0] = h1[0] - h4[0]; edge2[1] = h1[1] - h4[1]; edge2[2] = h1[2] - h4[2];
  edge3[0] = h3[0] - h4[0]; edge3[1] = h3[1] - h4[1]; edge3[2] = h3[2] - h4[2];  
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 4 failed " << jacobian  << std::endl;
#endif
    return false;
    }

  /* Vertex 5 */
  edge1[0] = h8[0] - h5[0]; edge1[1] = h8[1] - h5[1]; edge1[2] = h8[2] - h5[2];
  edge2[0] = h6[0] - h5[0]; edge2[1] = h6[1] - h5[1]; edge2[2] = h6[2] - h5[2];
  edge3[0] = h1[0] - h5[0]; edge3[1] = h1[1] - h5[1]; edge3[2] = h1[2] - h5[2];  
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 5 failed " << jacobian  << std::endl;
#endif
    return false;
    }

  /* Vertex 6 */
  edge1[0] = h5[0] - h6[0]; edge1[1] = h5[1] - h6[1]; edge1[2] = h5[2] - h6[2];
  edge2[0] = h7[0] - h6[0]; edge2[1] = h7[1] - h6[1]; edge2[2] = h7[2] - h6[2];
  edge3[0] = h2[0] - h6[0]; edge3[1] = h2[1] - h6[1]; edge3[2] = h2[2] - h6[2];  
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 6 failed " << jacobian  << std::endl;
#endif
    return false;
    }


  /* Vertex 7 */
  edge1[0] = h6[0] - h7[0]; edge1[1] = h6[1] - h7[1]; edge1[2] = h6[2] - h7[2];
  edge2[0] = h8[0] - h7[0]; edge2[1] = h8[1] - h7[1]; edge2[2] = h8[2] - h7[2];
  edge3[0] = h3[0] - h7[0]; edge3[1] = h3[1] - h7[1]; edge3[2] = h3[2] - h7[2];  
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 7 failed " << jacobian  << std::endl;
#endif
    return false;
    }

  /* Vertex 8 */
  edge1[0] = h7[0] - h8[0]; edge1[1] = h7[1] - h8[1]; edge1[2] = h7[2] - h8[2];
  edge2[0] = h5[0] - h8[0]; edge2[1] = h5[1] - h8[1]; edge2[2] = h5[2] - h8[2];
  edge3[0] = h4[0] - h8[0]; edge3[1] = h4[1] - h8[1]; edge3[2] = h4[2] - h8[2];  
  jacobian = vtkMath::Determinant3x3(edge1, edge2, edge3);
  if (jacobian <= 0)
    {
#ifdef DEBUG    
    std::cout << "Vertex 8 failed " << jacobian  << std::endl;
#endif
    return false;
    }

  
  return true;
}
