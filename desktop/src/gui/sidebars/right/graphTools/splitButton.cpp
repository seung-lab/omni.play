#include "common/logging.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/splitButton.h"
#include "system/omConnect.hpp"
#include "viewGroup/omSplitting.hpp"
#include "viewGroup/omViewGroupState.h"

SplitButton::SplitButton(GraphTools* d)
    : OmButton<GraphTools>(d, "Split", "Split object mode", true) {
  om::connect(this, SIGNAL(clicked(bool)), this,
              SLOT(enterOrExitSplitMode(bool)));
}

void SplitButton::enterOrExitSplitMode(const bool inSplitMode) {
  if (inSplitMode) {
    mParent->GetViewGroupState().TwoSegmentAction()
      .Enter(om::common::JoinOrSplit::SPLIT);
  } else {
    mParent->GetViewGroupState().TwoSegmentAction().Exit();
  }
}
