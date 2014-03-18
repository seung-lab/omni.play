#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "gui/toolbars/mainToolbar/mainToolbar.h"

class SaveButton : public OmButton<MainToolBar> {
 public:
  SaveButton(MainToolBar* mw);

 private:
  void doAction();
};
