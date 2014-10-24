#!/bin/bash

/scratch/vince/IaFeMesh-Build/bin/FERegistrationModule \
  --SubjectFileName /home/vince/development/model/phantom/finesphere.stl \
  --TemplateMeshFileName /home/vince/development/model/phantom/Square1.vtk,/home/vince/development/model/phantom/Square2.vtk,/home/vince/development/model/phantom/Square3.vtk,/home/vince/development/model/phantom/Square4.vtk \
  --VtkFileName /scratch/vince/Models/Warped/Phantom/warpSquareToSphereFinal.vtk \
  --NumIterations 300,300,300,300 \
  --YoungsModulus 10,10,10,10 \
  --numRefinementLevels 4 \
  --InterMeshFileName /scratch/vince/Models/Warped/Phantom/warpSquareToSphere \
  --pointRatio 1 \
  --distanceAlongNormals \
  --convergenceDistance 0.05 \
  --tolerance 0.001 \
  --applyCenterBoundaryConditions \
  --useExternalBoundaryConditions \
  --initialTransform ICP \
  --convergenceType Maximum \
  --debug  >& run.log

exit


#--applyCenterBoundaryConditions
#--useExternalBoundaryConditions


