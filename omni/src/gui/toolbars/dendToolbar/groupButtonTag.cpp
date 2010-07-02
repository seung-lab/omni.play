#include "gui/toolbars/dendToolbar/groupButtonTag.h"
#include "common/omDebug.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"
#include "segment/omSegmentCache.h"

GroupButtonTag::GroupButtonTag(DendToolBar * d)
	: OmButton<DendToolBar>( d, 
				 "Group As:", 
				 "",
				 false)
{
}

void GroupButtonTag::doAction()
{
	debug("dendbar", "DendToolBar::specialGroupAdd\n");
	SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}
	
	OmSegmentation & seg = sdw.getSegmentation();
	seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), GROUPROOT, mParent->getGroupNameFromGUI());
	OmEvents::SegmentModified();
}
