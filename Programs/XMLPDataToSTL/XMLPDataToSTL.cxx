#include "vtkXMLPolyDataReader.h"
#include "vtkSTLWriter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkTransform.h"


int main ( )
{
  vtkXMLPolyDataReader *polyReader = vtkXMLPolyDataReader::New( );
  polyReader->SetFileName("subject.vtp");
  polyReader->Update( );
  vtkPolyData *subjectPolyData = polyReader->GetOutput( );

  vtkSTLWriter *writer = vtkSTLWriter::New( );
  writer->SetInput( subjectPolyData );
  writer->SetFileName( "subject.stl" );
  writer->Update( );

  return 1;
}