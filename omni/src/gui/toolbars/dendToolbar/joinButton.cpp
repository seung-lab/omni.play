#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "volume/omSegmentation.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentCache.h"
#include "actions/omSegmentJoinAction.h"
#include "gui/toolbars/dendToolbar/graphTools.h"

JoinButton::JoinButton(GraphTools * d)
	: OmButton<GraphTools>( d,
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
	OmSegIDsSet ids = seg.GetSegmentCache()->GetSelectedSegmentIds();
	(new OmSegmentJoinAction(sdw.getID(), ids))->Run();

	mParent->updateGui();
}
