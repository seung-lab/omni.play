#ifndef OM_MOUSE_EVENT_RELEASE_HPP
#define OM_MOUSE_EVENT_RELEASE_HPP

#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventUtils.hpp"

class OmMouseEventRelease{
private:
	OmView2d *const v2d_;
	boost::shared_ptr<OmView2dState> state_;

public:
	OmMouseEventRelease(OmView2d* v2d,
						boost::shared_ptr<OmView2dState> state)
		: v2d_(v2d)
		, state_(state)
	{}

	void Release(QMouseEvent* event)
	{
		state_->setScribbling(false);

		switch (OmStateManager::GetToolMode()) {
		case SPLIT_MODE:
		case CUT_MODE:
			break;

		case SELECT_MODE:
		case CROSSHAIR_MODE:
		case PAN_MODE:
		case ZOOM_MODE:
			state_->SetCameraMoving(false);
			break;

		case ADD_VOXEL_MODE:
		case SUBTRACT_VOXEL_MODE:
		case FILL_MODE:
			doRelease(event);
			break;
		}
	}

private:
	void doRelease(QMouseEvent* event)
	{
		// END PAINTING

		if (event->button() != Qt::LeftButton) {
			return;
		}

		if(v2d_->amInFillMode()) {
			MouseRelease_LeftButton_Filling(event);
			return;
		}

		if(!state_->getScribbling()) {
			return;
		}

		state_->setScribbling(false);
		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		SegmentDataWrapper sdw = OmSegmentSelected::Get();
		if (!sdw.IsSegmentValid()){
			return;
		}

		switch (OmStateManager::GetToolMode()) {
		case ADD_VOXEL_MODE:
			v2d_->LineDrawer()->BrushToolApplyPaint(sdw.GetSegmentationID(),
													dataClickPoint,
													sdw.getID());
			break;
		case SUBTRACT_VOXEL_MODE:
			v2d_->LineDrawer()->BrushToolApplyPaint(sdw.GetSegmentationID(),
													dataClickPoint,
													0);
			break;
		case SELECT_MODE:
			OmMouseEventUtils::PickToolAddToSelection(sdw,
													  dataClickPoint,
													  v2d_);
			break;
		default:
			return;
		}

		state_->SetLastDataPoint(dataClickPoint);

		v2d_->myUpdate();
	}

	void MouseRelease_LeftButton_Filling(QMouseEvent * event)
	{
		state_->setScribbling(false);
		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		//store current selection
		SegmentDataWrapper sdw = OmSegmentSelected::Get();

		//return if not valid
		if (!sdw.IsSegmentValid() ){
			return;
		}

		if(FILL_MODE != OmStateManager::GetToolMode()){
			return;
		}

		const OmSegID data_value = sdw.getID();

		const OmSegID segid = sdw.GetSegmentation().GetVoxelValue(dataClickPoint);
		if(!segid){
			return;
		}

		const OmSegID rootSegID = sdw.GetSegmentCache()->findRootID(segid);

		v2d_->LineDrawer()->FillToolFill( sdw.GetSegmentationID(),
										  dataClickPoint,
										  data_value,
										  rootSegID );

		v2d_->doRedraw2d();
	}
};

#endif
