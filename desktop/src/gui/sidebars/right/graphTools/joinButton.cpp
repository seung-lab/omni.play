#include "actions/omActions.h"
#include "gui/widgets/omButton.hpp"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinButton.h"
#include "system/omStateManager.h"
#include "system/omConnect.hpp"


JoinButton::JoinButton(GraphTools* d)
    : OmButton<GraphTools>(d, "Join", "Join objects", true) {
  om::connect(this, SIGNAL(clicked(bool)), this,
              SLOT(enterOrExitJoinMode(bool)));
}

void JoinButton::enterOrExitJoinMode(const bool isJoining) {
  if (isJoining) {
    OmStateManager::SetToolModeAndSendEvent(om::tool::mode::JOIN);
  } else {
    OmStateManager::SetOldToolModeAndSendEvent();
  }
}
