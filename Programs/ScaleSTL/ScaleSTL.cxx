#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkTransform.h"


int main ( )
{
  double Center[3];
  Center[0] = Center[1] = Center [2] = 0;

  double ScaleFactor[3];
  ScaleFactor[0] = 1;
  ScaleFactor[1] = 1;
  ScaleFactor[2] = 1;

  vtkSTLReader *stl = vtkSTLReader::New( );
  stl->SetFileName( "E:\\Research\\Spine Model\\sclaed_lumbarnew_surf01075.stl" );
  stl->Update( );
  vtkPolyData *poly = stl->GetOutput( );

  vtkTransform* trans = vtkTransform::New();
  vtkPoints* mPointsStore = vtkPoints::New();

  trans->PostMultiply( );
  trans->Translate( -Center[0], -Center[1], -Center[2] );
  trans->Scale( ScaleFactor[0], ScaleFactor[1], ScaleFactor[2] );
  trans->Translate( Center );
  trans->TransformPoints( poly->GetPoints( ), mPointsStore );
  poly->GetPoints( )->DeepCopy( mPointsStore );

  vtkSTLWriter *writer = vtkSTLWriter::New( );
  writer->SetInput( poly );
  writer->SetFileName( "E:\\Research\\Spine Model\\sclaed_lumbarnew_surf01075.stl" );
  writer->Update( ) ;

  trans->Delete( );
  mPointsStore->Delete( );
}