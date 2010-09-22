#ifndef OM_VIEW2D_EVENTS_HPP
#define OM_VIEW2D_EVENTS_HPP

#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

#include "system/events/omSegmentEvent.h"
#include "system/events/omViewEvent.h"
#include "system/events/omToolModeEvent.h"

#include "gui/widgets/omCursors.h"

// deal w/ Omni events
class OmView2dEvents
	: public OmSegmentEventListener,
	  public OmViewEventListener,
	  public OmToolModeEventListener
{
private:
	OmView2d *const v2d_;
	boost::shared_ptr<OmView2dState> state_;

public:
	OmView2dEvents(OmView2d* v2d,
		       boost::shared_ptr<OmView2dState> state)
 		: v2d_(v2d)
		, state_(state)
	{}

	void SegmentObjectModificationEvent(OmSegmentEvent*){
		v2d_->myUpdate();
	}
	void SegmentDataModificationEvent(){}
	void SegmentEditSelectionChangeEvent(){}
	void VoxelSelectionModificationEvent(){}

	void ViewBoxChangeEvent(){
		v2d_->myUpdate();
	}
	void ViewPosChangeEvent(){
		v2d_->myUpdate();
	}
	void ViewCenterChangeEvent()
	{
		state_->ChangeViewCenter();
		v2d_->myUpdate();
		OmEvents::Redraw3d();
	}
	void ViewRedrawEvent(){
		v2d_->myUpdate();
	}

	void ToolModeChangeEvent(){
		OmCursors::setToolCursor(v2d_);
	}
};
#endif
