#pragma once
#include "precomp.h"

#include "actions/omActions.h"
#include "common/logging.h"
#include "events/events.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"

class BreakThresholdGroup : public OmDoubleSpinBox {
  Q_OBJECT;

 public:
  BreakThresholdGroup(GraphTools* d, OmViewGroupState& vgs)
      : OmDoubleSpinBox(d, true), vgs_(vgs) {
    setSingleStep(0.002);
    setMaximum(1.0);
    setDecimals(3);
    setInitialGUIThresholdValue();
  }

 private:
  OmViewGroupState& vgs_;

  void setInitialGUIThresholdValue() {
    static const double threshold = 1.;

    setGUIvalue(threshold);

    vgs_.setBreakThreshold(threshold);
  }

  void actUponValueChange(const double threshold) {

    vgs_.setBreakThreshold(threshold);

    // todo: don't use same freshness as normal tiles
    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
    om::event::Redraw3d();
  }
};
