#include "omChunkExtentWidget.h"
#include "view3d/omCamera.h"
#include "view3d/omView3d.h"
#include "mesh/omVolumeCuller.h"
#include "utility/glInclude.h"

static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };

OmChunkExtentWidget::OmChunkExtentWidget(OmView3d* view3d)
    : OmView3dWidget(view3d) {}

/*
 * 2-Pass Polygon Outlining using the Stencil Buffer
 * http://www.codeproject.com/KB/openGL/Outline_Mode.aspx
 */
void OmChunkExtentWidget::Draw() {
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  View3dPtr()->DrawVolumes(DRAWOP_LEVEL_VOLUME | DRAWOP_LEVEL_CHUNKS |
                           DRAWOP_DRAW_CHUNK_EXTENT);

  glPopAttrib();
}
