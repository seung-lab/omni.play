#pragma once
#include "precomp.h"

#include "gui/widgets/viewGroupStateWidget.hpp"
#include "events/listeners.h"

class OmButton;
class OmViewGroupState;
class BreakThresholdGroup;

class ShowBreakWidget : public ViewGroupStateWidget,
  public om::event::ToolModeEventListener {
Q_OBJECT

 public:
  ShowBreakWidget(QWidget* widget, OmViewGroupState& vgs);

  QString getName() { return "ShowBreakWidget"; }

  void ToolModeChangeEvent();

 private:
  OmButton* showBreakButton_;
  QGroupBox* breakSpinboxWidget_;
  BreakThresholdGroup* breakThresholdSpinBox_;

  void showHideSpinBox(bool shouldShowSpinBox);

 private
Q_SLOTS:
  void updateShouldVolumeBeShownBroken(const bool shouldVolumeBeShownBroken);
};
