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

			case SELECT_VOXEL_MODE:
				throw OmArgException("not implemented");
			}
		}

		v2d_->myUpdate();
	}

private:
	void selectSegments(QMouseEvent* event)
	{
		SegmentDataWrapper sdw = OmSegmentSelected::Get();
		if (!sdw.isValidWrapper()){
			return;
		}

		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		// TODO: bug here; ask MattW
		v2d_->LineDrawer()->bresenhamLineDraw(state_->GetLastDataPoint(),
											  dataClickPoint,
											  true);

		OmMouseEventUtils::PickToolAddToSelection(sdw,
												  dataClickPoint,
												  v2d_);

		state_->SetLastDataPoint(dataClickPoint);
	}

	void paint(QMouseEvent* event)
	{
		SegmentDataWrapper sdw = OmSegmentSelected::Get();
		if (!sdw.isValidWrapper()){
			return;
		}

		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		OmSegID segmentValueToPaint = 0;
		if( ADD_VOXEL_MODE == OmStateManager::GetToolMode()) {
			segmentValueToPaint = sdw.getID();
		}

		v2d_->LineDrawer()->BrushToolApplyPaint(sdw.getSegmentationID(),
												dataClickPoint,
												segmentValueToPaint);

		v2d_->LineDrawer()->bresenhamLineDraw(state_->GetLastDataPoint(),
											  dataClickPoint,
											  false);

		state_->SetLastDataPoint(dataClickPoint);
	}

	void mousePan(QMouseEvent* event)
	{
		state_->DoMousePan(Vector2i(event->x(),event->y()));
	}
};

#endif
