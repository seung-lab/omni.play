#include "actions/omActions.h"
#include "common/logging.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinAllButton.h"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

JoinAllButton::JoinAllButton(GraphTools* d)
    : OmButton<GraphTools>(d, "Join ALL", "Join all selected objects", false) {
  /*
   *om::connect(this, SIGNAL(clicked(bool)), this,
   *            SLOT(enterOrExitJoinMode(bool)));
   */
}

void JoinAllButton::doAction() {
  SegmentationDataWrapper sdw = mParent->GetSDW();
  if (!sdw.IsSegmentationValid()) {
    return;
  }

  auto seg = sdw.GetSegmentation();
  const om::common::SegIDSet ids =
      seg->Segments().Selection().GetSelectedSegmentIDs();
  OmActions::JoinSegments(sdw, ids);
}
