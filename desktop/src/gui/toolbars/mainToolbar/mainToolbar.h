#pragma once

#include "common/common.h"
#include "gui/tools.hpp"

#include <QtGui>

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
  void SetTool(const om::tool::mode tool);

 private:
  MainWindow *const mainWindow_;

  SaveButton *const saveButton;
  OpenViewGroupButton *const openViewGroupButton_;
  OpenSingleViewButton *const openSingleViewButton_;
  OpenDualViewButton *const openDoubleViewButton_;
  VolumeRefreshButton *const volumeRefreshButton;
  NavAndEditButtonGroup *const navEditButtons_;

  void addNavEditButtons();
  void setReadOnlyWidgetsEnabled();
  void setModifyWidgetsEnabled();
};
