#ifndef SET_UNCERTAIN_H
#define SET_UNCERTAIN_H

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "gui/widgets/omButton.hpp"
#include "actions/omActions.hpp"
#include "system/omEvents.h"

class SetUncertain : public OmButton<ValidationGroup> {
public:
	SetUncertain(ValidationGroup *d)
		: OmButton<ValidationGroup>( d,
									 "Uncertain",
									 "Uncertain about selected objects",
									 false)
	{}

private:
	void doAction()
	{
		//debug(dendbar, "ValidationGroup::addGroup\n");
		SegmentationDataWrapper sdw = mParent->GetSegmentationDataWrapper();
		if(!sdw.IsSegmentationValid()){
			return;
		}

		OmActions::UncertainSegment(sdw, true);

		OmEvents::SegmentModified();
	}
};

#endif
