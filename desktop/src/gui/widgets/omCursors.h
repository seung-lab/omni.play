#pragma once
#include "precomp.h"

class OmBusyCursorWrapper {
 public:
  OmBusyCursorWrapper() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  }

  ~OmBusyCursorWrapper() { QApplication::restoreOverrideCursor(); }
};

class OmCursors {
 public:
  static void setToolCursor(QWidget*);

 private:
  static QCursor figureOutCursor();
};
