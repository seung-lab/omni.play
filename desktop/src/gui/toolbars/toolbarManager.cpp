#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/right.hpp"
#include "gui/toolbars/toolbarManager.h"
#include "system/omAppState.hpp"
#include "viewGroup/omViewGroupState.h"

ToolBarManager::ToolBarManager(MainWindow* mainWindow)
    : QWidget(mainWindow), mainWindow_(mainWindow) {
  OmAppState::SetToolBarManager(this);
}

ToolBarManager::~ToolBarManager() {}

void ToolBarManager::UpdateReadOnlyRelatedWidgets() {
  if (mainToolBar_) {
    mainToolBar_->UpdateToolbar();
  }
}

void ToolBarManager::deleteMainBar() {
  if (mainToolBar_) {
    mainWindow_->removeToolBar(mainToolBar_.get());
    mainToolBar_.reset();
  }
}

void ToolBarManager::UpdateGuiFromProjectLoadOrOpen(OmViewGroupState& vgs) {
  deleteMainBar();
  mainToolBar_.reset(new MainToolBar(mainWindow_));

  rightSideBar_.reset(new om::sidebars::right(mainWindow_, vgs));

  vgs.SetToolBarManager(this);
}

void ToolBarManager::UpdateGuiFromProjectClose() { deleteMainBar(); }

void ToolBarManager::SetTool(const om::tool::mode tool) {
  mainToolBar_->SetTool(tool);
}
