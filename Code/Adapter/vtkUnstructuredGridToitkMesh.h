#ifndef __vtkUnstructuredGridToitkMesh_h__

#define __vtkUnstructuredGridToitkMesh_h__



#include "vtkPoints.h"

#include "vtkUnstructuredGrid.h"

#include "vtkHexahedron.h"

#include "vtkTetra.h"

#include "itkDefaultDynamicMeshTraits.h"

#include "itkMesh.h"

#include "itkAutomaticTopologyMeshSource.h"

#include "itkHexahedronCell.h"

#include "vtkCellArray.h"



/** 

  \class vtkUnstructuredGridToitkMesh

  \brief 

    \warning

  \sa 

  */



class vtkUnstructuredGridToitkMesh

{



 public:



  vtkUnstructuredGridToitkMesh( void );

  ~vtkUnstructuredGridToitkMesh( void );

 

  typedef itk::DefaultStaticMeshTraits<double, 3, 3,float,float> MeshTraits;

  typedef itk::Mesh<double,3, MeshTraits> MeshType;

  

  /**

  Useful Internal Typedefs

  */

  typedef itk::AutomaticTopologyMeshSource< MeshType >  MeshSourceType;

  typedef MeshSourceType::Pointer  MeshSourceTypePointer;

  

  

  /**

  The SetInput method provides pointer to the vtkUnstructuredGrid

  */

  void SetInput( vtkUnstructuredGrid * grid);

  MeshType::Pointer GetOutput();

  void ConvertvtkToitk();



  MeshType::Pointer   m_itkMesh;



  vtkUnstructuredGrid *m_Grid;



  

};



#endif

