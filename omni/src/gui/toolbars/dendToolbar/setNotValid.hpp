#ifndef SET_NOT_VALID_HPP
#define SET_NOT_VALID_HPP

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "gui/widgets/omButton.h"
#include "actions/omSegmentValidateAction.h"
#include "system/omEvents.h"

class SetNotValid : public OmButton<ValidationGroup> {
public:
	SetNotValid(ValidationGroup *d)
		: OmButton<ValidationGroup>( d,
									 "Not Valid",
									 "Unlock selected objects",
									 false)
	{}

private:
	void doAction()
	{
		//debug(dendbar, "ValidationGroup::deleteGroup\n");
		SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
		if(!sdw.isValid()){
			return;
		}

		OmSegmentValidateAction::Validate(sdw, false);

		OmEvents::SegmentModified();
	}
};

#endif
