#ifndef OM_MOUSE_EVENT_MOVE_HPP
#define OM_MOUSE_EVENT_MOVE_HPP

#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "view2d/omLineDraw.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

class OmMouseEventMove{
private:
	OmView2d *const v2d_;
	OmView2dState *const state_;

public:
	OmMouseEventMove(OmView2d* v2d, OmView2dState* state)
		: v2d_(v2d)
		, state_(state)
	{}

	void Move(QMouseEvent* event)
	{
		state_->SetMousePoint(event->x(), event->y());

		// http://qt.nokia.com/doc/4.5/qt.html#MouseButton-enum
		if (event->buttons() == Qt::LeftButton) {
			switch (OmStateManager::GetToolMode()) {
			case SPLIT_MODE:
			case CUT_MODE:
				break;

			case SELECT_MODE:
				if(state_->IsCameraMoving()){
					selectSegments(event);
				}
				break;
			case PAN_MODE:
				if(state_->IsCameraMoving()){
					mousePan(event);
					OmEvents::Redraw3d();
				}
				break;

			case CROSSHAIR_MODE:
			case ZOOM_MODE:
			case FILL_MODE:
				break;

			case ADD_VOXEL_MODE:
			case SUBTRACT_VOXEL_MODE:
				if (state_->getScribbling()) {
					// keep painting
					paint(event);
				}
				break;
			}
		}

		v2d_->myUpdate();
	}

private:
	void selectSegments(QMouseEvent* event)
	{
		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		v2d_->LineDrawer()->BresenhamLineDrawForSelecting(state_->GetLastDataPoint(),
														  dataClickPoint);

		state_->SetLastDataPoint(dataClickPoint);
	}

	void paint(QMouseEvent* event)
	{
		SegmentDataWrapper sdw = OmSegmentSelected::Get();
		if (!sdw.IsSegmentValid()){
			return;
		}

		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		OmSegID segmentValueToPaint = 0;
		if( ADD_VOXEL_MODE == OmStateManager::GetToolMode()) {
			segmentValueToPaint = sdw.getID();
		}

		v2d_->LineDrawer()->BrushToolApplyPaint(sdw.GetSegmentationID(),
												dataClickPoint,
												segmentValueToPaint);

		v2d_->LineDrawer()->BresenhamLineDrawForPainting(state_->GetLastDataPoint(),
														 dataClickPoint);

		state_->SetLastDataPoint(dataClickPoint);
	}

	void mousePan(QMouseEvent* event)
	{
		state_->DoMousePan(Vector2i(event->x(),event->y()));
	}
};

#endif
