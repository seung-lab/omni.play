#pragma once
#include "precomp.h"

#include "gui/sidebars/right/validationGroup/uncertainButton.hpp"
#include "actions/omActions.h"
#include "events/events.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"

UncertainButton::UncertainButton(QWidget* widget, OmViewGroupState& vgs, bool isUncertain)
    : ViewGroupStateButton(widget,
        isUncertain ? "Uncertain" : "Not Uncertain",
        isUncertain ? 
        "Uncertain about selected objects" : "Not uncertain about selected objects",
        false, vgs),
    isUncertain_(isUncertain) {}

void UncertainButton::onLeftClick() {
  SegmentationDataWrapper sdw = GetViewGroupState().Segmentation();

  if (!sdw.IsSegmentationValid()) {
    return;
  }

  OmActions::UncertainSegmentation(sdw, isUncertain_);

  om::event::SegmentModified();
}
