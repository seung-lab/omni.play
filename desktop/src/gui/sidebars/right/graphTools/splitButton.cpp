#include "common/logging.h"
#include "gui/widgets/omButton.hpp"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/splitButton.h"
#include "system/omConnect.hpp"
#include "system/omStateManager.h"

SplitButton::SplitButton(GraphTools* d)
    : OmButton<GraphTools>(d, "Split", "Split object mode", true) {
  om::connect(this, SIGNAL(clicked(bool)), this,
              SLOT(enterOrExitSplitMode(bool)));
}

void SplitButton::enterOrExitSplitMode(const bool isSplitting) {
  if (isSplitting) {
    OmStateManager::SetToolModeAndSendEvent(om::tool::mode::SPLIT);
  } else {
    OmStateManager::SetOldToolModeAndSendEvent();
  }
}
