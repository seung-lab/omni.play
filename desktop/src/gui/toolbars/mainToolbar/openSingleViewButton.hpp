#pragma once

#include "gui/mainWindow/mainWindow.h"
#include "gui/widgets/omButton.hpp"
#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewGroup.h"

class OpenSingleViewButton : public OmButton<MainWindow> {
 public:
  OpenSingleViewButton(MainWindow* mw)
      : OmButton<MainWindow>(mw, "Display View", "Open the 2D XY Views",
                             false) {
    setFlat(true);
    setIconAndText(":/toolbars/mainToolbar/icons/1299635956_window_new.png");
  }

 private:
  void doAction() { mParent->GetViewGroupState()->GetViewGroup()->AddXYView(); }
};
