#pragma once

#include "base.h"
#include "system/omStateManager.h"
#include "view3d/camera.h"
#include "view3d/view3d.h"
#include "view3d/drawStatus.hpp"

#include <QFont>

namespace om {
namespace v3d {

class PercDoneWidget : public Widget {
 public:
  explicit PercDoneWidget(View3d& view3d) : Widget(view3d) {
    font_.setWeight(QFont::Bold);
    font_.setItalic(true);
  }

  virtual void Draw() {
    auto status = view3d_.Status();
    drawText(QString::fromStdString(status.Msg()));
  }

 private:
  void drawText(const QString& str) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // need to disable lighting for proper text color
    glDisable(GL_LIGHTING);

    glColor4fv(TEXT_COLOR);  // set text color

    static const int widthPerLetter = 11;
    const int width = widthPerLetter * str.size();

    const int x = view3d_.width() - width;
    const int y = view3d_.height() - 15;

    view3d_.renderText(x, y, qPrintable(str), font_);

    glPopAttrib();
  }
};
}
}  // om::v3d::
