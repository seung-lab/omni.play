#include "actions/omActions.h"
#include "common/logging.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinAllButton.h"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"

JoinAllButton::JoinAllButton(QWidget* widget, OmViewGroupState& vgs)
      : ViewGroupStateButton(widget, "Join All", "Join all selected objects",
          false, vgs) {}

void JoinAllButton::onLeftClick() {
  SegmentationDataWrapper sdw = GetViewGroupState().Segmentation();

  if (!sdw.IsSegmentationValid()) {
    return;
  }

  auto seg = sdw.GetSegmentation();
  const om::common::SegIDSet ids =
      seg->Segments().Selection().GetSelectedSegmentIDs();
  OmActions::JoinSegments(sdw, ids);
}
