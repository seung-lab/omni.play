#include "gui/toolbars/dendToolbar/groupButtonAdd.h"
#include "common/omDebug.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"
#include "segment/omSegmentCache.h"

GroupButtonAdd::GroupButtonAdd(ValidationGroup * d)
	: OmButton<ValidationGroup>( d,
				 "Set Selection Valid",
				 "Locking selected objects",
				 false)
{
}

void GroupButtonAdd::doAction()
{
	debug("dendbar", "ValidationGroup::addGroup\n");
	SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}

	OmSegmentation & seg = sdw.getSegmentation();
	seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), VALIDROOT, QString("Valid"));
	OmEvents::SegmentModified();
}
