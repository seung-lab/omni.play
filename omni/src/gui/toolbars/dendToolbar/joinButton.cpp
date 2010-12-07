#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "volume/omSegmentation.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentCache.h"
#include "actions/omActions.hpp"
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
	SegmentationDataWrapper sdw = mParent->GetSegmentationDataWrapper();
	if(!sdw.IsSegmentationValid()){
		return;
	}

	OmSegmentation & seg = sdw.GetSegmentation();
	const OmSegIDsSet& ids = seg.GetSegmentCache()->GetSelectedSegmentIds();
	OmActions::JoinSegments(sdw.getID(), ids);

	mParent->updateGui();
}
