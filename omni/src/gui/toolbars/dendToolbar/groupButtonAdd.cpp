#include "gui/toolbars/dendToolbar/groupButtonAdd.h"
#include "common/omDebug.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"
#include "segment/omSegmentCache.h"

GroupButtonAdd::GroupButtonAdd(DendToolBar * d)
	: OmButton<DendToolBar>( d, 
				 "Set Selection Valid", 
				 "Locking selected objects", 
				 false)
{
}

void GroupButtonAdd::doAction()
{
	debug("dendbar", "DendToolBar::addGroup\n");
	SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}
	
	OmSegmentation & seg = sdw.getSegmentation();
	seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), VALIDROOT, QString("Valid"));
	OmEvents::SegmentModified();
}
