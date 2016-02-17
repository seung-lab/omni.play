#include "actions/omActions.h"
#include "common/logging.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinButton.h"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "viewGroup/omJoiningSplitting.hpp"
#include "viewGroup/omViewGroupState.h"
#include "system/omConnect.hpp"

JoinButton::JoinButton(GraphTools* d)
    : OmButton<GraphTools>(d, "Join", "Join objects", true) {
  om::connect(this, SIGNAL(clicked(bool)), this,
              SLOT(enterOrExitJoinMode(bool)));
}

void JoinButton::doAction() {
  /*SegmentationDataWrapper sdw = mParent->GetSDW();
  if (!sdw.IsSegmentationValid()) {
    return;
  }

  auto seg = sdw.GetSegmentation();
  const om::common::SegIDSet ids =
      seg->Segments().Selection().GetSelectedSegmentIDs();
  OmActions::JoinSegments(sdw, ids);

  mParent->updateGui();*/
}

void JoinButton::enterOrExitJoinMode(const bool isJoining) {
  if (isJoining) {
    OmStateManager::SetToolModeAndSendEvent(om::tool::mode::JOIN);
  } else {
    OmStateManager::SetOldToolModeAndSendEvent();
  }
}
