#pragma once
#include "precomp.h"

#include "gui/sidebars/right/validationGroup/validButton.hpp"
#include "actions/omActions.h"
#include "events/events.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"

ValidButton::ValidButton(QWidget* widget, OmViewGroupState& vgs,
    om::common::SetValid setValid)
      : ViewGroupStateButton(widget,
          om::common::SetValid::SET_VALID == setValid ? "Valid" : "Not Valid",
          om::common::SetValid::SET_VALID == setValid ?
          "Locking Selected Objects" : "Unlock Selected Objects", false, vgs),
      setValid_(setValid) {}

void ValidButton::onLeftClick() {
  SegmentationDataWrapper sdw = GetViewGroupState().Segmentation();

  if (!sdw.IsSegmentationValid()) {
    return;
  }

  OmActions::ValidateSelectedSegments(sdw, om::common::SetValid::SET_VALID);

  om::event::SegmentModified();
}
