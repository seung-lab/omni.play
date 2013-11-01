#pragma once

#include "gui/widgets/omDoubleSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/omChannelTileFilter.hpp"

class GammaSpinBox : public OmDoubleSpinBox {
  Q_OBJECT;
  ;
 public:
  GammaSpinBox(QWidget* d) : OmDoubleSpinBox(d, true) {
    setSingleStep(0.05);
    setRange(-5, 5);
    setInitialGUIThresholdValue();
  }

  QString Label() const { return "Gamma"; }

 private:
  void actUponValueChange(const double threshold) {
    OmChannelTileFilter::SetGamma(threshold);
    OmTileCache::ClearChannel();
    om::event::Redraw2dBlocking();
  }

  void setInitialGUIThresholdValue() {
    setValue(OmChannelTileFilter::GetGamma());
  }
};
