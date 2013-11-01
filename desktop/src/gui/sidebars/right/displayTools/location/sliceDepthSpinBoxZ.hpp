#pragma once

#include "gui/sidebars/right/displayTools/location/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxZ : public SliceDepthSpinBoxBase {
  Q_OBJECT;
  ;
 public:
  SliceDepthSpinBoxZ(QWidget* d, OmViewGroupState* vgs)
      : SliceDepthSpinBoxBase(d, vgs) {}

  virtual QString Label() const { return "Z"; }

 private:
  virtual om::common::ViewType viewType() const { return om::common::XY_VIEW; }
};
