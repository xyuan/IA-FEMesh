#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <iostream>


#include "CheckHexahedronTopology.h"


int main( int argc, char * argv[] )
{
   double h1[3];
   double h2[3];
   double h3[3];
   double h4[3];
   double h5[3];
   double h6[3];
   double h7[3];
   double h8[3];
   
   
   h1[0] = -1.0; h1[1] = -1.0; h1[2] = 1.0;
   h2[0] = -1.0; h2[1] = 1.0; h2[2] = 1.0;
   h3[0] = 1.0; h3[1] = 1.0; h3[2] = 1.0;
   h4[0] = 1.0; h4[1] = -1.0; h4[2] = 1.0;
   h5[0] = -1.0; h5[1] = -1.0; h5[2] = -1.0;
   h6[0] = -1.0; h6[1] = 1.0; h6[2] = -1.0;
   h7[0] = 1.0; h7[1] = 1.0; h7[2] = -1.0;
   h8[0] = 1.0; h8[1] = -1.0; h8[2] = -1.0;
   
   bool result = CheckHexahedronTopology(h1,h2,h3,h4,h5,h6,h7,h8);
   
   std::cout << "Check Topology " << result << std::endl;
   
   
   h1[0] = 0.0; h1[1] = 0.0; h1[2] = 0.0;
   h2[0] = 1.0; h2[1] = 0.0; h2[2] = 0.0;
   h3[0] = 1.0; h3[1] = 1.0; h3[2] = 0.0;
   h4[0] = 0.0; h4[1] = 1.0; h4[2] = 0.0;
   h5[0] = 0.0; h5[1] = 0.0; h5[2] = 1.0;
   h6[0] = 1.0; h6[1] = 0.0; h6[2] = 1.0;
   h7[0] = 1.0; h7[1] = 1.0; h7[2] = 1.0;
   h8[0] = 1.0; h8[1] = 1.0; h8[2] = 1.0;
   
   result = CheckHexahedronTopology(h1,h2,h3,h4,h5,h6,h7,h8);
   
   std::cout << "Check Topology " << result << std::endl;
   
}
