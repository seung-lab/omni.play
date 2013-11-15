#pragma once

#include "common/common.h"
#include "zi/utility.h"

#include <QSize>
#include <QUndoStack>
#include <QGLWidget>
#include <QGLContext>

class MainWindow;
class InspectorWidget;
class ToolBarManager;
class TaskSelector;

namespace om {
namespace sidebars {
class rightImpl;
}
}

class OmAppState : private om::SingletonBase<OmAppState> {
 public:
  static QString GetPID();
  static QString GetHostname();

  static void SetInspector(InspectorWidget* miw);
  static void SetTaskSelector(TaskSelector* ts);
  static void SetMainWindow(MainWindow* mw);
  static MainWindow* GetMainWindow();

  static void SetRightToolBar(om::sidebars::rightImpl* rightToolbar) {
    instance().rightToolbar_ = rightToolbar;
  }

  static ToolBarManager* GetToolBarManager() {
    return instance().toolBarManager_;
  }
  static void SetToolBarManager(ToolBarManager* tbm);

  static void UpdateStatusBar(const QString& msg);

  static QSize GetViewBoxSizeHint();

  static bool OpenProject(std::string fileNameAndPath);
  static void OpenTaskSelector();

 private:
  MainWindow* mainWindow_;
  InspectorWidget* inspectorWidget_;
  TaskSelector* taskSelector_;
  om::sidebars::rightImpl* rightToolbar_;
  ToolBarManager* toolBarManager_;

  OmAppState()
      : mainWindow_(nullptr),
        inspectorWidget_(nullptr),
        rightToolbar_(nullptr),
        toolBarManager_(nullptr) {}

  ~OmAppState() {};

  friend class zi::singleton<OmAppState>;
};
