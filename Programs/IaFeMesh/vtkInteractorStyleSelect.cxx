/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkInteractorStyleSelect.cxx,v $
Language:  C++
Date:      $Date: 2006/09/18 19:25:43 $
Version:   $Revision: 1.3 $

Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInteractorStyleSelect.h"

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellArray.h>



vtkCxxRevisionMacro(vtkInteractorStyleSelect, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkInteractorStyleSelect);

//----------------------------------------------------------------------------
vtkInteractorStyleSelect::vtkInteractorStyleSelect()
{
	this->MotionFactor   = 10.0;
	this->State = VTKIS_NONE;
	this->InteractorStyleSelectState = VTKIS_NONE;
	Trace = NULL;
}

//----------------------------------------------------------------------------
vtkInteractorStyleSelect::~vtkInteractorStyleSelect()
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSelect::OnMouseMove() 
{ 
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	switch (this->State) 
	{
	case VTKIS_ROTATE:
		this->FindPokedRenderer(x, y);
		this->Rotate();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;

	case VTKIS_PAN:
		this->FindPokedRenderer(x, y);
		this->Pan();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;

	case VTKIS_DOLLY:
		this->FindPokedRenderer(x, y);
		this->Dolly();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;
	case VTKIS_PICKPOINT:
		this->FindPokedRenderer(x, y);
//		should include code for line drawn from last point picked to the mouse tip
//		this->Interactor->AddObserver(vtkCommand::StartPickEvent,this->Trace);
		break;

	}
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSelect::OnLeftButtonDown () 
{ 
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], 
		this->Interactor->GetEventPosition()[1]);
	this->State = this->InteractorStyleSelectState;
	if (this->CurrentRenderer == NULL)
	{
		return;
	}
	switch (this->State) 
	{
	case VTKIS_ROTATE:
		this->StartRotate();
		break;

	case VTKIS_PAN:
		this->StartPan();
		break;
	case VTKIS_DOLLY:
		this->StartDolly();
		break;
	case VTKIS_PICKPOINT:
		this->PickPoint(this->Trace);
		break;
	}
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSelect::OnLeftButtonUp ()
{
	switch (this->State) 
	{
	case VTKIS_ROTATE:
		this->EndRotate();
		break;

	case VTKIS_PAN:
		this->EndPan();
		break;
	case VTKIS_PICKPOINT:
//		this->Interactor->AddObserver(vtkCommand::EndPickEvent,this->Trace);
		break;
	case VTKIS_DOLLY:
		this->EndDolly();
		break;
	}
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSelect::Rotate()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

	int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

	double delta_elevation = -20.0 / size[1];
	double delta_azimuth = -20.0 / size[0];

	double rxf = (double)dx * delta_azimuth * this->MotionFactor;
	double ryf = (double)dy * delta_elevation * this->MotionFactor;

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->Azimuth(rxf);
	camera->Elevation(ryf);
	camera->OrthogonalizeViewUp();

	if (this->AutoAdjustCameraClippingRange)
	{
		this->CurrentRenderer->ResetCameraClippingRange();
	}

	if (rwi->GetLightFollowCamera()) 
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSelect::Pan()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	double viewFocus[4], focalDepth, viewPoint[3];
	double newPickPoint[4], oldPickPoint[4], motionVector[3];

	// Calculate the focal depth since we'll be using it a lot

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->GetFocalPoint(viewFocus);
	this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], 
		viewFocus);
	focalDepth = viewFocus[2];

	this->ComputeDisplayToWorld((double)rwi->GetEventPosition()[0], 
		(double)rwi->GetEventPosition()[1],
		focalDepth, 
		newPickPoint);

	// Has to recalc old mouse point since the viewport has moved,
	// so can't move it outside the loop

	this->ComputeDisplayToWorld((double)rwi->GetLastEventPosition()[0],
		(double)rwi->GetLastEventPosition()[1],
		focalDepth, 
		oldPickPoint);

	// Camera motion is reversed

	motionVector[0] = oldPickPoint[0] - newPickPoint[0];
	motionVector[1] = oldPickPoint[1] - newPickPoint[1];
	motionVector[2] = oldPickPoint[2] - newPickPoint[2];

	camera->GetFocalPoint(viewFocus);
	camera->GetPosition(viewPoint);
	camera->SetFocalPoint(motionVector[0] + viewFocus[0],
		motionVector[1] + viewFocus[1],
		motionVector[2] + viewFocus[2]);

	camera->SetPosition(motionVector[0] + viewPoint[0],
		motionVector[1] + viewPoint[1],
		motionVector[2] + viewPoint[2]);

	if (rwi->GetLightFollowCamera()) 
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSelect::Dolly()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	double *center = this->CurrentRenderer->GetCenter();

	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
	double dyf = this->MotionFactor * (double)(dy) / (double)(center[1]);
	double zoomFactor = pow((double)1.1, dyf);

	if (camera->GetParallelProjection())
	{
		camera->SetParallelScale(camera->GetParallelScale() / zoomFactor);
	}
	else
	{
		camera->Dolly(zoomFactor);
		if (this->AutoAdjustCameraClippingRange)
		{
			this->CurrentRenderer->ResetCameraClippingRange();
		}
	}

	if (rwi->GetLightFollowCamera()) 
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}

void vtkInteractorStyleSelect::SetInteractorStyleSelectState(int newState)
{
	switch (newState) 
	{
	case VTKIS_ROTATE:
		this->InteractorStyleSelectState = newState;
		break;

	case VTKIS_PAN:
		this->InteractorStyleSelectState = newState;
		break;
	case VTKIS_DOLLY:
		this->InteractorStyleSelectState = newState;
		break;
	case VTKIS_PICKPOINT:
		this->InteractorStyleSelectState = newState;
		break;
	case VTKIS_BOUNDINGBOXINTERACT:
		this->InteractorStyleSelectState = newState;
		break;
	case VTKIS_NONE:
		this->InteractorStyleSelectState = newState;
	}

}

int vtkInteractorStyleSelect::GetInteractorStyleSelectState()
{
	return this->InteractorStyleSelectState;
}

void vtkInteractorStyleSelect::PickPoint(vtkActor* Trace)
{
	double x[3];
	if(this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1], 0.0, this->CurrentRenderer))
	{
		this->Interactor->GetPicker()->GetPickPosition(x);
		vtkPolyDataMapper* TraceMapperSafeDownCast = vtkPolyDataMapper::SafeDownCast(Trace->GetMapper());
		TraceMapperSafeDownCast->GetInput()->GetPoints()->InsertPoint(TraceMapperSafeDownCast->GetInput()->GetPoints()->GetNumberOfPoints(),x);
		if(TraceMapperSafeDownCast->GetInput()->GetPoints()->GetNumberOfPoints() > 1)
		{
/*			TraceMapperSafeDownCast->GetInput()->GetPolys()->InsertNextCell(2);
			TraceMapperSafeDownCast->GetInput()->GetPolys()->InsertCellPoint(TraceMapperSafeDownCast->GetInput()->GetPoints()->GetNumberOfPoints()-2);
			TraceMapperSafeDownCast->GetInput()->GetPolys()->InsertCellPoint(TraceMapperSafeDownCast->GetInput()->GetPoints()->GetNumberOfPoints()-1);
			TraceMapperSafeDownCast->GetInput()->SetPoints(TraceMapperSafeDownCast->GetInput()->GetPoints());
			TraceMapperSafeDownCast->GetInput()->SetPolys(TraceMapperSafeDownCast->GetInput()->GetPolys());*/
			TraceMapperSafeDownCast->GetInput()->GetLines()->InsertNextCell(2);
			TraceMapperSafeDownCast->GetInput()->GetLines()->InsertCellPoint(TraceMapperSafeDownCast->GetInput()->GetPoints()->GetNumberOfPoints()-2);
			TraceMapperSafeDownCast->GetInput()->GetLines()->InsertCellPoint(TraceMapperSafeDownCast->GetInput()->GetPoints()->GetNumberOfPoints()-1);
			TraceMapperSafeDownCast->GetInput()->SetPoints(TraceMapperSafeDownCast->GetInput()->GetPoints());
			TraceMapperSafeDownCast->GetInput()->SetLines(TraceMapperSafeDownCast->GetInput()->GetLines());
			Trace->GetProperty()->SetRepresentationToWireframe();
			Trace->GetProperty()->SetLineWidth(4.0);
			double XYZ[6];
			this->GetCurrentRenderer()->AddActor(Trace);
			TraceMapperSafeDownCast->GetInput()->Modified();
			this->Interactor->Render();
		}
	}
}

void vtkInteractorStyleSelect::SetActor(vtkActor* trace)
{
	this->Trace = trace;
}

vtkActor* vtkInteractorStyleSelect::GetActor()
{
	return this->Trace;
}
//----------------------------------------------------------------------------
void vtkInteractorStyleSelect::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

}


