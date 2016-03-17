#pragma once
#include "precomp.h"

#include "gui/tools.hpp"
#include "common/common.h"

class MainWindow;
class MainToolBar;
class OmViewGroupState;

namespace om {
namespace sidebars {
class right;
}
}

class ToolBarManager : public QWidget {
  Q_OBJECT;

 public:
  ToolBarManager(MainWindow *mw);
  ~ToolBarManager();

  void UpdateReadOnlyRelatedWidgets();
  void UpdateGuiFromProjectLoadOrOpen(OmViewGroupState &);
  void UpdateGuiFromProjectClose();

 private:
  MainWindow *const mainWindow_;

  std::unique_ptr<MainToolBar> mainToolBar_;
  std::unique_ptr<om::sidebars::right> rightSideBar_;

  void deleteMainBar();
};
