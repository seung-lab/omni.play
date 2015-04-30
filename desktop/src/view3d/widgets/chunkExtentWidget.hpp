#pragma once
#include "precomp.h"

#include "view3d/camera.h"
#include "view3d/view3d.h"
#include "view3d/mesh/omVolumeCuller.h"
#include "base.h"

namespace om {
namespace v3d {

class ChunkExtentWidget : public Widget {
 public:
  explicit ChunkExtentWidget(View3d &view3d) : Widget(view3d) {}

  /*
   * 2-Pass Polygon Outlining using the Stencil Buffer
   * http://www.codeproject.com/KB/openGL/Outline_Mode.aspx
   */
  virtual void Draw() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    view3d_.DrawChunkBoundaries();
    glPopAttrib();
  }
};
}
}  // om::v3d::
