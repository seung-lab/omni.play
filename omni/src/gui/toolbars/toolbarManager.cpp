#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/mainToolbar/mainToolbar.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/omAppState.hpp"
#include "viewGroup/omViewGroupState.h"

ToolBarManager::ToolBarManager(MainWindow* mainWindow)
    : QWidget(mainWindow)
    , mainWindow_(mainWindow)
{
    OmAppState::SetToolBarManager(this);
}

ToolBarManager::~ToolBarManager()
{}

void ToolBarManager::UpdateReadOnlyRelatedWidgets()
{
   if(mainToolBar_){
       mainToolBar_->UpdateToolbar();
    }
}

void ToolBarManager::deleteDendBar()
{
   if(dendToolBar_){
        mainWindow_->removeToolBar(dendToolBar_.get());
        dendToolBar_.reset();
    }
}

void ToolBarManager::deleteMainBar()
{
   if(mainToolBar_){
        mainWindow_->removeToolBar(mainToolBar_.get());
        mainToolBar_.reset();
    }
}

void ToolBarManager::UpdateGuiFromProjectLoadOrOpen(OmViewGroupState* vgs)
{
    deleteMainBar();
    mainToolBar_.reset(new MainToolBar(mainWindow_));

    deleteDendBar();
    dendToolBar_.reset(new DendToolBar(mainWindow_, vgs));

    vgs->SetToolBarManager(this);
}

void ToolBarManager::UpdateGuiFromProjectClose()
{
    deleteDendBar();
    deleteMainBar();
}

void ToolBarManager::SetSplittingOff()
{
    if(dendToolBar_){
        dendToolBar_->SetSplittingOff();
    }
}

void ToolBarManager::SetCuttingOff()
{
    if(dendToolBar_){
        dendToolBar_->SetCuttingOff();
    }
}

void ToolBarManager::SetTool(const om::tool::mode tool){
    mainToolBar_->SetTool(tool);
}

void ToolBarManager::WindowResized(QPoint oldPos)
{
    // printf("toolbarManager:: resizing to %d, %d\n", oldPos.x(), oldPos.y());
    if(dendToolBar_){
        dendToolBar_->updateToolBarsPos(oldPos);
    }
}

void ToolBarManager::WindowMoved(QPoint oldPos)
{
    // printf("toolbarManager:: moving to %d, %d\n", oldPos.x(), oldPos.y());
    if(dendToolBar_){
        dendToolBar_->updateToolBarsPos(oldPos);
    }
}
