#include<iostream>
#include <fstream>

using namespace std;

void RemoveCommas( char *word );

int main()
{
  int nodesPerRow = 1;
  int numOfNodes = 0;
  int numOfElements = 0;

  char dummyStr[100];
  char dummy[100];
  char nodeNumStr[100];
  char xcoorStr[50];
  char ycoorStr[50];
  char zcoorStr[50];

  char elemNumStr[100];
  char elemConnStr[100];

  std::ifstream inputFile;
  inputFile.open( "Abaqus.inp", std::ios::in );

  if ( inputFile == NULL )
  {
    cout << "\n\n!!!!!!!!!!!!!! Database Not Found !!!!!!!!!! \n\n" << endl;
    exit(1);
  }

  /* Just to count the number of nodes which is to be appended at the beginning of the vtk file */
  do
  {
    inputFile.getline(dummy, 100, ',');
    if ( strcmp( dummy, "*ELEMENT" ) != 0 )
    {
      inputFile.getline(dummyStr, 50, '\n');
      numOfNodes++;
    }
  }
  while( strcmp( dummy, "*ELEMENT" ) != 0);

  /* Counting the number of elements in the Abaqus input file */
  do
  {
    inputFile.getline( dummyStr, 200, '\n');
    numOfElements++;
  }
  while( !inputFile.eof( ) );

  numOfElements -= 2;

  inputFile.clear( );
  inputFile.seekg( std::ios::beg );

  std::ofstream outputFile;
  outputFile.open( "UnstructuredGrid.vtk", std::ios::out );

  /* Writing out the Nodal Definitions */
  outputFile << "# vtk DataFile Version 3.0 \nvtk output \nASCII \n\nDATASET UNSTRUCTURED_GRID\nPOINTS " << (numOfNodes) << " float" << endl;
 // outputFile << "0 0 0" << endl; // Additional node since Abaqus node numbering starts from 1 & vtk numbering starts from 0
  do
  {
    inputFile.getline(nodeNumStr, 100, ',');
    if ( strcmp( nodeNumStr, "*ELEMENT" ) != 0 )
    {
      inputFile.getline(xcoorStr, 50, ',');
      inputFile.getline(ycoorStr, 50, ',');
      inputFile.getline(zcoorStr, 50, '\n');

      outputFile << xcoorStr << " " << ycoorStr << " " << zcoorStr << "  ";
      if ( nodesPerRow % 3 == 0)
      {
        outputFile << endl;
      }
      nodesPerRow++;
    }
  }
  while( strcmp( nodeNumStr, "*ELEMENT" ) != 0 );

  /* Writing out the Element Definitions */

  outputFile << "\nCELLS " << numOfElements << " " << (9 * numOfElements) << endl;
  inputFile.getline( elemNumStr, 100, '\n');
  for ( int i = 0; i < numOfElements; i++ )
  {
    inputFile.getline( elemNumStr, 100, ',' );
    inputFile.getline( elemConnStr, 100, '\n' );

    RemoveCommas( elemConnStr );

    outputFile << "8 " << elemConnStr << endl;;
  }

  outputFile << "\nCELL_TYPES " << numOfElements << endl;
  for ( int i = 0; i < numOfElements; i++ )
  {
    outputFile << "12" << endl;
  }
  inputFile.close( );
  outputFile.close( );

	return 0;
}


void RemoveCommas( char *word )
{
  if ( word )
    while( *word )
    {
      if ( *word == ',' )
      {
        *word = ' ';
      }
      word++;
    }
}