#pragma once
#include "precomp.h"

#include "gui/widgets/omCheckBoxWidget.hpp"
#include "system/cache/omCacheManager.h"
#include "viewGroup/omViewGroupState.h"

namespace om {
namespace displayTools {

class BrightenSelected : public OmCheckBoxWidget {
 private:
  OmViewGroupState& vgs_;

 public:
  BrightenSelected(QWidget* parent, OmViewGroupState& vgs)
      : OmCheckBoxWidget(parent, "Brighten Selected Segments"), vgs_(vgs) {
    setChecked(true);
  }

 private:
  virtual void doAction(const bool isChecked) {

    vgs_.BrightenSelected(isChecked);
    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
  }
};

}  // namespace displayTools
}  // namespace om
