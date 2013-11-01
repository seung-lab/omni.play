#pragma once

#include "common/logging.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/omChannelTileFilter.hpp"

class BrightnessSpinBox : public OmIntSpinBox {
  Q_OBJECT;
  ;
 public:
  BrightnessSpinBox(QWidget* d) : OmIntSpinBox(d, true) {
    setSingleStep(1);
    setRange(-255, 255);
    setInitialGUIThresholdValue();
  }

  QString Label() const { return "Brightness"; }

 private:

  void actUponValueChange(const int val) {
    OmChannelTileFilter::SetBrightnessShift(val);
    OmTileCache::ClearChannel();
    om::event::Redraw2dBlocking();
  }

  void setInitialGUIThresholdValue() {
    setValue(OmChannelTileFilter::GetBrightnessShift());
  }
};
