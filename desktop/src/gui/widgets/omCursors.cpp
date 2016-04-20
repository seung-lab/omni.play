#include "gui/widgets/omCursors.h"
#include "common/common.h"
#include "gui/tools.hpp"

void OmCursors::setToolCursor(QWidget* w, om::tool::mode tool) {
  w->setCursor(figureOutCursor(tool));
}

QCursor OmCursors::figureOutCursor(om::tool::mode tool) {
  switch (tool) {
    case om::tool::SELECT:
      return Qt::BlankCursor;
    case om::tool::PAN:
    default:
      return Qt::ArrowCursor;
  }
}
