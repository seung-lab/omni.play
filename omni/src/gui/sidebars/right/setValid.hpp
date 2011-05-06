#ifndef SET_VALID_H
#define SET_VALID_H

#include "common/omDebug.h"
#include "gui/sidebars/right/validationGroup.h"
#include "gui/widgets/omButton.hpp"
#include "actions/omActions.h"
#include "events/omEvents.h"

class SetValid : public OmButton<ValidationGroup> {
public:
    SetValid(ValidationGroup *d)
        : OmButton<ValidationGroup>( d,
                                     "Valid",
                                     "Locking selected objects",
                                     false)
    {}

private:
    void doAction()
    {
        //debug(dendbar, "ValidationGroup::addGroup\n");
        SegmentationDataWrapper sdw = mParent->GetSDW();

        if(!sdw.IsSegmentationValid()){
            return;
        }

        OmActions::ValidateSelectedSegments(sdw, om::SET_VALID);

        OmEvents::SegmentModified();
    }
};

#endif
