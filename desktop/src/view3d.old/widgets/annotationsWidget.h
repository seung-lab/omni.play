#pragma once
#include "precomp.h"

#include "../omView3dWidget.h"
#include "common/common.h"
#include "volume/omSegmentation.h"

struct annotation {
  QString text;
  om::coords::Global point;
  om::common::Color color;
};

class AnnotationsWidget : public OmView3dWidget {

 public:
  AnnotationsWidget(OmView3d *view3d, OmViewGroupState &vgs);
  virtual void Draw();

 private:
  QFont font_;
  constexpr static const float DIST_CUTOFF = 100.0f;
  OmViewGroupState &vgs_;
};
