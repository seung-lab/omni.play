#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "system/omConnect.hpp"
#include "gui/sidebars/right/graphTools/showBreakWidget.hpp"
#include "gui/widgets/viewGroupStateWidget.hpp"
#include "gui/sidebars/right/graphTools/breakThresholdGroup.h"
#include "viewGroup/omViewGroupState.h"
#include "system/omStateManager.h"

ShowBreakWidget::ShowBreakWidget(QWidget* widget, OmViewGroupState& vgs)
  : ViewGroupStateWidget(widget, vgs),
    showBreakButton_(new OmButton(this, "Break", "View broken segments", true)),
    breakSpinboxWidget_(new QGroupBox("Break Threshold", this)),
    breakThresholdSpinBox_(new BreakThresholdGroup(this, vgs)) {

  QFormLayout* showBreakForm = new QFormLayout(this);
  showBreakForm->addWidget(showBreakButton_);

  QHBoxLayout* layout = new QHBoxLayout(breakSpinboxWidget_);
  layout->addWidget(breakThresholdSpinBox_);
  showBreakForm->addWidget(breakSpinboxWidget_);

  breakSpinboxWidget_->hide();

  om::connect(showBreakButton_, SIGNAL(clicked(bool)), this,
      SLOT(updateShouldVolumeBeShownBroken()));
}

void ShowBreakWidget::updateShouldVolumeBeShownBroken(const bool shouldVolumeBeShownBroken) {
  if (shouldVolumeBeShownBroken) {
    breakSpinboxWidget_->hide();
  } else {
    breakSpinboxWidget_->show();
  }
  GetViewGroupState().SetShouldVolumeBeShownBroken(shouldVolumeBeShownBroken);
}

void ShowBreakWidget::ToolModeChangeEvent() {
  showBreakButton_->setChecked(OmStateManager::GetToolMode()==om::tool::mode::SPLIT);
}
