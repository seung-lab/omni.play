#include "annotation/annotation.h"
#include "common/common.h"
#include "view3d/camera.h"
#include "view3d/view3d.h"
#include "view3d/widgets/annotationsWidget.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "utility/glInclude.h"

using namespace om::v3d;

void AnnotationsWidget::Draw() {
  if (!vgs_.getAnnotationVisible()) {
    return;
  }

  Vector3f camPos = view3d_.GetCamera().GetPosition();

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  GLUquadricObj* quad = gluNewQuadric();

  for (OmSegmentation* vol : view3d_.Segmentations()) {
    FOR_EACH(it, vol->Annotations().Enabled()) {
      om::annotation::data& data = *it->Object;
      const om::coords::Global coord = data.coord.ToGlobal();
      float dist = camPos.distance(coord);

      glPushMatrix();
      glEnable(GL_LIGHTING | GL_DEPTH_TEST);
      glColor3ub(data.color.red, data.color.green, data.color.blue);
      glTranslatef(coord.x, coord.y, coord.z);
      gluSphere(quad, data.size * .005 * dist, 26, 13);

      glDisable(GL_LIGHTING | GL_DEPTH_TEST);
      glColor4fv(TEXT_COLOR);
      view3d_.renderText(.4, -.2, 0, QString::fromStdString(data.comment),
                         font_);
      glPopMatrix();
    }
  }

  gluDeleteQuadric(quad);

  glPopAttrib();
}
