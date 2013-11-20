#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/left/inspectorWidget.h"
#include "gui/sidebars/right/rightImpl.h"
#include "system/omAppState.hpp"
#include "zi/utility.h"
#include "project/omProject.h"
#include "gui/taskSelector/taskSelector.h"

#include <QApplication>

QString OmAppState::GetPID() { return QString::number(qApp->applicationPid()); }

QString OmAppState::GetHostname() {
  return QString::fromStdString(zi::system::hostname);
}

void OmAppState::SetInspector(InspectorWidget* miw) {
  instance().inspectorWidget_ = miw;
}

void OmAppState::SetTaskSelector(TaskSelector* ts) {
  instance().taskSelector_ = ts;
}

void OmAppState::SetMainWindow(MainWindow* mw) { instance().mainWindow_ = mw; }
MainWindow* OmAppState::GetMainWindow() { return instance().mainWindow_; }

void OmAppState::UpdateStatusBar(const QString& msg) {
  instance().mainWindow_->updateStatusBar(msg);
}

QSize OmAppState::GetViewBoxSizeHint() {
  QWidget* mw = instance().mainWindow_;
  if (!mw) {
    mw = QApplication::activeWindow();
    if (!mw) {
      log_debugs(unknown) << "warning: assuming window size is 1000x640";
      return QSize(1000, 640);
    }
  }

  int w = mw->width();
  int h = mw->height();

  if (instance().inspectorWidget_) {
    w -= instance().inspectorWidget_->width();
  }

  if (instance().rightToolbar_) {
    w -= instance().rightToolbar_->width();
  }

  return QSize(w, h);
}

void OmAppState::SetToolBarManager(ToolBarManager* tbm) {
  instance().toolBarManager_ = tbm;
}

bool OmAppState::OpenProject(const std::string& fileNameAndPath,
                             const std::string& userName) {
  if (instance().mainWindow_) {
    return instance().mainWindow_->openProject(fileNameAndPath, userName);
  } else {
    try {
      OmProject::SafeLoad(fileNameAndPath, nullptr, userName);
      return true;
    }
    catch (om::Exception& e) {
      log_debugs(task) << "Exception thrown: " << e.what();
      return false;
    }
  }
}

void OmAppState::OpenTaskSelector() {
  if (instance().taskSelector_) {
    instance().taskSelector_->show();
  }
}