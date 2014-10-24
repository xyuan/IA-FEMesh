/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: DeformableFeRegistration.cxx,v $
Language:  C++
Date:      $Date: 2008/01/12 22:31:44 $
Version:   $Revision: 1.6 $

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


#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkPolyDataToitkMesh.h>
#include <vtkGeometryFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataReader.h>
#include <vtkIdList.h>
#include <vtkPointLocator.h>
#include <vtkOBBTree.h>
#include <vtkMath.h>
#include <itkMesh.h>
#include <itkFEM.h>
#include <itkFEMElements.h>
#include <itkDefaultStaticMeshTraits.h>
#include <itkFEMElementBase.h>
#include <itkPointSet.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkMaskPoints.h>
#include <vtkMatrix4x4.h>
#include <vtkGeneralTransform.h>
#include <vtkPointLocator.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkProcrustesAlignmentFilter.h>
#include <vtkPointSet.h>


#include "vtkUnstructuredGridToitkMesh.h"
#include "vtkMimxComputeNormalsFromPolydataFilter.h"
#include "ItkMeshToFEMMesh.h"
#include "vtkMimxAbaqusFileWriter.h"
#include "DeformableFeRegistrationCLP.h"


typedef itk::fem::Element3DC0LinearHexahedronStrain       HexElementType;
typedef HexElementType::Node                              HexNodeType;
typedef itk::DefaultStaticMeshTraits<double, 3, 3,float,float> MeshTraits;
typedef itk::Mesh<double,3, MeshTraits> MeshType;
typedef itk::fem::Solver       SolverType;
typedef SolverType*            SolverPointerType;
typedef itk::ItkMeshToFEMMesh<MeshType> FEMMeshType;
typedef itk::fem::Node        NodeType;
typedef NodeType::ArrayType   NodeArray;
typedef vnl_vector<double> DoubleVectorType;

void UpdateGridCoordinates( SolverPointerType UGridSolver,
                           vtkUnstructuredGrid *DefCoarseGrid );
void LoadNodes( SolverPointerType UGridSolver,
               vtkUnstructuredGrid *DeformedMesh,
               vtkPolyData *subjectPolyData,
               vtkOBBTree *obb, 
               bool applyCenterBoundadyConditions,
               double tolerance, 
               bool debug, bool projectAlongNormal, bool useExternalBoundaryConditions );

vtkUnstructuredGrid* ApplyThinPlateSpline( vtkUnstructuredGrid *DefCoarseGrid,
                                          vtkUnstructuredGrid *UndefCoarseGrid,
                                          vtkUnstructuredGrid *FineGrid,
                                          const int PointRatio, std::string baseName, 
                                          const int iteration, 
                                          bool debug );

void WriteIntermediateMesh( vtkUnstructuredGrid* IntermediateMesh, std::string name );

vtkGeneralTransform* PerformICP ( vtkPolyData *subjectPolyData, vtkUnstructuredGrid *OrigGrid, int iter, int land, float meanDist );
vtkGeneralTransform* PerformProcrustes ( vtkPolyData *subjectPolyData, vtkUnstructuredGrid *OrigGrid, int iter, int land, float meanDist );
vtkUnstructuredGrid* ApplyInitializeTransform(vtkGeneralTransform *initialTransform, vtkUnstructuredGrid * originalGrid);
bool IntersectPointNormalWithSurface(double *pointLocation, double *pointNormal, double *distance,
                                     vtkOBBTree *obb, double *projectedPoint);
double ComputeAverageDistance( vtkUnstructuredGrid* IntermediateMesh, vtkPolyData* subjectSurface,
                               vtkOBBTree *obb, bool distanceAlongNormals, bool debug, double *maxDistance);


#define MAX_TEMPLATES 10


/*
*
* FE Based Template mapping program - Describe the input and output parameters
*
*/
int main( int argc, char **argv)
{

  PARSE_ARGS;
  
  if ( debug )
    {
    // Summarize the command line arguements for the user
    std::cout << "Voxel Meshing Parameters" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "\tSubject Surface filename: " << SubjectSurface << std::endl;
    std::cout << "\tTemplate Mesh filename: " << TemplateMeshFileNames[0] << std::endl;
    std::cout << "\tOutput VTK filename: " << vtkFilename << std::endl;
    std::cout << "\tIntermediate Mesh filename: " << interMeshFileName << std::endl;
    std::cout << "\tNumber of Refinement Levels: " << numRefinementLevels << std::endl;
    std::cout << "\tYoung's Modulus: " << youngsModulus[0] << std::endl;
    std::cout << "\tMaximum number of Iterations: " << numIterations[0] << std::endl;
    std::cout << "\tNumber of Lanfmarks: " << numberOfLandmarks << std::endl;
    std::cout << "\tMax Mean Distance for ICP: " << maxMeanDistanace << std::endl;
    std::cout << "\tPoint Ratio for TPS: " << PointRatio << std::endl;
    std::cout << "\tPoisson Ratio: " << poissonsRatio << std::endl;
    std::cout << "\tMax Number of ICP Iterations: " << numberOfIcpIterations << std::endl;    
    std::cout << "\tApply Center Boundary Conditions: " << applyCenterBoundaryConditions << std::endl;
    std::cout << "\tDistance Tolerance: " << tolerance << std::endl;
    std::cout << "\tDistance Along Normal: " << distanceAlongNormals << std::endl;
    std::cout << "\tConvergence Distance: " << convergenceDistance << std::endl;
    std::cout << "\tConvergence Type: " << convergenceType << std::endl;
    std::cout << "\tInitial Transform Type: " << initialTransform << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
  }
  
  int initialTransformType;
  if ( initialTransform.compare("None") == 0 )
  {
    initialTransformType = 0;
  } 
  else if ( initialTransform.compare("ICP") == 0 )
  {
    initialTransformType = 1;
  } 
  else if ( initialTransform.compare("Procrustes") == 0 )
  {
    initialTransformType = 2;
  } 
  else
  {
    std::cerr << "Error: Invalid Initial Transform Type: Must be None, ICP, or Procrustes" << std::endl;
    exit(1);
  } 

  int convergenceDistanceType;
  if ( convergenceType.compare("None") == 0 )
  {
    convergenceDistanceType = 0;
    convergenceDistance = 0.0;
  } 
  else if ( convergenceType.compare("Average") == 0 )
  {
    convergenceDistanceType = 1;
  } 
  else if ( convergenceType.compare("Maximum") == 0 )
  {
    convergenceDistanceType = 2;
  } 
  else
  {
    std::cerr << "Error: Invalid Initial Transform Type: Must be None, Average, or Maximum" << std::endl;
    exit(1);
  } 
  
  
  /* Reading in Subject Surface */
  vtkPolyData *subjectPolyData;
  if ( (strstr(argv[1], ".stl") != NULL) || (strstr(argv[1], ".STL")!= NULL) )
  {
    vtkSTLReader *PDReader = vtkSTLReader::New();
    PDReader->SetFileName( SubjectSurface.c_str() );
    PDReader->Update();
    subjectPolyData = PDReader->GetOutput();
  }
  else
  {
    vtkPolyDataReader *polyReader = vtkPolyDataReader::New();
    polyReader->SetFileName( SubjectSurface.c_str() );
    polyReader->Update( );	
    subjectPolyData = polyReader->GetOutput();
  }


  /*
  * Build OBB Tree from the Surface -
  *   Used to check inside/outside status for exterior mesh points
  */
  vtkOBBTree *obb = vtkOBBTree::New();
  obb->SetDataSet( subjectPolyData );
  obb->SetMaxLevel( 999999 );
  obb->SetNumberOfCellsPerBucket( 999999 );
  obb->SetTolerance( 0.000001 );
  obb->BuildLocator( );

  /* Setting up the Material Properties for the Mesh */
  itk::fem::MaterialLinearElasticity::Pointer material;


  // Load the initial (coarse) template mesh
  vtkUnstructuredGridReader *CoarseGridReader = vtkUnstructuredGridReader::New();
  CoarseGridReader->SetFileName( TemplateMeshFileNames[0].c_str() );
  CoarseGridReader->Update();
  vtkUnstructuredGrid *CurrentMesh = CoarseGridReader->GetOutput( );


  vtkGeneralTransform *initializeTransform = vtkGeneralTransform::New( );

  if ( initialTransformType > 0 )
  {
    if ( initialTransformType == 1)
      {
      initializeTransform = PerformICP( subjectPolyData, CurrentMesh, numberOfIcpIterations, numberOfLandmarks, maxMeanDistanace );
      }
    else
      {
      initializeTransform = PerformProcrustes( subjectPolyData, CurrentMesh, numberOfIcpIterations, numberOfLandmarks, maxMeanDistanace );
      }
    CurrentMesh = ApplyInitializeTransform( initializeTransform, CurrentMesh );
   
    if ( interMeshFileName.length() > 0 )
     {
     std::string name = interMeshFileName + "_ICP.vtk";
     WriteIntermediateMesh( CurrentMesh, name );
     }
  }
  vtkUnstructuredGrid *DeformedMesh = vtkUnstructuredGrid::New();
  vtkUnstructuredGrid *BestMesh = vtkUnstructuredGrid::New();
  vtkUnstructuredGrid *ScaledMesh = vtkUnstructuredGrid::New(); 
  ScaledMesh->DeepCopy( CurrentMesh );
  

  for ( int i = 0 ; i < numRefinementLevels; i++ ) // Number of Refinement Levels to be performed.
  {
    double avgDist = 0.0;
    double oldDist;
    double maxDistance;
  
    material = itk::fem::MaterialLinearElasticity::New();
    material->GN = 0; // All nodes will have the same material prop.
    material->E = youngsModulus[i];
    material->nu = poissonsRatio; // Poisson's Ratio set to '0' to avoid any deformation in the lateral direction

    
    
    // Convert the VTK Unstructured Grid into an ITK Mesh
    vtkUnstructuredGridToitkMesh *vtkGridToItkMesh = new vtkUnstructuredGridToitkMesh;
    vtkGridToItkMesh->SetInput( CurrentMesh );
    MeshType::Pointer UGrigItkMesh = vtkGridToItkMesh->GetOutput( );
    
    
    // Convert ITK Mesh to FEM Mesh for Unstructured Grid
    FEMMeshType::Pointer UGridFEMMesh = FEMMeshType::New( );
    UGridFEMMesh->SetInput( UGrigItkMesh );
    UGridFEMMesh->Update( );
    SolverPointerType UGridSolver = UGridFEMMesh->GetOutput( );
    UGridSolver->GenerateGFN();
    UGridSolver->mat.push_back( itk::fem::FEMP<itk::fem::Material>(&*material) );
    

    // Copy the initial mesh that will be deformed
    DeformedMesh->DeepCopy( CurrentMesh );
    BestMesh->DeepCopy( CurrentMesh );
    
    // Make the original distance based on the starting configuration
    oldDist = ComputeAverageDistance( CurrentMesh, subjectPolyData, obb, distanceAlongNormals, debug, &maxDistance );


    for ( int j = 0; j <  numIterations[i]; j++ )
    {
      if ( debug ) std::cout << "\n **Refinement Level = " << i+1 << "  & Iteration Number = " << j+1 << "**\n" << std::endl;
      /* Loading Nodes */
      LoadNodes( UGridSolver, DeformedMesh, subjectPolyData, obb, applyCenterBoundaryConditions, tolerance, debug, distanceAlongNormals, useExternalBoundaryConditions );

      /* Now Assembling the Matrices & Solving the FEM Problem */
      
      UGridSolver->InitializeLinearSystemWrapper( );
      UGridSolver->AssembleK();
      UGridSolver->DecomposeK();
      UGridSolver->AssembleF();

      if ( debug ) std::cout << " ALL Matrices Assembled\n";
      
      try
      {
        if ( debug ) std::cout << " Solving FEM Problem.....\n";
        UGridSolver->Solve();
      }
      catch (itk::ExceptionObject &e)
      {
        std::cerr << "Exception caught: " << e << "\n";
        return EXIT_FAILURE;
      }

      /* Clearing the Load vector.*/
      UGridSolver->load.clear( );

      UpdateGridCoordinates( UGridSolver, DeformedMesh );

      if ( interMeshFileName.length() > 0 )
      {
        if ( debug ) std::cout << "Write Intermediate Mesh File" << std::endl;
        char buffer[16];
        sprintf(buffer, "_%d_%d", i+1, j+1);
        std::string name = interMeshFileName + buffer + ".vtk";
        WriteIntermediateMesh( DeformedMesh, name );
      }

      avgDist = ComputeAverageDistance( DeformedMesh, subjectPolyData, obb, distanceAlongNormals, debug, &maxDistance );

      if ( debug ) std::cout << " Old Distance = " << oldDist << " Avg Distance = " << avgDist << " Max Distance = " << maxDistance << std::endl;
      if ( avgDist > oldDist )
      {
        if ( debug ) std::cout << "Reverting to previous mesh" << std::endl;
        DeformedMesh->Initialize();
        DeformedMesh->Allocate( BestMesh->GetNumberOfCells( ), BestMesh->GetNumberOfCells( ) );
        for ( int k=0; k < BestMesh->GetNumberOfCells( ); k++ )
        {
          DeformedMesh->InsertNextCell( BestMesh->GetCellType( k ), BestMesh->GetCell( k )->GetPointIds( ) );
        }
        vtkPoints *copyPoints = vtkPoints::New();
        copyPoints->Allocate(  BestMesh->GetPoints( )->GetNumberOfPoints(),  BestMesh->GetPoints( )->GetNumberOfPoints() );
        copyPoints->DeepCopy( BestMesh->GetPoints() );
        DeformedMesh->SetPoints( copyPoints );
	
        if ( debug )
        {
          char buffer[16];
          sprintf(buffer, "BEST_%d", i+1);
          std::string name = interMeshFileName + "_" + buffer + ".vtk";
          if ( debug ) std::cout << "Write ICP Mesh File: " << name << std::endl;
          WriteIntermediateMesh( DeformedMesh, name );
        }

        break;
      }
      else
      {
        if ( debug ) std::cout << "Copy Deformed Mesh to BEST Mesh" << std::endl;
        BestMesh->Initialize();
        BestMesh->Allocate( DeformedMesh->GetNumberOfCells( ), DeformedMesh->GetNumberOfCells( ) );
        for ( int k=0; k < DeformedMesh->GetNumberOfCells( ); k++ )
        {
          BestMesh->InsertNextCell( DeformedMesh->GetCellType( k ), DeformedMesh->GetCell( k )->GetPointIds( ) );
        }

        vtkPoints *copyPoints = vtkPoints::New();
        copyPoints->Allocate(  DeformedMesh->GetPoints( )->GetNumberOfPoints(),  DeformedMesh->GetPoints( )->GetNumberOfPoints() );
        copyPoints->DeepCopy( DeformedMesh->GetPoints() );
        BestMesh->SetPoints( copyPoints );

        oldDist = avgDist;
      }
      
      /* If within user specified convergence distance - move to next level */
      if ( ( avgDist < convergenceDistance ) && ( convergenceDistanceType == 1 ) )
      {
        break;
      }
      else if ( ( maxDistance < convergenceDistance ) && ( convergenceDistanceType == 2 ) )
      {
        break;
      }

    } // End of Iterations

    if ( i != numRefinementLevels-1 )
    {        
      if ( debug ) std::cout << " Going to Next Level " << i << " with template mesh " << TemplateMeshFileNames[i+1] << std::endl;
      
      /* We have another Mesh Refinement level that is initialized with TPS */
      vtkUnstructuredGridReader *GridReader = vtkUnstructuredGridReader::New( );
      GridReader->SetFileName( TemplateMeshFileNames[i+1].c_str( ) );
      GridReader->Update( );
      vtkUnstructuredGrid *NextMesh = GridReader->GetOutput( );
      if ( initialTransformType > 0 )
      {
        NextMesh = ApplyInitializeTransform( initializeTransform, NextMesh );                
        if ( interMeshFileName.length() > 0 )
        {
          /* Iterations are 1 based and we are refering to mesh for next iteration */
          char buffer[10];
          sprintf(buffer, "%d", i+2);
          std::string name = interMeshFileName + "_" + buffer + "_ICP.vtk";
          if ( debug ) std::cout << "Write ICP Mesh File: " << name << std::endl;
          WriteIntermediateMesh( NextMesh, name );
        }
      }
      
      CurrentMesh = ApplyThinPlateSpline( DeformedMesh, ScaledMesh, NextMesh, PointRatio, interMeshFileName, i, debug);
      if ( interMeshFileName.length() > 0 )
      {
        char buffer[10];
        /* Iterations are 1 based and we are refering to mesh for next iteration */
        sprintf(buffer, "%d", i+2);
        std::string name = interMeshFileName + "_" + buffer + "_TPS.vtk";
        if ( debug ) std::cout << "Write TPS Mesh File: " << name << std::endl;
        WriteIntermediateMesh( CurrentMesh, name );
      }
      ScaledMesh->Initialize();
      ScaledMesh->DeepCopy( NextMesh );
    }

  } // End of Mesh Refinement Levels

  vtkUnstructuredGridWriter *finalWriter = vtkUnstructuredGridWriter::New( );
  finalWriter->SetInput( DeformedMesh );
  finalWriter->SetFileName( vtkFilename.c_str() );
  finalWriter->Update( );
  
  if ( debug ) std::cout << "Registration is complete!" << std::endl;
  return EXIT_SUCCESS;
}



/*
*
* Apply Loads to the grid - based on distance from the template mesh to the subject surface
*
*/
void LoadNodes( SolverPointerType UGridSolver, vtkUnstructuredGrid *DeformedMesh, 
                vtkPolyData *subjectPolyData,  vtkOBBTree *obb, 
                bool applyCenterBoundaryConditions, double tolerance, bool debug,
                bool projectAlongNormal, bool useExternalBoundaryConditions )
{

  /* To find closest point from the STL surface*/
  vtkPointLocator *PLocator = vtkPointLocator::New();
  PLocator->SetDataSet( subjectPolyData );
  PLocator->Update();
    
  vtkGeometryFilter *Geometry = vtkGeometryFilter::New();
  Geometry->SetInput( DeformedMesh );
  Geometry->Update( );
  vtkPolyData *DefPoly = Geometry->GetOutput( );
  

  vtkMimxComputeNormalsFromPolydataFilter *computeNormals =   vtkMimxComputeNormalsFromPolydataFilter::New();
  computeNormals->SetInput(DefPoly);
  computeNormals->Update();
  vtkPointSet *normalsPointSet = computeNormals->GetOutput( );
  vtkPoints *finalNormals = normalsPointSet->GetPoints();

  if (debug )
  {
    std::cout << "Mesh Number of Points: " << DeformedMesh->GetNumberOfPoints() << std::endl;
    std::cout << "Mesh Geom Number of Points: " << DefPoly->GetNumberOfPoints() << std::endl;
    std::cout << "Normal Point Number: " << finalNormals->GetNumberOfPoints() << std::endl;
  }


  /* Save the Center - Use for Internal Nodes for Boundary Conditions */
  double meshCenter[3];
  DeformedMesh->GetCenter( meshCenter );
  const int numPtsDistance = 8;
  double minDistance[numPtsDistance];
  int minEltGN[numPtsDistance];
  int minPtCount[numPtsDistance];
  for (int i=0;i<numPtsDistance;i++) minDistance[i] = 1.0e20;
  if (debug ) std::cout << "Mesh Center " << meshCenter[0] << " " << meshCenter[1] << " " << meshCenter[2] << std::endl;


  /* To compute distance between 2 points */
  vtkMath *math = vtkMath::New();

  /* Now Applying Loads to the Nodes of the Elements */

  itk::fem::Element::VectorType uptCoords(3);
  double pointLocation[3];

  int d = 0;
  int r = 0;
  vtkIdType nodeListId = 0;
  double pointNormal[3];
  vtkIdList *nodeList = vtkIdList::New();
  nodeList->Initialize();
  itk::fem::Solver::ElementArray::iterator eltIterator = UGridSolver->el.begin();


  for ( eltIterator; eltIterator != UGridSolver->el.end(); eltIterator++ ) // Iterating thru all the elements
  {
    int eltGN = (*eltIterator)->GN;
    (*eltIterator)->SetMaterial( dynamic_cast<itk::fem::MaterialLinearElasticity*>(&*UGridSolver->mat.Find(0)) );

    unsigned int Npts=(*eltIterator)->GetNumberOfNodes();
    for( unsigned int ptCount = 0; ptCount < Npts; ptCount++ )
    {
      uptCoords = (*eltIterator)->GetNode(ptCount)->GetCoordinates( );
      //std::cout << " Node " << eltGN << " Point Index " << ptCount << 
      //             " coordinates = " << uptCoords[0] << " , "<< uptCoords[1]<< " , " << uptCoords[2] <<std::endl;
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
          double distance = pow ( math->Distance2BetweenPoints( pointLocation, pDatapt ), 0.5 ); //Get dist btwn FEM node & closest surface pt
          
          if ( projectAlongNormal )
          {
            IntersectPointNormalWithSurface(pointLocation, pointNormal, &distance, obb, pDatapt);
          }

          int isOutside = obb->InsideOrOutside( pointLocation ); // Check whether point is outside the surface.
          if ( isOutside == 1) // If point is outside, make the distance negative so that the normals are flipped.
          {
            distance *= -1.0;
          }
	  
          if ( fabs( distance ) > tolerance )
          {
	    //std::cout << "Load Node: " << distance << " with direction " << pointNormal[0]
	    //          << " " << pointNormal[1] << " " << pointNormal[2] << std::endl;
            itk::fem::LoadNode::Pointer loader = itk::fem::LoadNode::New( ); //. Creating load pointer to apply to each valid node.
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
          else
          {
            if ( useExternalBoundaryConditions )
            {
              //if ( debug ) std::cout << "Apply External Boundary Condition" << std::endl;
              itk::fem::LoadBC::Pointer restrictor1 = itk::fem::LoadBC::New( );
              restrictor1->m_element = UGridSolver->el.Find( eltGN );
              restrictor1->m_dof = (ptCount*3)+0;
              restrictor1->m_value = vnl_vector<double>( 1, 0.0 );
              UGridSolver->load.push_back(itk::fem::FEMP<itk::fem::Load>(*&restrictor1));

              itk::fem::LoadBC::Pointer restrictor2 = itk::fem::LoadBC::New( );
              restrictor2->m_element = UGridSolver->el.Find( eltGN );
              restrictor2->m_dof = (ptCount*3)+1;
              restrictor2->m_value = vnl_vector<double>( 1, 0.0 );
              UGridSolver->load.push_back(itk::fem::FEMP<itk::fem::Load>(*&restrictor2));

              itk::fem::LoadBC::Pointer restrictor3 = itk::fem::LoadBC::New( );
              restrictor3->m_element = UGridSolver->el.Find( eltGN );
              restrictor3->m_dof = (ptCount*3)+2;
              restrictor3->m_value = vnl_vector<double>( 1, 0.0 );
              UGridSolver->load.push_back(itk::fem::FEMP<itk::fem::Load>(*&restrictor3));
            }
          }
        }
        else
        {
          if ( applyCenterBoundaryConditions )
          {
            double distance = math->Distance2BetweenPoints( pointLocation, meshCenter );
            //std::cout << "Current Distance " << distance << std::endl;

            int index, tmpIndex;
            int point, tmpPoint;
            double tmpDistance;
            int i;
            for (i=0;i<numPtsDistance;i++)
            {
              if (distance < minDistance[i] )
              {
                index = eltGN;
                point = ptCount;
                break;
              }
            }
            for (int j=i;j<numPtsDistance;j++)
            {
               tmpDistance = minDistance[j];
               tmpIndex = minEltGN[j];
               tmpPoint = minPtCount[j];
               minDistance[j] = distance;
               minEltGN[j] = index;
               minPtCount[j] = point;
               distance = tmpDistance;
               index = tmpIndex;
               point = tmpPoint;
            }
          }
        }
      }
    } 
  }

  if ( applyCenterBoundaryConditions )
  {
   if ( debug ) std::cout << "Applying Center Boundary Conditions " << std::endl;
   for (int i=0;i<numPtsDistance;i++)
    {
       if ( debug )
       {
         std::cout << "Min Distance " << minDistance[i] << "  Element Index " <<
         minEltGN[i] << " Point Index " << minPtCount[i] << std::endl;
       }

       itk::fem::LoadBC::Pointer restrictor1 = itk::fem::LoadBC::New( );
       restrictor1->m_element = UGridSolver->el.Find( minEltGN[i] );
       restrictor1->m_dof = (minPtCount[i]*3)+0;
       restrictor1->m_value = vnl_vector<double>( 1, 0.0 );
       UGridSolver->load.push_back(itk::fem::FEMP<itk::fem::Load>(*&restrictor1));
            
       itk::fem::LoadBC::Pointer restrictor2 = itk::fem::LoadBC::New( );
       restrictor2->m_element = UGridSolver->el.Find( minEltGN[i] );
       restrictor2->m_dof = (minPtCount[i]*3)+1;
       restrictor2->m_value = vnl_vector<double>( 1, 0.0 );
       UGridSolver->load.push_back(itk::fem::FEMP<itk::fem::Load>(*&restrictor2));
            
       itk::fem::LoadBC::Pointer restrictor3 = itk::fem::LoadBC::New( );
       restrictor3->m_element = UGridSolver->el.Find( minEltGN[i] );
       restrictor3->m_dof = (minPtCount[i]*3)+2;
       restrictor3->m_value = vnl_vector<double>( 1, 0.0 );
       UGridSolver->load.push_back(itk::fem::FEMP<itk::fem::Load>(*&restrictor3));
    }   
  }
   
}


/*
*
* Update the template grid based on the results from the FE Analysis
*
*/

void UpdateGridCoordinates( SolverPointerType UGridSolver, vtkUnstructuredGrid *DefCoarseGrid )
{
  /* Updated Nodal Coordinates of all nodes in the system; */
  itk::fem::Element::VectorType newpt(3);
  for( ::itk::fem::Solver::NodeArray::iterator n = UGridSolver->node.begin(); n != UGridSolver->node.end(); n++)
  {
    newpt = (*n)->GetCoordinates( );
    for( unsigned int d=0, dof; (dof=(*n)->GetDegreeOfFreedom(d))!=::itk::fem::Element::InvalidDegreeOfFreedomID; d++ )
    {
      newpt[d] = newpt[d] + UGridSolver->GetSolution(dof);
    }
    (*n)->SetCoordinates(newpt);
  }

  /* Updating the Original Unstructured Grid */
  vtkIdType j = 0;
  for( ::itk::fem::Solver::NodeArray::iterator n = UGridSolver->node.begin(); n != UGridSolver->node.end(); n++)
  {
    DoubleVectorType uptCoords = (*n)->GetCoordinates();
    double newCoords[3];
    newCoords[0] = uptCoords[0];
    newCoords[1] = uptCoords[1];
    newCoords[2] = uptCoords[2];
    DefCoarseGrid->GetPoints()->SetPoint( j, newCoords );
    j++;
  }
  DefCoarseGrid->Update( );
}


/*
*
* Thin plate spline is used to apply the results from a coarse grid onto a more
* refined grid. Initializes the next level of registration.
*
*/
vtkUnstructuredGrid* ApplyThinPlateSpline( vtkUnstructuredGrid *DefCoarseGrid,
                                          vtkUnstructuredGrid *UndefCoarseGrid,
                                          vtkUnstructuredGrid *FineGrid,
                                          const int PointRatio, std::string interMeshFileName,
                                          const int iteration, bool debug )
{
  /*Creating a PolyData to store the o/p of geometry filter*/
  //vtkPolyData *UndefPoly = vtkPolyData::New( );
  //vtkPolyData *DefPoly = vtkPolyData::New( );

  if (  interMeshFileName.length() > 0 )
  {
    char buffer[16];
    sprintf(buffer, "%d", iteration+1);
    std::string name = interMeshFileName + "_TPS_DEF_" + buffer + ".vtk";
    std::cout << "Write ICP DEF Mesh File: " << name << std::endl;
    WriteIntermediateMesh( DefCoarseGrid, name );

    name = interMeshFileName + "_TPS_UNDEF_" + buffer + ".vtk";
    std::cout << "Write ICP UNDEF Mesh File: " << name << std::endl;
    WriteIntermediateMesh( UndefCoarseGrid, name );
  }

  /* Geometry Filter to convert Unstructured Grid to Surface Mesh */
  if ( debug ) std::cout << "Deformed Mesh Points: " << DefCoarseGrid->GetNumberOfPoints() << std::endl;
  vtkGeometryFilter *DefGeometry = vtkGeometryFilter::New();
  DefGeometry->SetInput( DefCoarseGrid );
  DefGeometry->Update( );
  vtkCleanPolyData *deformedCleanPolyDataFilter = vtkCleanPolyData::New();
  deformedCleanPolyDataFilter->SetInput( DefGeometry->GetOutput( ) );
  deformedCleanPolyDataFilter->SetTolerance( 0.0 ); 
  deformedCleanPolyDataFilter->Update( );
  vtkPolyData *DefPoly = deformedCleanPolyDataFilter->GetOutput( );

  vtkGeometryFilter *UndefGeometry = vtkGeometryFilter::New();
  UndefGeometry->SetInput( UndefCoarseGrid );
  UndefGeometry->Update( );
  if ( debug ) std::cout << "UnDeformed Mesh Points: " << UndefCoarseGrid->GetNumberOfPoints() << std::endl;
  
  vtkCleanPolyData *undeformedCleanPolyDataFilter = vtkCleanPolyData::New();
  undeformedCleanPolyDataFilter->SetInput( UndefGeometry->GetOutput( ) );
  undeformedCleanPolyDataFilter->SetTolerance( 0.0 ); 
  undeformedCleanPolyDataFilter->Update( );
  vtkPolyData *UndefPoly = undeformedCleanPolyDataFilter->GetOutput( );

  if ( debug ) 
  {
    std::cout << "Deformed surface Points: " << DefPoly->GetNumberOfPoints() << std::endl;
    std::cout << "UnDeformed surface Points: " << UndefPoly->GetNumberOfPoints() << std::endl;
  }

  /* Keep only a portion of the points for TPS Algorithm */
  vtkMaskPoints *maskDefPoints = vtkMaskPoints::New();
  maskDefPoints->SetInput( DefPoly );
  maskDefPoints->SetOnRatio( PointRatio );
  maskDefPoints->RandomModeOn( );
  maskDefPoints->Update(  );
  
  vtkPoints *DefPoints = maskDefPoints->GetOutput( )->GetPoints( );

  if ( debug ) std::cout << "Deformed surface Mask Points: " << DefPoints->GetNumberOfPoints() << std::endl;

  vtkPointLocator *DefLocator = vtkPointLocator::New( );
  DefLocator->InitPointInsertion( DefPoints, maskDefPoints->GetOutput( )->GetBounds( ) );
  DefLocator->SetDataSet( maskDefPoints->GetOutput( ) );
  DefLocator->Update( );
      
  vtkIdList *IdList = vtkIdList::New( );
  IdList->Allocate( DefPoints->GetNumberOfPoints( ) );
  
  for ( int i = 0; i < DefPoly->GetNumberOfPoints( ); i++ )
  {
    if( DefLocator->IsInsertedPoint( DefPoly->GetPoint( i ) ) != -1 )
    {
      IdList->InsertNextId( i );
    }
  }

  vtkPoints *UndefPoints = vtkPoints::New( );
  UndefPoints->SetNumberOfPoints( DefPoints->GetNumberOfPoints( ) );
  vtkIdType pointId;
  for ( int i = 0; i < IdList->GetNumberOfIds( ); i++ )
  {
    pointId = IdList->GetId( i );
    UndefPoints->SetPoint( i, UndefPoly->GetPoint( pointId ) );
  }

  vtkThinPlateSplineTransform *thinTransform = vtkThinPlateSplineTransform::New( );
  thinTransform->SetBasisToR( );
  thinTransform->SetSourceLandmarks( UndefPoints );
  thinTransform->SetTargetLandmarks( DefPoints );
  thinTransform->Update( );

  vtkTransformFilter *finalTransform = vtkTransformFilter::New( );
  finalTransform->SetInput( FineGrid );
  finalTransform->SetTransform( thinTransform );
  finalTransform->Update( );

  vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::New();
  ugrid->Allocate( FineGrid->GetNumberOfCells( ), FineGrid->GetNumberOfCells( ) );
  for ( int i=0; i < FineGrid->GetNumberOfCells( ); i++ )
  {
    ugrid->InsertNextCell( FineGrid->GetCellType( i ), FineGrid->GetCell( i )->GetPointIds( ) );
  }
  ugrid->SetPoints( finalTransform->GetOutput( )->GetPoints( ) );

  return ( ugrid );
}


void WriteIntermediateMesh( vtkUnstructuredGrid* IntermediateMesh, std::string name )
{
  vtkUnstructuredGridWriter *Writer = vtkUnstructuredGridWriter::New( );
  Writer->SetInput( IntermediateMesh );
  Writer->SetFileName( name.c_str( ) );
  Writer->Update( );
  Writer->Delete( );
}

vtkGeneralTransform* PerformICP( vtkPolyData *subjectPolyData, vtkUnstructuredGrid *OrigGrid, int numberOfIterations, int numberOfLandmarks, float maxMeanDistance)
{

  vtkUnstructuredGrid* icpGrid = vtkUnstructuredGrid::New();

  icpGrid->Allocate( OrigGrid->GetNumberOfCells( ), OrigGrid->GetNumberOfCells( ) );

  for ( int i=0; i < OrigGrid->GetNumberOfCells( ); i++ )
  {
    icpGrid->InsertNextCell( OrigGrid->GetCellType( i ), OrigGrid->GetCell( i )->GetPointIds( ) );
  }

  icpGrid->SetPoints( OrigGrid->GetPoints() );

  /* Extract the surface points for ICP registration */
  vtkGeometryFilter *extractSurface = vtkGeometryFilter::New( );
  extractSurface->SetInput( OrigGrid );
  extractSurface->Update( );
  
  vtkCleanPolyData *cleanPolyDataFilter = vtkCleanPolyData::New();
  cleanPolyDataFilter->SetInput( extractSurface->GetOutput( ) );
  cleanPolyDataFilter->SetTolerance( 0.0 ); 
  cleanPolyDataFilter->Update( );
  

  vtkIterativeClosestPointTransform *icpTransform = vtkIterativeClosestPointTransform::New();
  icpTransform->SetSource( cleanPolyDataFilter->GetOutput( ) );
  icpTransform->SetTarget( subjectPolyData );
  icpTransform->StartByMatchingCentroidsOn( );
  icpTransform->GetLandmarkTransform( )->SetModeToRigidBody( );
  icpTransform->SetMaximumNumberOfIterations( numberOfIterations );
  icpTransform->CheckMeanDistanceOn();
  icpTransform->SetMaximumMeanDistance( maxMeanDistance );
  icpTransform->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
  icpTransform->SetMaximumNumberOfLandmarks( numberOfLandmarks );
  icpTransform->Update( );

  /*Added*/
  vtkTransformFilter *initialTransformUgrid = vtkTransformFilter::New( );
  initialTransformUgrid->SetInput( icpGrid );
  initialTransformUgrid->SetTransform( icpTransform );
  initialTransformUgrid->Update( );

  vtkUnstructuredGrid* qFinalGrid = vtkUnstructuredGrid::New();
  qFinalGrid->Allocate( icpGrid->GetNumberOfCells( ), icpGrid->GetNumberOfCells( ) );
  for ( int i = 0; i < icpGrid->GetNumberOfCells( ); i++ )
  {
    qFinalGrid->InsertNextCell( icpGrid->GetCellType( i ), icpGrid->GetCell( i )->GetPointIds( ) );
  }
  qFinalGrid->SetPoints( initialTransformUgrid->GetOutput( )->GetPoints( ) );
  /* End Added */

  /* Scale Based on Bounding Box Size */
  double templateBounds[6];
  qFinalGrid->GetBounds(templateBounds);

  double subjectBounds[6];
  subjectPolyData->GetBounds(subjectBounds);

  vtkMatrix4x4 *sMatrix = vtkMatrix4x4::New();
  sMatrix->Identity( );
  sMatrix->SetElement(0, 0, (subjectBounds[1]-subjectBounds[0])/(templateBounds[1]-templateBounds[0]));
  sMatrix->SetElement(1, 1, (subjectBounds[3]-subjectBounds[2])/(templateBounds[3]-templateBounds[2]));
  sMatrix->SetElement(2, 2, (subjectBounds[5]-subjectBounds[4])/(templateBounds[5]-templateBounds[4]));
  
  vtkMatrix4x4 *t1Matrix = vtkMatrix4x4::New();
  t1Matrix->Identity( );
  t1Matrix->SetElement(0, 3, -((templateBounds[1]+templateBounds[0])/2) );
  t1Matrix->SetElement(1, 3, -((templateBounds[2]+templateBounds[3])/2) );
  t1Matrix->SetElement(2, 3, -((templateBounds[4]+templateBounds[5])/2) );

  vtkMatrix4x4 *t2Matrix = vtkMatrix4x4::New();
  t2Matrix->Identity( );
  t2Matrix->SetElement(0, 3, ((templateBounds[1]+templateBounds[0])/2) );
  t2Matrix->SetElement(1, 3, ((templateBounds[2]+templateBounds[3])/2) );
  t2Matrix->SetElement(2, 3, ((templateBounds[4]+templateBounds[5])/2) );
  
  vtkTransform *scaleTransform = vtkTransform::New();
  scaleTransform->SetMatrix( sMatrix );
    
  vtkTransformFilter *scaleTransformUgrid = vtkTransformFilter::New( );
  scaleTransformUgrid->SetInput( qFinalGrid );
  scaleTransformUgrid->SetTransform( scaleTransform );
  scaleTransformUgrid->Update( );
  
  vtkUnstructuredGrid *sFinalGrid = vtkUnstructuredGrid::New( );
  sFinalGrid->Allocate( qFinalGrid->GetNumberOfCells( ), qFinalGrid->GetNumberOfCells( ) );
  for ( int i=0; i < qFinalGrid->GetNumberOfCells( ); i++ )
  {
    sFinalGrid->InsertNextCell( qFinalGrid->GetCellType( i ), qFinalGrid->GetCell( i )->GetPointIds( ) );
  }
  sFinalGrid->SetPoints( qFinalGrid->GetPoints() );
  
  
  vtkGeometryFilter *extract = vtkGeometryFilter::New( );
  extract->SetInput( sFinalGrid );
  extract->Update( );

  vtkCleanPolyData *cleanFinalPolyDataFilter = vtkCleanPolyData::New();
  cleanFinalPolyDataFilter->SetInput( extract->GetOutput( ) );
  cleanFinalPolyDataFilter->SetTolerance( 0.0 ); 
  cleanFinalPolyDataFilter->Update( );

  vtkIterativeClosestPointTransform *icpTransform2 = vtkIterativeClosestPointTransform::New();
  icpTransform2->SetSource( cleanFinalPolyDataFilter->GetOutput( ) );
  icpTransform2->SetTarget( subjectPolyData );
  icpTransform2->StartByMatchingCentroidsOn( );
  icpTransform2->GetLandmarkTransform( )->SetModeToRigidBody( );
  icpTransform2->SetMaximumNumberOfIterations( numberOfIterations );
  icpTransform2->CheckMeanDistanceOn();
  icpTransform2->SetMaximumMeanDistance( maxMeanDistance );
  icpTransform2->SetMeanDistanceModeToRMS();	//SetMeanDistanceModeToAbsoluteValue()
  icpTransform2->SetMaximumNumberOfLandmarks( numberOfLandmarks );
  icpTransform2->Update( );
  
  
  vtkGeneralTransform *finalTransform = vtkGeneralTransform::New();
  finalTransform->PreMultiply( );
  finalTransform->Concatenate( icpTransform2);
  finalTransform->Concatenate( t2Matrix );
  finalTransform->Concatenate( sMatrix );
  finalTransform->Concatenate( t1Matrix );
  finalTransform->Concatenate( icpTransform );
    
  return ( finalTransform );

}

vtkGeneralTransform* PerformProcrustes( vtkPolyData *subjectPolyData, vtkUnstructuredGrid *OrigGrid, int numberOfIterations, int numberOfLandmarks, float maxMeanDistance)
{

  /* Extract the surface points for ICP registration */
  vtkGeometryFilter *extractSurface = vtkGeometryFilter::New( );
  extractSurface->SetInput( OrigGrid );
  extractSurface->Update( );
  
  vtkCleanPolyData *cleanPolyDataFilter = vtkCleanPolyData::New();
  cleanPolyDataFilter->SetInput( extractSurface->GetOutput( ) );
  cleanPolyDataFilter->SetTolerance( 0.0 );
  cleanPolyDataFilter->Update( );
  int templateNumPoints =  cleanPolyDataFilter->GetOutput( )->GetNumberOfPoints();

  vtkPoints *surfacePts = subjectPolyData->GetPoints();
  int surfaceNumPoints =  subjectPolyData->GetNumberOfPoints();
  int ratio =   surfaceNumPoints/templateNumPoints;

  vtkMaskPoints *maskPointsFilter = vtkMaskPoints::New();
  maskPointsFilter->SetInput( subjectPolyData ); 
  maskPointsFilter->SetOnRatio( ratio );
//  maskPointsFilter->RandomModeOn();
  maskPointsFilter->SetMaximumNumberOfPoints( templateNumPoints );
//  maskPointsFilter->SetOffset( eNumPoints -1 );
  maskPointsFilter->Update( );
//  std::cout << "Num points " << templateNumPoints << std::endl;
//  std::cout << "mask Num " << maskPointsFilter->GetOutput()->GetNumberOfPoints() << std::endl;
//  std::cout << "surface pts " << surfaceNumPoints << std::endl;

  vtkProcrustesAlignmentFilter *procrustesTransform = vtkProcrustesAlignmentFilter::New();
  procrustesTransform->SetNumberOfInputs( 2 );
  procrustesTransform->SetInput( 0, maskPointsFilter->GetOutput( ) );
  procrustesTransform->SetInput( 1, cleanPolyDataFilter->GetOutput( ) );
  procrustesTransform->GetLandmarkTransform()->SetModeToAffine();
  procrustesTransform->Update( );
 
  vtkTransformFilter *transformTemplatePts = vtkTransformFilter::New( );
  transformTemplatePts->SetInput( cleanPolyDataFilter->GetOutput( ) );
  transformTemplatePts->SetTransform( procrustesTransform->GetLandmarkTransform() );
  transformTemplatePts->Update( );

  /* Scale Based on Bounding Box Size */
  double templateBounds[6];
  transformTemplatePts->GetOutput( )->GetBounds(templateBounds);

  double subjectBounds[6];
  maskPointsFilter->GetOutput( )->GetBounds(subjectBounds);

  vtkMatrix4x4 *sMatrix = vtkMatrix4x4::New();
  sMatrix->Identity( );
  sMatrix->SetElement(0, 0, (subjectBounds[1]-subjectBounds[0])/(templateBounds[1]-templateBounds[0]));
  sMatrix->SetElement(1, 1, (subjectBounds[3]-subjectBounds[2])/(templateBounds[3]-templateBounds[2]));
  sMatrix->SetElement(2, 2, (subjectBounds[5]-subjectBounds[4])/(templateBounds[5]-templateBounds[4]));


  vtkMatrix4x4 *t1Matrix = vtkMatrix4x4::New();
  t1Matrix->Identity( );
  t1Matrix->SetElement(0, 3, -((templateBounds[1]+templateBounds[0])/2) );
  t1Matrix->SetElement(1, 3, -((templateBounds[2]+templateBounds[3])/2) );
  t1Matrix->SetElement(2, 3, -((templateBounds[4]+templateBounds[5])/2) );

  vtkMatrix4x4 *t2Matrix = vtkMatrix4x4::New();
  t2Matrix->Identity( );
  t2Matrix->SetElement(0, 3, ((templateBounds[1]+templateBounds[0])/2) );
  t2Matrix->SetElement(1, 3, ((templateBounds[2]+templateBounds[3])/2) );
  t2Matrix->SetElement(2, 3, ((templateBounds[4]+templateBounds[5])/2) );
 
 
  vtkGeneralTransform *finalTransform = vtkGeneralTransform::New();
  finalTransform->PreMultiply( );
  finalTransform->Concatenate( procrustesTransform->GetLandmarkTransform() );
  finalTransform->Concatenate( t2Matrix );
  finalTransform->Concatenate( sMatrix );
  finalTransform->Concatenate( t1Matrix );
  
  return ( finalTransform );

}

vtkUnstructuredGrid* ApplyInitializeTransform(vtkGeneralTransform *initialTransform, vtkUnstructuredGrid * originalGrid)
{
  vtkTransformFilter *initialTransformUgrid = vtkTransformFilter::New( );
  initialTransformUgrid->SetInput( originalGrid );
  initialTransformUgrid->SetTransform( initialTransform );
  initialTransformUgrid->Update( );

  vtkUnstructuredGrid* finalGrid = vtkUnstructuredGrid::New();
  finalGrid->Allocate( originalGrid->GetNumberOfCells( ), originalGrid->GetNumberOfCells( ) );
  for ( int i=0; i < originalGrid->GetNumberOfCells( ); i++ )
  {
    finalGrid->InsertNextCell( originalGrid->GetCellType( i ), originalGrid->GetCell( i )->GetPointIds( ) );
  }
  finalGrid->SetPoints( initialTransformUgrid->GetOutput( )->GetPoints() );

  return ( finalGrid );
}

double ComputeAverageDistance(vtkUnstructuredGrid* IntermediateMesh, vtkPolyData* subjectSurface, 
                              vtkOBBTree *obb, bool distanceAlongNormals, bool debug, double *maxDistance)
{

  vtkGeometryFilter *Geom = vtkGeometryFilter::New( );
  Geom->SetInput( IntermediateMesh );
  Geom->Update( );
    
  vtkCleanPolyData *cleanPolyDataFilter = vtkCleanPolyData::New();
  cleanPolyDataFilter->SetInput( Geom->GetOutput( ) );
  cleanPolyDataFilter->SetTolerance( 0.0 ); 
  cleanPolyDataFilter->Update( );
  
  vtkPolyDataNormals* normalFilter = vtkPolyDataNormals::New();
	normalFilter->SetInput( cleanPolyDataFilter->GetOutput() );
	normalFilter->SplittingOff();
	normalFilter->ComputePointNormalsOn();
	normalFilter->Update();

  vtkPolyData *intermediateSurfaceMesh = normalFilter->GetOutput( );
  vtkDataArray *pointNormals = intermediateSurfaceMesh->GetPointData()->GetNormals(); 
    

  vtkPointLocator *PLocator = vtkPointLocator::New();
  PLocator->SetDataSet( subjectSurface );
  PLocator->Update();
  
  vtkMath *math = vtkMath::New(); 

  int count = 0;  
  double cumdist = 0.0;
  *maxDistance = 0.0;
  
  for( int i = 0; i < intermediateSurfaceMesh->GetNumberOfPoints( ); i++ )
  {
    double x[3], ClosestPoint[3];
    
    intermediateSurfaceMesh->GetPoint( i, x );
    vtkIdType ptId = PLocator->FindClosestPoint( x ); 
    subjectSurface->GetPoint( ptId, ClosestPoint );
    double dist = pow ( math->Distance2BetweenPoints( x, ClosestPoint ), 0.5 ); 
          
    if ( distanceAlongNormals )
    {
      //std::cout << "Closest Point Dist: " << dist;
      double normal[3];
      pointNormals->GetTuple( i, normal );
      IntersectPointNormalWithSurface(x, normal, &dist, obb, ClosestPoint);
      //std::cout << " Projected Point Dist: " << dist << std::endl;
    }
    cumdist += dist;
    if ( dist > *maxDistance ) *maxDistance = dist;
    
    count++;
  }
  
  double avgDist = cumdist/(double)count;

  Geom->Delete( );
  cleanPolyDataFilter->Delete( );
  normalFilter->Delete( );
  PLocator->Delete( );

  return avgDist;
}


bool IntersectPointNormalWithSurface(double *pointLocation, double *pointNormal, double *distance,
                                     vtkOBBTree *obb, double *projectedPoint)
{
  double a0[3];
  double a1[3];


  a0[0] = pointLocation[0] - 2 * (*distance) * pointNormal[0];
  a0[1] = pointLocation[1] - 2 * (*distance) * pointNormal[1];
  a0[2] = pointLocation[2] - 2 * (*distance) * pointNormal[2];

  a1[0] = pointLocation[0] + 2 * (*distance) * pointNormal[0];
  a1[1] = pointLocation[1] + 2 * (*distance) * pointNormal[1];
  a1[2] = pointLocation[2] + 2 * (*distance) * pointNormal[2];


  vtkIdList *intersectionIds = vtkIdList::New();
  vtkPoints *intersectionPoints = vtkPoints::New();
  vtkMath *math = vtkMath::New(); 

  int lineIntersects = obb->IntersectWithLine(a0, a1, intersectionPoints, intersectionIds);
  if ( lineIntersects != 0 )
  {
    double minDistance = 10e20;
    vtkIdType minPointId = 0;
    for (int i=0;i<intersectionPoints->GetNumberOfPoints();i++)
    {
      double currentDistance = pow ( math->Distance2BetweenPoints( pointLocation, intersectionPoints->GetPoint(i) ), 0.5 ); 
      if ( currentDistance < minDistance)
      {
        minDistance = currentDistance;
        minPointId = i;
      }
    }
    
    projectedPoint[0] = intersectionPoints->GetPoint(minPointId)[0];
    projectedPoint[1] = intersectionPoints->GetPoint(minPointId)[1];
    projectedPoint[2] = intersectionPoints->GetPoint(minPointId)[2];

    *distance = minDistance;
    return true;
  }
  else
  {
    return false;
  }
  
  
}

