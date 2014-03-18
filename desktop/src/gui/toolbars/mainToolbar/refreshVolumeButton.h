#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"

class MainWindow;

class VolumeRefreshButton : public OmButton<MainWindow> {

 public:
  VolumeRefreshButton(MainWindow* mw);

 private:
  void doAction();
};
