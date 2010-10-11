#ifndef OM_MOUSE_EVENT_PRESS_HPP
#define OM_MOUSE_EVENT_PRESS_HPP

#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "gui/widgets/omSegmentContextMenu.h"
#include "segment/actions/segment/omSegmentSplitAction.h"

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
				v2d_->doRedraw();
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
		SegmentDataWrapper * sdw = getSelectedSegment(event);

		if(NULL == sdw) {
			return;
		}

		OmSegmentSplitAction::DoFindAndSplitSegment(*sdw,
													state_->getViewGroupState());
	}
	void mouseSetCrosshair(QMouseEvent * event)
	{
		v2d_->doRedraw();
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
			OmMouseZoom::MouseZoom(state_, event);
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

		SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();
		if ( sdw.isValid() ) {
			//run action
			if (!doselection) {
				if (dosubtract) {
					data_value = 0;
				} else {
					data_value = sdw.getID();
				}
				v2d_->LineDrawer()->BrushToolApplyPaint(sdw.getSegmentationID(),
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

		SegmentDataWrapper * sdw = getSelectedSegment( event );
		if( NULL == sdw ){
			return;
		}

		return doSelectSegment( (*sdw), augment_selection );
	}

	void doSelectSegment( SegmentDataWrapper sdw, bool augment_selection )
	{
		OmSegmentation & segmentation = sdw.getSegmentation();

		if( !sdw.isValid() ){
			printf("not valid\n");
			return;
		}

		const OmId segmentID = sdw.getID();

		OmSegmentEditor::SetEditSelection( segmentation.GetId(), segmentID);

		OmSegmentSelector sel( segmentation.GetId(), this, "view2dEvent" );
		if( augment_selection ){
			sel.augmentSelectedSet_toggle( segmentID);
		} else {
			sel.selectJustThisSegment_toggle( segmentID );
		}
		sel.sendEvent();

		state_->touchFreshnessAndRedraw();
		v2d_->myUpdate();
	}

	void mouseShowSegmentContextMenu(QMouseEvent * event)
	{
		SegmentDataWrapper * sdw = getSelectedSegment(event);
		if(sdw) {
			mSegmentContextMenu.Refresh(*sdw, state_->getViewGroupState());
			mSegmentContextMenu.exec(event->globalPos());
		}
	}

	SegmentDataWrapper* getSelectedSegment( QMouseEvent * event )
	{
		const DataCoord dataClickPoint =
			state_->ComputeMouseClickPointDataCoord(event);

		if(SEGMENTATION == state_->getVol()->getVolumeType()){
			return getSelectedSegmentSegmentation(dataClickPoint,
												  state_->getVol()->getID());
		}

		SegmentDataWrapper* ret = NULL;
		OmChannel& channel = OmProject::GetChannel(state_->getVol()->getID());
		foreach( OmId id, channel.GetValidFilterIds() ) {

			OmFilter2d &filter = channel.GetFilter(id);
			OmId segmentationID = filter.GetSegmentation();
			if (!OmProject::IsSegmentationValid(segmentationID)){
				continue;
			}

			ret = getSelectedSegmentSegmentation(dataClickPoint,
												 segmentationID);
			if(ret){
				break;
			}
		}

		return ret;
	}

	SegmentDataWrapper*
	getSelectedSegmentSegmentation(const DataCoord& dataClickPoint,
								   const OmId segmentationID)
	{
		OmSegmentation & segmentation =
			OmProject::GetSegmentation(segmentationID);
		const OmSegID segmentID = segmentation.GetVoxelValue(dataClickPoint);
		if(0 == segmentID){
			return NULL;
		}
		return new SegmentDataWrapper(segmentationID, segmentID);
	}
};

#endif
