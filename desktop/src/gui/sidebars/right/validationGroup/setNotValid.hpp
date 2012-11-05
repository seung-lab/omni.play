#pragma once

#include "common/omDebug.h"
#include "gui/sidebars/right/validationGroup/validationGroup.h"
#include "gui/widgets/omButton.hpp"
#include "actions/omActions.h"
#include "events/omEvents.h"

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
        SegmentationDataWrapper sdw = mParent->GetSDW();
        if(!sdw.IsSegmentationValid()){
            return;
        }

        OmActions::ValidateSelectedSegments(sdw, om::common::SET_NOT_VALID);

        OmEvents::SegmentModified();
    }
};

