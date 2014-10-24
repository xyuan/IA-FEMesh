
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkUnstructuredGridToitkMesh.h"
#include "vtkPolyDataToitkMesh.h"
#include "vtkGeometryFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkTriangle.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyDataWriter.h"
#include "vtkIdList.h"
#include "vtkPointLocator.h"
#include "vtkComputeNormalsFromPolydataFilter.h"
#include "vtkOBBTree.h"
#include "vtkMath.h"
#include "ItkMeshToFEMMesh.h"
#include "itkMesh.h"
#include "itkFEM.h"
#include "itkFEMElements.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkFEMElementBase.h"
#include "itkPointSet.h"
#include "itkMeshSpatialObject.h"
#include "itkSpatialObjectWriter.h"


int main( )
{

  // Reading in VTK Unstructured Grid in the form of VTK file
  vtkUnstructuredGridReader *UGridReader = vtkUnstructuredGridReader::New();
  UGridReader->SetFileName( "/home/vince/development/IaFeMesh/Testing/Data/Deformable/template.vtk" );
  UGridReader->Update();

  vtkUnstructuredGrid *UnstGrid = UGridReader->GetOutput();

  // Convert from VTK Unstructured Grid to ITK Mesh

  typedef itk::DefaultStaticMeshTraits<double, 3, 3,float,float> MeshTraits;
  typedef itk::Mesh<double,3, MeshTraits> MeshType;

  vtkUnstructuredGridToitkMesh *vtkGridToItkMesh = new vtkUnstructuredGridToitkMesh;
  vtkGridToItkMesh->SetInput( UnstGrid );
  MeshType::Pointer UGrigItkMesh = vtkGridToItkMesh->GetOutput( );
  
  typedef itk::MeshSpatialObject<MeshType> MeshSpatialObjectType;
  MeshSpatialObjectType::Pointer soMesh = MeshSpatialObjectType::New();
  soMesh->SetMesh( UGrigItkMesh );
  
  typedef itk::SpatialObjectWriter<3, double, MeshTraits>WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( soMesh );
  writer->SetFileName( "./MeshSpatialObject.mhd" );
  writer->SetBinaryPoints ( false );
  writer->Update( );
      

  // Convert ITK Mesh to FEM Mesh for Unstructured Grid

  typedef itk::fem::Element3DC0LinearHexahedronStrain       HexElementType;
  typedef HexElementType::Node                              HexNodeType;

  typedef itk::fem::Solver       SolverType;
  typedef SolverType*            SolverPointerType;

  typedef itk::ItkMeshToFEMMesh<MeshType> FEMMeshType;
  FEMMeshType::Pointer UGridFEMMesh = FEMMeshType::New( );

  UGridFEMMesh->SetInput( UGrigItkMesh );
  UGridFEMMesh->SetFileName( "Finger.txt" );
  UGridFEMMesh->Update( );
  
  exit(0);
  
  
  SolverType* UGridSolver = UGridFEMMesh->GetOutput( );
  UGridSolver->GenerateGFN();

  
  // std::cout << "\t END conversion from Unstructured Grid to FEM Mesh \n" << std::endl;

  int numberOfPoints = UGridReader->GetOutput( )->GetNumberOfPoints( );

  vtkSTLReader *PDReader = vtkSTLReader::New();
  PDReader->SetFileName( "/scratch/rbafna/Model/subject.stl" );
  PDReader->Update();
  vtkPolyData *subjectPolyData = PDReader->GetOutput();

  /* To find closest point from the STL surface*/
  vtkPointLocator *PLocator = vtkPointLocator::New();
  PLocator->SetDataSet( subjectPolyData );
  PLocator->Update();

  /* To compute distance between 2 points */
  vtkMath *math = vtkMath::New();

  /* Creating a PolyData to store the o/p of geometry filter*/
  vtkPolyData *PolyData = vtkPolyData::New();

  /* To Check whether point is Inside the STL or not so that we can flip the Normal*/
  vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet( subjectPolyData );
  obb->SetMaxLevel( 999999 );
  obb->SetNumberOfCellsPerBucket( 99999 );
  obb->SetTolerance(0.000001);
  obb->BuildLocator();

  /* Setting up the Material Properties for the Mesh */
  itk::fem::MaterialLinearElasticity::Pointer material;
  material = itk::fem::MaterialLinearElasticity::New(); // Need to determine appropriate material props.
  material->GN = 0; // All nodes will have the same material prop.
  material->E = 55;
  material->nu = 0.0; // Poisson's Ratio set to '0' to avoid any deformation in the lateral direction
  UGridSolver->mat.push_back( itk::fem::FEMP<itk::fem::Material>(&*material) );

  typedef itk::fem::Node        NodeType;
  typedef NodeType::ArrayType   NodeArray;

  typedef vnl_vector<double> DoubleVectorType;

  /* Now Applying Loads to the Nodes of the Elements */

  itk::fem::Element::VectorType uptCoords(3);
  double pointLocation[3];
  float tol = 0.5;

  for ( int k = 0 ; k < 35; k++ ) // Number of Iterations to be performed. Need to get a better feel for it
  {
    /* Geometry Filter to convert Unstructured Grid to Surface Mesh ONLY for COMPUTING NORMALS */
    vtkGeometryFilter *GeometryFilter = vtkGeometryFilter::New();
    GeometryFilter->SetInput( UnstGrid );
    GeometryFilter->Update( );
    PolyData = GeometryFilter->GetOutput( );

    /* Computing Normals */  
    vtkComputeNormalsFromPolydataFilter *computeNormals = vtkComputeNormalsFromPolydataFilter::New();
    computeNormals->SetInput(PolyData);
    computeNormals->Update();
    vtkPointSet *normalsPointSet = computeNormals->GetOutput( );
    vtkPoints *finalNormals = normalsPointSet->GetPoints();

    int d = 0;
    vtkIdType nodeListId = 0;
    double pointNormal[3];
    vtkIdList *nodeList = vtkIdList::New();
    nodeList->Initialize();
    itk::fem::Solver::ElementArray::iterator eltIterator = UGridSolver->el.begin();
    for ( eltIterator; eltIterator != UGridSolver->el.end(); eltIterator++) // Iterating thru all the elements
    {
      int eltGN = (*eltIterator)->GN;      
      (*eltIterator)->SetMaterial( dynamic_cast<itk::fem::MaterialLinearElasticity*>(&*UGridSolver->mat.Find(0)) );
      unsigned int Npts=(*eltIterator)->GetNumberOfNodes();
      for(unsigned int ptCount = 0; ptCount < Npts; ptCount++)
      {
        uptCoords = (*eltIterator)->GetNode(ptCount)->GetCoordinates( );
        //std::cout << " Getting coordinates = " << uptCoords[0] << " , "<< uptCoords[1]<< " , " << uptCoords[2] <<std::endl;
        int globalNumber = (*eltIterator)->GetNode(ptCount)->GN;
        if ( nodeList->IsId( globalNumber ) == -1 ) // Keep track of nodes which have been loaded. So that the same node is not loaded more than once
        {
          nodeList->InsertId( nodeListId, globalNumber ); /* Can use SetId which is faster. For that setnumberofpoints needed.*/
          nodeListId++;
          finalNormals->GetPoint(globalNumber , pointNormal);
          if ( pointNormal[0] != -2 ) // Internal nodes have normals = -2
          {
            double pDatapt[3];
            pointLocation[0] = uptCoords[0]; // Getting the nodal coordinates from FEM Mesh & converting into double type
            pointLocation[1] = uptCoords[1];
            pointLocation[2] = uptCoords[2];

            vtkIdType ptId = PLocator->FindClosestPoint( pointLocation ); // Getting closest point on the surface
            subjectPolyData->GetPoint( ptId, pDatapt );
            double distance = sqrt ( static_cast<double>(math->Distance2BetweenPoints( pointLocation, pDatapt ) ) ); //Get dist btwn FEM node & closest surface pt
            int isOutside = obb->InsideOrOutside( pointLocation ); // Check whether point is outside the surface. 
            if ( distance > tol ) // Some tolerance value. Need to get a better idea of what these values shud be
            {
              if ( isOutside == 1) // If point is outside, make the distance -ve so that the normals are flipped.
              {
                distance *= -1;
              } 
	    
	      itk::fem::LoadNode::Pointer loader = itk::fem::LoadNode::New(); //. Creating load pointer to apply to each valid node.
              loader->GN = d;
              loader->m_element = UGridSolver->el.Find( eltGN );
              loader->m_pt = ptCount;
              loader->F = vnl_vector<double>(3);
              loader->F[0] = distance * pointNormal[0];
              loader->F[1] = distance * pointNormal[1];
              loader->F[2] = distance * pointNormal[2];
              UGridSolver->load.push_back(itk::fem::FEMP<itk::fem::Load>(*&loader));
              d++;
            }
          }
        }
      }
    }
    if ( k == 4 )  tol /= 2;
    if ( k == 14 ) tol /= 2;
    if ( k == 24 ) tol /= 2;
    if ( k == 29 ) tol = 0;

    std::cout <<" \n Iteration Number = " << k+1 <<" . And Tolerance =  " << tol << "\n"; 
    std::cout << " \n Number of Points Loaded = " << d << std::endl;

    /* Now Assebling the Matrices & Solving the FEM Problem */

    UGridSolver->AssembleK();
    UGridSolver->DecomposeK();
    UGridSolver->AssembleF();

    std::cout << " ALL Matrices Assembled\n";

    try 
    { 
      std::cout << " Solving FEM Problem.....\n";
      UGridSolver->Solve();
    }
    catch (itk::ExceptionObject &e) 
    {
      std::cerr << "Exception caught: " << e << "\n";
      return EXIT_FAILURE;
    }

    /* Clearing the Load vector.*/
    UGridSolver->load.clear();


    itk::fem::Element::VectorType newpt(3);

    /*
    * Updated Nodal Coordinates of all nodes in the system;
    */
    for( ::itk::fem::Solver::NodeArray::iterator n = UGridSolver->node.begin(); n != UGridSolver->node.end(); n++)
    {
      newpt = (*n)->GetCoordinates( );
      for( unsigned int d=0, dof; (dof=(*n)->GetDegreeOfFreedom(d))!=::itk::fem::Element::InvalidDegreeOfFreedomID; d++ )
      {
        newpt[d] = newpt[d] + UGridSolver->GetSolution(dof);
      }
      (*n)->SetCoordinates(newpt);
    }
    /* Updating the Unstructured Grid */    
    vtkIdType j = 0;
    for( ::itk::fem::Solver::NodeArray::iterator n = UGridSolver->node.begin(); n != UGridSolver->node.end(); n++)
    {
      DoubleVectorType uptCoords = (*n)->GetCoordinates();
      double newCoords[3];
      newCoords[0] = uptCoords[0];
      newCoords[1] = uptCoords[1];
      newCoords[2] = uptCoords[2];
      UnstGrid->GetPoints()->SetPoint( j, newCoords );
      // std::cout << " Final Updated coordinates = " << uptCoords << std::endl;
      j++;
    }
    UnstGrid->Update();
  }
  /* Projecting ONLY the surface points onto the subject surface */
  /*double uGridPt[3];
  double pDatapt[3];
  double pointNormal[3];
  vtkIdType ptId;
  for ( int i = 0; i < numberOfPoints; i++ )
  {
    finalNormals->GetPoint( i , pointNormal);
    if ( pointNormal[0] != -2 )
    {
      UnstGrid->GetPoint( i, uGridPt );
      ptId = PLocator->FindClosestPoint( uGridPt );
      PolyData->GetPoint( ptId, pDatapt );
      UnstGrid->GetPoints()->SetPoint( i, pDatapt );
    }
  }
  UnstGrid->Update();
*/

  /* Writing out the Unstructured Grid for final Visualization */
  vtkUnstructuredGridWriter *UnstGridWriter = vtkUnstructuredGridWriter::New();
  UnstGridWriter->SetInput(UnstGrid);
  UnstGridWriter->SetFileName("/scratch/rbafna/DefModels/DefVince_35_Iter_55_0.0_R_Tol_2nd.vtk");
  UnstGridWriter->Update();

  /* Converting the PolyData to STL format & writing it out for final analysis */
  vtkTriangleFilter *triangle = vtkTriangleFilter::New();
  triangle->SetInput( PolyData );
  triangle->Update();

  vtkSTLWriter *stlwriter = vtkSTLWriter::New();
  stlwriter->SetInput( triangle->GetOutput( ) );
  stlwriter->SetFileName( "/scratch/rbafna/DefSTL/Def_STL_35_Iter_55_0.0_R_Tol_2nd.stl");
  stlwriter->Update();

  std::cout << " All Done !!! " << std::endl;
  return 0;
}
