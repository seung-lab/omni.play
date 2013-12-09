#pragma once

#include "system/omStateManager.h"
#include "view3d/camera.h"
#include "view3d/view3d.h"
#include "view3d/widgets/base.h"

namespace om {
namespace v3d {

class InfoWidget : public Widget {
 public:
  explicit InfoWidget(View3d &view3d) : Widget(view3d) {
    font_.setWeight(QFont::Bold);
    font_.setItalic(true);
  }

  virtual void Draw() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // need to disable lighting for proper text color
    glDisable(GL_LIGHTING);

    glColor4fv(TEXT_COLOR);  // set text color

    const int height = view3d_.height();
    renderCameraText(10, height - 30);

    glPopAttrib();
  }

 private:
  void renderCameraText(int x, int y) {
    const Vector3f focus = view3d_.GetCamera().GetFocus();
    QString str =
        QString("Focus: ( %1, %2, %3 )").arg(focus.x).arg(focus.y).arg(focus.z);
    view3d_.renderText(x, y, str, font_);

    const float distance = view3d_.GetCamera().GetDistance();
    str = QString("Distance: %1").arg(distance);
    view3d_.renderText(x, y + 15, str, font_);
  }
};
}
}  // om::v3d::
