#pragma once
#include "precomp.h"

#include "gui/widgets/omWidget.hpp"

class MSTThresholdSpinBox;
class SizeThresholdSpinBox;
class AutomaticSpreadingThresholdSpinBox;
class OmViewGroupState;
class SegmentationDataWrapper;
class SplitButton;
class ShatterButton;

namespace om {
namespace sidebars {
class rightImpl;
}
}

class GraphTools : public OmWidget {
  Q_OBJECT;

 public:
  GraphTools(om::sidebars::rightImpl*, OmViewGroupState& vgs);

  void SetSplittingOff();
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
  void setShatteringOff();

Q_SIGNALS:
  void signalSplittingOff();
  void signalShatteringOff();

 private:
  om::sidebars::rightImpl* const mParent;

  OmViewGroupState& vgs_;

  SplitButton* splitButton;
  ShatterButton* shatterButton;

  MSTThresholdSpinBox* threshold_;
  SizeThresholdSpinBox* sizeThreshold_;
  AutomaticSpreadingThresholdSpinBox* asthreshold_;

  QWidget* thresholdBox();
  QWidget* makeBreakThresholdBox();
  QWidget* breakThresholdBox_;
};
