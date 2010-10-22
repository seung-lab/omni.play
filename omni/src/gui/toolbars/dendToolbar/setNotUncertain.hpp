#ifndef SET_NOT_UNCERTAIN_H
#define SET_NOT_UNCERTAIN_H

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "gui/widgets/omButton.h"
#include "segment/actions/segment/omSegmentUncertainAction.h"
#include "system/omEvents.h"

class SetNotUncertain : public OmButton<ValidationGroup> {
public:
	SetNotUncertain(ValidationGroup *d)
		: OmButton<ValidationGroup>( d,
									 "Not Uncertain",
									 "Not uncertain about selected objects",
									 false)
	{}

private:
	void doAction()
	{
		//debug(dendbar, "ValidationGroup::addGroup\n");
		SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
		if(!sdw.isValid()){
			return;
		}

		OmSegmentUncertainAction::SetUncertain(sdw, false);

		OmEvents::SegmentModified();
	}
};

#endif
