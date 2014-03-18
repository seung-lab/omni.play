#pragma once
#include "precomp.h"

#include "base.h"

class OmViewGroupState;

namespace om {
namespace v3d {

class AnnotationsWidget : public Widget {
 private:
  constexpr static const float DIST_CUTOFF = 100.0f;
  OmViewGroupState& vgs_;

 public:
  AnnotationsWidget(View3d& view3d, OmViewGroupState& vgs)
      : Widget(view3d), vgs_(vgs) {}

  virtual void Draw();
};
}
}  // om::v3d::
