#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/left/inspectorWidget.h"
#include "gui/sidebars/right/rightImpl.h"
#include "system/omAppState.hpp"
#include "zi/omUtility.h"

#include <QApplication>

QString OmAppState::GetPID() { return QString::number(qApp->applicationPid()); }

QString OmAppState::GetHostname() {
  return QString::fromStdString(zi::system::hostname);
}

void OmAppState::SetInspector(InspectorWidget* miw) {
  instance().inspectorWidget_ = miw;
}

void OmAppState::SetMainWindow(MainWindow* mw) { instance().mainWindow_ = mw; }

void OmAppState::UpdateStatusBar(const QString& msg) {
  instance().mainWindow_->updateStatusBar(msg);
}

QSize OmAppState::GetViewBoxSizeHint() {
  QWidget* mw = instance().mainWindow_;
  if (!mw) {
    mw = QApplication::activeWindow();
    if (!mw) {
      printf("warning: assuming window size is 1000x640\n");
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
