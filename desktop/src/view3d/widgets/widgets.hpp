#pragma once
#include "precomp.h"

#include "system/preferences3d.hpp"
#include "system/omPreferences.h"
#include "view3d/widgets/annotationsWidget.h"
#include "view3d/widgets/base.h"
#include "view3d/widgets/chunkExtentWidget.hpp"
#include "view3d/widgets/infoWidget.hpp"
#include "view3d/widgets/percDoneWidget.hpp"
#include "view3d/widgets/viewBoxWidget.h"

namespace om {
namespace v3d {

enum widgets {
  viewbox = 0,
  info,
  chunk_extent,
  perc_done,
  annotations,
  TOTAL_NUM
};

class Widgets {
 private:
  std::array<Widget*, widgets::TOTAL_NUM> widgets_;

 public:
  Widgets(View3d& view3d, OmViewGroupState& vgs)
      : widgets_(
            {{new ViewBoxWidget(view3d, vgs),    new InfoWidget(view3d),
              new ChunkExtentWidget(view3d),     new PercDoneWidget(view3d),
              new AnnotationsWidget(view3d, vgs)}}) {}

  ~Widgets() {
    for (auto* w : widgets_) {
      delete w;
    }
  }

  inline void Draw() {
    for (auto* w : widgets_) {
      if (w->enabled()) {
        w->Draw();
      }
    }
  }

  void UpdateEnabledWidgets() {
    const auto& prefs = OmPreferences::V3dPrefs();

    // set widgets enabled
    widgets_[viewbox]->enable(prefs.SHOW_VIEWBOX);
    widgets_[info]->enable(prefs.SHOW_INFO);
    widgets_[chunk_extent]->enable(false);  // prefs.SHOW_CHUNK_EXTENT); Breaks
                                            // view3d.
    widgets_[perc_done]->enable(true);
    widgets_[annotations]->enable(true);
  }
};
}
}  // om::v3d::
