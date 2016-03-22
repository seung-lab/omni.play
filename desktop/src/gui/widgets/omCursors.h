#pragma once
#include "precomp.h"
#include "gui/tools.hpp"

class OmBusyCursorWrapper {
 public:
  OmBusyCursorWrapper() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  }

  ~OmBusyCursorWrapper() { QApplication::restoreOverrideCursor(); }
};

class OmCursors {
 public:
  static void setToolCursor(QWidget*, om::tool::mode);

 private:
  static QCursor figureOutCursor(om::tool::mode);
};
