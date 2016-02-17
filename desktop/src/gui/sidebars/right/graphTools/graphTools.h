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
class ShatterButton;

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
  void ActivateToolButton(om::tool::mode tool);
  void SetShatteringOff();

  OmViewGroupState& GetViewGroupState() const { return vgs_; }

  SegmentationDataWrapper GetSDW();
  void updateGui();

  void HideBreakThreasholdBox();
  void ShowBreakThreasholdBox();

  QString getName() { return "Graph Tools"; }

 private
Q_SLOTS:
  void setSplittingOff();
  void setJoiningOff();
  void setShatteringOff();

Q_SIGNALS:
  void signalSplittingOff();
  void signalJoiningOff();
  void signalShatteringOff();

 private:
  void signalOff(om::tool::mode tool);

  om::sidebars::rightImpl* const mParent;

  OmViewGroupState& vgs_;

  SplitButton* splitButton;
  JoinButton* joinButton;
  ShatterButton* shatterButton;

  MSTThresholdSpinBox* threshold_;
  AutomaticSpreadingThresholdSpinBox* asthreshold_;

  QWidget* thresholdBox();
  QWidget* makeBreakThresholdBox();
  QWidget* breakThresholdBox_;
  std::vector<om::tool::mode> supportedTools_;
};
