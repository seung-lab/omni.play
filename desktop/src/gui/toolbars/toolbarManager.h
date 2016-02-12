#pragma once
#include "precomp.h"

#include "gui/tools.hpp"
#include "common/common.h"
#include "common/enums.hpp"

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

  void SetJoiningSplittingOff(om::common::JoinOrSplit joinOrSplit);
  void SetShatteringOff();

  void SetTool(const om::tool::mode tool);

 private:
  MainWindow *const mainWindow_;

  std::unique_ptr<MainToolBar> mainToolBar_;
  std::unique_ptr<om::sidebars::right> rightSideBar_;

  void deleteMainBar();
};
