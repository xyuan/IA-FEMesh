/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateFEMeshFromBBGroup.h,v $
Language:  C++
Date:      $Date: 2008/08/14 05:01:52 $
Version:   $Revision: 1.24 $

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
// .NAME vtkKWMimxCreateFEMeshFromBBGroup - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxCreateFEMeshFromBBGroup_h
#define __vtkKWMimxCreateFEMeshFromBBGroup_h

#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

#define HEX_BB_SOLID_MESH          1
#define HEX_EXTRUDE_SOLID_MESH     2
#define TET_BB_SOLID_MESH          3
#define TET_EXTRUDE_SOLID_MESH     4
#define QUAD_BB_SURFACE_MESH       5
#define QUAD_EXTRACT_SURFACE_MESH  6
#define TRI_BB_SURFACE_MESH        7
#define TRI_EXTRACT_SURFACE_MESH   8

class vtkKWComboBoxWithLabel;
class vtkIntArray;
class vtkKWEntryWithLabel;
class vtkKWMimxNodeElementNumbersGroup;
class vtkKWCheckButtonWithLabel;
class vtkKWRadioButton;
class vtkKWMenuButton;
class vtkKWPushButton;
class vtkKWMimxDefineElSetGroup;
class vtkMimxMeshActor;
class vtkKWMenuButtonWithLabel;


class vtkKWMimxCreateFEMeshFromBBGroup : public vtkKWMimxGroupBase
{
public:

/*  enum { 
    HEX_BB_SOLID_MESH          = 1,
    HEX_EXTRUDE_SOLID_MESH     = 2,
    TET_BB_SOLID_MESH          = 3,
    TET_EXTRUDE_SOLID_MESH     = 4,
    TET_EXTRUDE_SOLID_MESH     = 5,
    QUAD_BB_SURFACE_MESH       = 6,
    QUAD_EXTRACT_SURFACE_MESH  = 7,
    TRI_BB_SURFACE_MESH        = 8,
    TRI_EXTRACT_SURFACE_MESH   = 9
  };*/
    

  static vtkKWMimxCreateFEMeshFromBBGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxCreateFEMeshFromBBGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void CreateFEMeshFromBBDoneCallback();
  int CreateFEMeshFromBBApplyCallback();
  void CreateFEMeshFromBBCancelCallback();
  void SmoothFEMeshCallback(int state);
  void UpdateObjectLists();
  void VolumetricMeshCallback();
  void SurfaceMeshCallback();
  void VolumetricMeshBBCallback();
  void VolumetricMeshExtrudeCallback();
  void InterpolationModeCallback();
  void SurfaceMeshBBCallback();
  void SurfaceMeshExtractCallback();
  void DefineRegionCallback();
  int  GetMeshingMode( );
  void AddMeshToDisplay(vtkUnstructuredGrid *ugrid, const char *namePrefix, int index, const char *FoundationName);
  void InitializeMeshFieldData(vtkUnstructuredGrid *mesh, const char *nodeSetName, 
                     const char *elementSetName, int nodeNumber, int elementNumber);
  int CreateMeshFromBuildingBlock(vtkPolyData *polyDaya, vtkUnstructuredGrid *buildingBlock, 
                     bool smoothMesh, int numSmooth, const char *nodeSetName, int nodeNumber, 
                     const char *elementSetName, int elementNumber, int numRecalc, 
                     bool generateSurfaceMesh, bool generateTriMesh, const char *FoundationName);
  int ExtrudeHexSolidMesh(vtkMimxMeshActor *MeshActor, const char *NodeSetName,
	  const char *ElSetName, int StartNodeNum, int StartElemNum,
	  int NumberOfDiv, double ExtrusionLength, const char *FoundationName);
	int ExtractSurfaceMesh(vtkMimxMeshActor *MeshActor, const char *NodeSetName,
	  const char *ElSetName, int StartNodeNum, int StartElemNum, bool generateTriMesh, const char *FoundationName);
protected:
	vtkKWMimxCreateFEMeshFromBBGroup();
	~vtkKWMimxCreateFEMeshFromBBGroup();
	virtual void CreateWidget();
  
  vtkKWFrame *ModeFrame;
  vtkKWRadioButton *VolumetricMeshButton;
  vtkKWRadioButton *SurfaceMeshButton;
  vtkKWRadioButton *HexMeshButton;
  vtkKWRadioButton *TetMeshButton;
  vtkKWRadioButton *QuadMeshButton;
  vtkKWRadioButton *TriMeshButton;
  vtkKWMenuButton *TypeMenuButton;
  vtkKWMenuButtonWithLabel *InterpolationMenuButton;
  
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *SurfaceListComboBox;
  vtkKWComboBoxWithLabel *BBListComboBox;
  vtkKWComboBoxWithLabel *MeshListComboBox;
  vtkIntArray *OriginalPosition;
 /* vtkKWFrame *ExtrudeFrame;
  vtkKWEntryWithLabel *ExtrudeLengthEntry;
  vtkKWEntryWithLabel *NumberOfDivisionsEntry;*/
  vtkKWFrame *SmoothFrame;
  vtkKWFrame *InterpFrame;
  vtkKWEntryWithLabel *SmoothingIterationsEntry;
  vtkKWEntryWithLabel *ElementSetNameEntry;
  vtkKWEntryWithLabel *RecalculationIterationsEntry;
  vtkKWMimxNodeElementNumbersGroup *NodeElementNumbersGroup;
  vtkKWFrameWithLabel *MeshImprovementFrame;
  vtkKWCheckButtonWithLabel *SmoothButton;
  vtkKWFrame *SelectFrame;
  vtkKWRadioButton *SelectSurfaceButton;
  vtkKWRadioButton *SelectFaceButton;
  vtkKWRadioButton *SelectElementButton;
  vtkKWPushButton *SelectButton;
  vtkKWMimxDefineElSetGroup *DefineElementSetDialog;
  
private:
  vtkKWMimxCreateFEMeshFromBBGroup(const vtkKWMimxCreateFEMeshFromBBGroup&); // Not implemented
  void operator=(const vtkKWMimxCreateFEMeshFromBBGroup&); // Not implemented
  
  int ExtractCount;
  int BBCount;
  int ExtrudeCount;
 };

#endif

