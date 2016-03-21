#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/mainToolbar/refreshVolumeButton.h"
#include "utility/segmentationDataWrapper.hpp"

VolumeRefreshButton::VolumeRefreshButton(MainWindow* parent)
    : OmButton(parent, "Rebuild", "Rebuild image and mesh data",
                           false) {
  setFlat(true);
  setIconAndText(":/toolbars/mainToolbar/icons/1278008555_agt_softwareD.png");
}

void VolumeRefreshButton::onLeftClick() {
  const auto& set = SegmentationDataWrapper::ValidIDs();

  FOR_EACH(iter, set) { log_errors << "fixme!!!!!!!!!!!!!!"; }
}
