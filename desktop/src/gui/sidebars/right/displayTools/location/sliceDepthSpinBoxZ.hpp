#pragma once

#include "gui/sidebars/right/displayTools/location/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxZ : public SliceDepthSpinBoxBase {
  Q_OBJECT public : SliceDepthSpinBoxZ(QWidget* d, OmViewGroupState* vgs)
                    : SliceDepthSpinBoxBase(d, vgs) {}

  virtual QString Label() const { return "Z"; }

 private:
  virtual ViewType viewType() const { return XY_VIEW; }
};
