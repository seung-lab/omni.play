#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "system/omConnect.hpp"
#include "gui/sidebars/right/graphTools/showBreakWidget.hpp"
#include "gui/widgets/viewGroupStateWidget.hpp"
#include "gui/sidebars/right/graphTools/breakThresholdGroup.h"
#include "viewGroup/omViewGroupState.h"
#include "system/omStateManager.h"
#include "events/events.h"
#include "gui/tools.hpp"

ShowBreakWidget::ShowBreakWidget(QWidget* widget, OmViewGroupState& vgs)
  : ViewGroupStateWidget(widget, vgs),
    showBreakButton_(new OmButton(this, "Break", "View broken segments", true)),
    breakSpinboxWidget_(new QGroupBox("Break Threshold", this)),
    breakThresholdSpinBox_(new BreakThresholdGroup(this, vgs)) {

  QGridLayout* breakLayout = new QGridLayout(this);

  breakLayout->addWidget(showBreakButton_, 0, 0, 1, 2);
  breakLayout->addWidget(breakSpinboxWidget_, 1, 0, 1, 2);

  QFormLayout* showBreakForm = new QFormLayout(breakSpinboxWidget_);
  showBreakForm->addWidget(breakThresholdSpinBox_);

  breakSpinboxWidget_->hide();

  om::connect(showBreakButton_, SIGNAL(toggled(bool)), this,
      SLOT(updateShouldVolumeBeShownBroken(bool)));
}

void ShowBreakWidget::updateShouldVolumeBeShownBroken(const bool shouldVolumeBeShownBroken) {
  if (shouldVolumeBeShownBroken) {
    breakSpinboxWidget_->show();
  } else {
    breakSpinboxWidget_->hide();
  }
  GetViewGroupState().SetShouldVolumeBeShownBroken(shouldVolumeBeShownBroken);
  om::event::Redraw2d();
  om::event::Redraw3d();
}

void ShowBreakWidget::ToolModeChangeEvent(om::tool::mode eventTool) {
  showBreakButton_->setChecked(eventTool == om::tool::mode::SPLIT);
}
