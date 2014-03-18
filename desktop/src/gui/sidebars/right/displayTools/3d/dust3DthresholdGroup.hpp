#pragma once
#include "precomp.h"

#include "gui/widgets/omIntSpinBox.hpp"
#include "events/events.h"
#include "viewGroup/omViewGroupState.h"

class Dust3DThresholdGroup : public OmIntSpinBox {
  Q_OBJECT;

 public:
  Dust3DThresholdGroup(QWidget* parent, OmViewGroupState* vgs)
      : OmIntSpinBox(parent, true), vgs_(vgs) {
    setSingleStep(5);
    setMaximum(std::numeric_limits<int32_t>::max());
    setInitialGUIThresholdValue();
  }

 private:
  OmViewGroupState* const vgs_;

  void actUponValueChange(const int threshold) {
    vgs_->setDustThreshold(threshold);
    om::event::Redraw3d();
  }

  void setInitialGUIThresholdValue() {
    int dThreshold = vgs_->getDustThreshold();
    setValue(dThreshold);
  }
};
