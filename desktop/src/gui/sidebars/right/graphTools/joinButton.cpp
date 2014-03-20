#include "actions/omActions.h"
#include "common/logging.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinButton.h"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

JoinButton::JoinButton(GraphTools* d)
    : OmButton<GraphTools>(d, "Join", "Join objects", false) {}

void JoinButton::doAction() {
  SegmentationDataWrapper sdw = mParent->GetSDW();
  if (!sdw.IsSegmentationValid()) {
    return;
  }

  auto seg = sdw.GetSegmentation();
  const om::common::SegIDSet ids =
      seg->Segments().Selection().GetSelectedSegmentIDs();
  OmActions::JoinSegments(sdw, ids);

  mParent->updateGui();
}
