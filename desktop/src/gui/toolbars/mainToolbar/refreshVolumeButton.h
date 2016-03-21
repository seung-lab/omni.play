#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"

class MainWindow;

class VolumeRefreshButton : public OmButton {

 public:
  VolumeRefreshButton(MainWindow* mw);

 private:
  void onLeftClick() override;
};
