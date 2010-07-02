#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "volume/omSegmentation.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentCache.h"
#include "segment/actions/segment/omSegmentJoinAction.h"

JoinButton::JoinButton(DendToolBar * d)
	: OmButton<DendToolBar>( d, 
				 "Join", 
				 "Join objects", 
				 false)
{
}

void JoinButton::doAction()
{
	SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}

	OmSegmentation & seg = sdw.getSegmentation();
	OmIDsSet ids = seg.GetSegmentCache()->GetSelectedSegmentIds();
	(new OmSegmentJoinAction(sdw.getID(), ids))->Run();

	mParent->updateGui();
}
