#pragma once

#include "tiles/omTileTypes.hpp"
#include "view3d/camera.h"
#include "base.h"

class OmViewGroupState;
class OmViewGroupView2dState;

namespace om {
namespace v3d {

class ViewBoxWidget : public Widget {
 public:
  ViewBoxWidget(View3d& view3d, OmViewGroupState& vgs);
  virtual void Draw();

 private:
  OmViewGroupState& vgs_;

  void drawRectangle(const Vector3i& v0, const Vector3i& v1, const Vector3i& v2,
                     const Vector3i& v3);

  void draw2dBox(const om::common::ViewType plane, const Vector2f& min,
                 const Vector2f& max, const float depth);

  void drawLines(Vector3i depth);

  void draw2dBoxWrapper(OmViewGroupView2dState& view2dState,
                        const om::common::ViewType viewType);
};
}
}  // om::v3d::
