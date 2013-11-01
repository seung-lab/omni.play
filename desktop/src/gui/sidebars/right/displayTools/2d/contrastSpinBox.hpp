#pragma once

#include "common/logging.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/omChannelTileFilter.hpp"
#include "viewGroup/omViewGroupState.h"

#include <limits>

class ContrastSpinBox : public OmDoubleSpinBox {
  Q_OBJECT;
  ;
 public:
  ContrastSpinBox(QWidget* d) : OmDoubleSpinBox(d, true) {
    setSingleStep(0.05);
    setRange(-5, 5);
    setInitialGUIThresholdValue();
  }

  QString Label() const { return "Contrast"; }

 private:
  void actUponValueChange(const double threshold) {
    OmChannelTileFilter::SetContrastValue(threshold);
    OmTileCache::ClearChannel();
    om::event::Redraw2dBlocking();
  }

  void setInitialGUIThresholdValue() {
    setValue(OmChannelTileFilter::GetContrastValue());
  }
};
