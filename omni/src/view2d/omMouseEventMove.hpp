#ifndef OM_MOUSE_EVENT_MOVE_HPP
#define OM_MOUSE_EVENT_MOVE_HPP

#include "segment/actions/omSegmentEditor.h"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "view2d/omLineDraw.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

class OmMouseEventMove{
private:
	OmView2d *const v2d_;
	boost::shared_ptr<OmView2dState> state_;
public:
	OmMouseEventMove(OmView2d* v2d,
			 boost::shared_ptr<OmView2dState> state)
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
				break;

			case SELECT_MODE:
				if(state_->IsCameraMoving()){
					MouseMove_LeftButton_Scribbling(event);
				}
				break;
			case PAN_MODE:
				if(state_->IsCameraMoving()){
					mouseMove_CamMoving(event);
					OmEvents::Redraw3d();
				}
				break;
			case CROSSHAIR_MODE:
			case ZOOM_MODE:
			case ADD_VOXEL_MODE:
			case SUBTRACT_VOXEL_MODE:
			case FILL_MODE:
				if (PAN_MODE == OmStateManager::GetToolMode()) {
					mouseMove_CamMoving(event);
				} else if (state_->getScribbling()) {
					// keep painting
					MouseMove_LeftButton_Scribbling(event);
				}
				break;

			case SELECT_VOXEL_MODE:
				assert(0 && "not implemented");
			}
		}

		v2d_->myUpdate();
	}

private:
	void MouseMove_LeftButton_Scribbling(QMouseEvent * event)
	{
		bool doselection = false;

		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		//store current selection
		SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();

		//return if not valid
		if (!sdw.isValid())
			return;

		//switch on tool mode
		OmSegID data_value;
		switch (OmStateManager::GetToolMode()) {
		case ADD_VOXEL_MODE:
			//get value associated to segment id
			data_value = sdw.getID();
			break;
		case SUBTRACT_VOXEL_MODE:
			data_value = NULL_SEGMENT_VALUE;
			break;
		case SELECT_MODE:
			doselection = true;
			break;
		default:
			return;
		}

		if (!doselection) {
			//run action
			v2d_->getLineDrawer()->BrushToolApplyPaint(sdw.getSegmentationID(),
								   dataClickPoint,
								   data_value);
			v2d_->getLineDrawer()->bresenhamLineDraw(state_->GetLastDataPoint(),
								 dataClickPoint);
		} else {
			// TODO: bug here; ask MattW
			v2d_->getLineDrawer()->bresenhamLineDraw(state_->GetLastDataPoint(),
								 dataClickPoint,
								 doselection);
			OmMouseEventUtils::PickToolAddToSelection(sdw,
								  dataClickPoint,
								  v2d_);
		}

		state_->SetLastDataPoint(dataClickPoint);

		v2d_->myUpdate();
	}

	void mouseMove_CamMoving(QMouseEvent * event)
	{
		const Vector2i zoomMipVector = state_->getViewGroupState()->GetZoomLevel();
		const Vector2f current_pan = state_->ComputePanDistance();
		const Vector2i drag =
			state_->GetClickPoint() - Vector2i(event->x(),event->y());

		const SpaceCoord dragCoord = state_->ScreenToSpaceCoord(drag);
		const SpaceCoord zeroCoord = state_->ScreenToSpaceCoord(Vector2i(0,0));

		const SpaceCoord difference = zeroCoord - dragCoord;
		const SpaceCoord oldDepth = state_->getViewGroupState()->GetViewDepthCoord();
		const SpaceCoord depth = oldDepth - difference;
		state_->setSliceDepth(depth);
		OmEvents::ViewCenterChanged();

		state_->SetClickPoint(event->x(), event->y());
	}
};

#endif
