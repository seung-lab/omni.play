#pragma once
#include "precomp.h"

class OmViewGroupState;
class SegmentationDataWrapper;

namespace om {
namespace sidebars {
class rightImpl;
}
}

class DisplayTools : public QWidget {
  Q_OBJECT;

 public:
  DisplayTools(om::sidebars::rightImpl*, OmViewGroupState* vgs);

  OmViewGroupState* GetViewGroupState() { return vgs_; }

  void updateGui();

  QString getName() { return "Display Tools"; }

 private:
  OmViewGroupState* const vgs_;

  QWidget* view2dSliceDepthBox();
  QWidget* thresholdBox();
};
