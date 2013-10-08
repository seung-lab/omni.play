#include "omSelectionWidget.h"
#include "view3d/omCamera.h"
#include "view3d/omView3d.h"
#include "mesh/omVolumeCuller.h"
#include "common/omGl.h"

static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };

OmSelectionWidget::OmSelectionWidget(OmView3d* view3d)
    : OmView3dWidget(view3d) {}
;

/////////////////////////////////
///////          Draw Methods

/*
 * 2-Pass Polygon Outlining using the Stencil Buffer
 * http://www.codeproject.com/KB/openGL/Outline_Mode.aspx
 */
void OmSelectionWidget::Draw() {
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  //clear and setup stencil
  //glClearStencil(0);
  //glClear( GL_STENCIL_BUFFER_BIT );
  glEnable(GL_STENCIL_TEST);

  //disable lighting (faster draw)
  glDisable(GL_LIGHTING);
  //selected visiable over everything
  glDisable(GL_DEPTH_TEST);

  //stencil buff only allows writing to screen when buff not 1
  glStencilFunc(GL_NOTEQUAL, 1, 0xFFFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  //set to line mode
  glPolygonMode(GL_FRONT, GL_LINE);
  glLineWidth(5.0f);

  //enable writing to color buffer
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  //glDepthMask(GL_TRUE);
  View3dPtr()->DrawVolumes(DRAWOP_LEVEL_ALL | DRAWOP_SEGMENT_FILTER_SELECTED |
                           DRAWOP_SEGMENT_COLOR_HIGHLIGHT);

  glPopAttrib();
}
