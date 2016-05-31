#pragma once
#include "precomp.h"

#include "gui/widgets/omWidget.hpp"
#include "gui/tools.hpp"
#include "gui/widgets/toolButton.hpp"

class MSTThresholdSpinBox;
class GrowThresholdSpinBox;
class OmViewGroupState;
class SegmentationDataWrapper;
class JoinAllButton;
class ShatterButton;
class ShowBreakWidget;

namespace om {
namespace sidebars {
class rightImpl;
}
}

class GraphTools : public OmWidget {
  Q_OBJECT

 public:
  GraphTools(om::sidebars::rightImpl*, OmViewGroupState& vgs);

  OmViewGroupState& GetViewGroupState() const { return vgs_; }

  SegmentationDataWrapper GetSDW();

  QString getName() { return "Graph Tools"; }

 private:
  void signalOff(om::tool::mode tool);
  QPushButton* getButton(om::tool::mode tool);

  om::sidebars::rightImpl* const mParent;

  OmViewGroupState& vgs_;

  ToolButton* joinButton;
  ToolButton* splitButton;
  ToolButton* multiSplitButton;
  ToolButton* shatterButton;
  JoinAllButton* joinAllButton;
  ShowBreakWidget* showBreakWidget;

  MSTThresholdSpinBox* threshold_;
  GrowThresholdSpinBox* growThreshold_;

  QWidget* thresholdBox();
  std::vector<om::tool::mode> supportedTools_;
};
