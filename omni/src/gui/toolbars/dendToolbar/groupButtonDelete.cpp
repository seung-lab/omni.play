#include "gui/toolbars/dendToolbar/groupButtonDelete.h"
#include "common/omDebug.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"
#include "segment/omSegmentCache.h"

GroupButtonDelete::GroupButtonDelete(DendToolBar * d)
	: OmButton<DendToolBar>( d, 
				 "Set Selection Not Valid", 
				 "Unlock selected objects", 
				 false)
{
}

void GroupButtonDelete::doAction()
{
        debug("dendbar", "DendToolBar::deleteGroup\n");
	SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}

	OmSegmentation & seg = sdw.getSegmentation();
	seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), NOTVALIDROOT, QString("Not Valid"));
	OmEvents::SegmentModified();
}
