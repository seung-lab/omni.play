#pragma once
#include "precomp.h"

#include "gui/mainWindow/mainWindow.h"
#include "gui/widgets/omButton.hpp"
#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewGroup.h"

class OpenViewGroupButton : public OmButton<MainWindow> {
 public:
  OpenViewGroupButton(MainWindow* mw)
      : OmButton<MainWindow>(mw, "Display Views", "Open the 2D and 3D views",
                             false) {
    setFlat(true);
    setIconAndText(":/toolbars/mainToolbar/icons/1278009384_kllckety.png");
  }

 private:
  void doAction() { mParent->GetViewGroupState().GetViewGroup().AddAllViews(); }
};
