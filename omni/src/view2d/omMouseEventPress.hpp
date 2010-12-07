#ifndef OM_MOUSE_EVENT_PRESS_HPP
#define OM_MOUSE_EVENT_PRESS_HPP

#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "gui/widgets/omSegmentContextMenu.h"
#include "actions/omActions.hpp"

class OmMouseEventPress{
private:
	OmView2d *const v2d_;
	boost::shared_ptr<OmView2dState> state_;
	OmSegmentContextMenu mSegmentContextMenu;

public:
	OmMouseEventPress(OmView2d* v2d,
					  boost::shared_ptr<OmView2dState> state)
		: v2d_(v2d)
		, state_(state)
	{}

	void Press(QMouseEvent* event)
	{
		state_->SetClickPoint(event->x(), event->y());

		if( SPLIT_MODE == OmStateManager::GetToolMode()){
			if (event->button() == Qt::LeftButton) {
				doFindAndSplitSegment( event );
				v2d_->doRedraw2d();
				return;
			}
		} else if(CUT_MODE == OmStateManager::GetToolMode()) {
			if (event->button() == Qt::LeftButton) {
				doFindAndCutSegment( event );
				v2d_->doRedraw2d();
				return;
			}
		}

		if( event->button() == Qt::LeftButton) {
			const bool crosshair = event->modifiers() & Qt::ControlModifier;
			if( crosshair ){
				mouseSetCrosshair(event);
			} else {
				mouseLeftButton(event);
			}
		} else if (event->button() == Qt::RightButton) {
			if(event->modifiers() & Qt::ControlModifier) {
				mouseSelectSegment(event);
			} else {
				mouseShowSegmentContextMenu(event);
			}
		}

		state_->SetCameraMoving(true);
	}

private:
	void doFindAndSplitSegment(QMouseEvent* event)
	{
		boost::optional<SegmentDataWrapper> sdw = getSelectedSegment(event);

		if(!sdw) {
			return;
		}

		OmActions::FindAndSplitSegments(*sdw, state_->getViewGroupState());
	}

	void doFindAndCutSegment(QMouseEvent* event)
        {
                boost::optional<SegmentDataWrapper> sdw = getSelectedSegment(event);

                if(!sdw) {
                        return;
                }

                OmActions::FindAndCutSegments(*sdw, state_->getViewGroupState());
        }

	void mouseSetCrosshair(QMouseEvent * event)
	{
		v2d_->doRedraw2d();
		v2d_->SetDepth(event);
		state_->getViewGroupState()->setTool(PAN_MODE);
	}

	void mouseLeftButton(QMouseEvent * event)
	{
		bool doselection = false;
		bool dosubtract = false;

		OmSegID data_value;

		switch (OmStateManager::GetToolMode()) {
		case SELECT_MODE:
			state_->setScribbling(true);
			mouseSelectSegment(event);
			return;
		case PAN_MODE:
			state_->SetClickPoint(event->x(), event->y());
			return;
		case CROSSHAIR_MODE:
			mouseSetCrosshair(event);
			return;
		case ZOOM_MODE:
			v2d_->Zoom()->MouseLeftButtonClick(event);
			OmEvents::Redraw3d();
			return;
		case ADD_VOXEL_MODE:
			state_->setScribbling(true);
			break;
		case SUBTRACT_VOXEL_MODE:
			state_->setScribbling(true);
			dosubtract = true;
			break;
		case SELECT_VOXEL_MODE:
		default:
			return;
		}

		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		SegmentDataWrapper sdw = OmSegmentSelected::Get();
		if ( sdw.IsSegmentValid() ) {
			//run action
			if (!doselection) {
				if (dosubtract) {
					data_value = 0;
				} else {
					data_value = sdw.getID();
				}
				v2d_->LineDrawer()->BrushToolApplyPaint(sdw.GetSegmentationID(),
														dataClickPoint,
														data_value);
			} else {
				OmMouseEventUtils::PickToolAddToSelection(sdw,
														  dataClickPoint,
														  v2d_);
				v2d_->LineDrawer()->bresenhamLineDraw(state_->GetLastDataPoint(),
														 dataClickPoint,
														 true);
			}
		} else {
			//debug(genone, "No segment_id in edit selection\n");
		}

		state_->SetLastDataPoint(dataClickPoint);

		v2d_->myUpdate();
	}

	void mouseSelectSegment(QMouseEvent * event)
	{
		bool augment_selection = event->modifiers() & Qt::ShiftModifier;

		boost::optional<SegmentDataWrapper> sdw = getSelectedSegment(event);
		if(!sdw){
			return;
		}

		return doSelectSegment(*sdw, augment_selection );
	}

	void doSelectSegment(const SegmentDataWrapper& sdw, bool augment_selection )
	{
		if( !sdw.IsSegmentValid() ){
			printf("not valid\n");
			return;
		}

		const OmID segmentID = sdw.getID();

		OmSegmentSelected::Set(sdw);

		OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view2dEvent" );
		if( augment_selection ){
			sel.augmentSelectedSet_toggle( segmentID);
		} else {
			sel.selectJustThisSegment_toggle( segmentID );
		}
		sel.sendEvent();

		state_->touchFreshnessAndRedraw2d();
		v2d_->myUpdate();
	}

	void mouseShowSegmentContextMenu(QMouseEvent * event)
	{
		boost::optional<SegmentDataWrapper> sdw = getSelectedSegment(event);
		if(sdw){
			mSegmentContextMenu.Refresh(*sdw, state_->getViewGroupState());
			mSegmentContextMenu.exec(event->globalPos());
		}
	}

	boost::optional<SegmentDataWrapper> getSelectedSegment( QMouseEvent * event )
	{
		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		if(SEGMENTATION == state_->getVol()->getVolumeType()){
			SegmentationDataWrapper sdw(state_->getVol()->getID());
			return getSelectedSegmentSegmentation(dataClickPoint, sdw);
		}

		boost::optional<SegmentDataWrapper> ret;
		OmChannel& channel = OmProject::GetChannel(state_->getVol()->getID());
		foreach( OmID id, channel.GetValidFilterIds() ) {

			OmFilter2d &filter = channel.GetFilter(id);
			SegmentationDataWrapper sdw = filter.GetSegmentationWrapper();
			if (!sdw.IsSegmentationValid()){
				continue;
			}

			ret = getSelectedSegmentSegmentation(dataClickPoint, sdw);

			if(ret){
				break;
			}
		}

		return ret;
	}

	boost::optional<SegmentDataWrapper>
	getSelectedSegmentSegmentation(const DataCoord& dataClickPoint,
								   const SegmentationDataWrapper& sdw)
	{
		const OmSegID segmentID =
			sdw.GetSegmentation().GetVoxelValue(dataClickPoint);

		printf("id = %u\n", segmentID);

		if(0 == segmentID){
			return boost::optional<SegmentDataWrapper>();
		}

		SegmentDataWrapper ret(sdw, segmentID);
		return boost::optional<SegmentDataWrapper>(ret);
	}
};

#endif
