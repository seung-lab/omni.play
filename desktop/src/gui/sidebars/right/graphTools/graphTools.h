#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "gui/widgets/omWidget.hpp"
#include "gui/tools.hpp"

class MSTThresholdSpinBox;
class AutomaticSpreadingThresholdSpinBox;
class OmViewGroupState;
class SegmentationDataWrapper;
class SplitButton;
class JoinButton;
class JoinAllButton;
class ShatterButton;
class BreakButton;

namespace om {
namespace sidebars {
class rightImpl;
}
}

class GraphTools : public OmWidget, public om::event::ToolModeEventListener {
  Q_OBJECT;

 public:
  GraphTools(om::sidebars::rightImpl*, OmViewGroupState& vgs);

  void ToolModeChangeEvent();

  OmViewGroupState& GetViewGroupState() const { return vgs_; }

  SegmentationDataWrapper GetSDW();
  void updateGui();

  void ActivateToolButton(om::tool::mode tool);
  void HideBreakThreasholdBox();
  void ShowBreakThreasholdBox();

  QString getName() { return "Graph Tools"; }

 private:
  void signalOff(om::tool::mode tool);
  QPushButton* getButton(om::tool::mode tool);

  om::sidebars::rightImpl* const mParent;

  OmViewGroupState& vgs_;

  SplitButton* splitButton;
  JoinButton* joinButton;
  JoinAllButton* joinAllButton;
  ShatterButton* shatterButton;
  BreakButton* breakButton;

  MSTThresholdSpinBox* threshold_;
  AutomaticSpreadingThresholdSpinBox* asthreshold_;

  QWidget* thresholdBox();
  QWidget* makeBreakThresholdBox();
  QWidget* breakThresholdBox_;
  std::vector<om::tool::mode> supportedTools_;
};
