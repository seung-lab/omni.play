#pragma once
#include "precomp.h"

#include "common/common.h"
#include "gui/tools.hpp"

class MainWindow;
class NavAndEditButtonGroup;
class OmViewGroupState;
class OpenViewGroupButton;
class OpenSingleViewButton;
class OpenDualViewButton;
class SaveButton;
class VolumeRefreshButton;

class MainToolBar : public QToolBar {
  Q_OBJECT;

 public:
  MainToolBar(MainWindow *mw);

  void UpdateToolbar();

 private:
  MainWindow *const mainWindow_;

  SaveButton *const saveButton;
  OpenViewGroupButton *const openViewGroupButton_;
  OpenViewGroupButton *const openSingleViewButton_;
  OpenViewGroupButton *const openDoubleViewButton_;
  VolumeRefreshButton *const volumeRefreshButton;
  NavAndEditButtonGroup *const navEditButtons_;

  void addNavEditButtons();
  void EnableWidgets();
};
