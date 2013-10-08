#pragma once

#include "common/omDebug.h"
#include "gui/sidebars/right/validationGroup/validationGroup.h"
#include "gui/widgets/omButton.hpp"
#include "actions/omActions.h"
#include "events/omEvents.h"
#include "utility/segmentationDataWrapper.hpp"

class SetNotUncertain : public OmButton<ValidationGroup> {
 public:
  SetNotUncertain(ValidationGroup *d)
      : OmButton<ValidationGroup>(d, "Not Uncertain",
                                  "Not uncertain about selected objects",
                                  false) {}

 private:
  void doAction() {
    //debug(dendbar, "ValidationGroup::addGroup\n");
    SegmentationDataWrapper sdw = mParent->GetSDW();
    if (!sdw.IsSegmentationValid()) {
      return;
    }

    OmActions::UncertainSegmentation(sdw, false);

    OmEvents::SegmentModified();
  }
};
