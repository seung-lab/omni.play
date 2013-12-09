#pragma once

#include "common/logging.h"
#include "gui/sidebars/right/validationGroup/validationGroup.h"
#include "gui/widgets/omButton.hpp"
#include "actions/omActions.h"
#include "events/events.h"

class SetUncertain : public OmButton<ValidationGroup> {
 public:
  SetUncertain(ValidationGroup *d)
      : OmButton<ValidationGroup>(d, "Uncertain",
                                  "Uncertain about selected objects", false) {}

 private:
  void doAction() {
    SegmentationDataWrapper sdw = mParent->GetSDW();
    if (!sdw.IsSegmentationValid()) {
      return;
    }

    OmActions::UncertainSegmentation(sdw, true);

    om::event::SegmentModified();
  }
};
