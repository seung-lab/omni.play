#include "common/logging.h"
#include "gui/sidebars/right/graphTools/breakButton.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "system/cache/omCacheManager.h"
#include "system/omConnect.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omJoiningSplitting.hpp"

BreakButton::BreakButton(GraphTools* d)
    : OmButton<GraphTools>(d, "Break", "Shatter object mode", true) {
  om::connect(this, SIGNAL(clicked(bool)), this,
              SLOT(SetShouldVolumeBeShownBroken(bool)));
}

void BreakButton::SetShouldVolumeBeShownBroken(const bool shouldVolumeBeShownBroken) {
  if (shouldVolumeBeShownBroken) {
    mParent->ShowBreakThreasholdBox();
  } else {
    mParent->HideBreakThreasholdBox();
  }
  mParent->GetViewGroupState().SetShouldVolumeBeShownBroken(shouldVolumeBeShownBroken);
}
