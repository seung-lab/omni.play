#include "events/events.h"
#include "gui/sidebars/right/graphTools/breakButton.h"
#include "gui/sidebars/right/graphTools/joinAllButton.h"
#include "gui/sidebars/right/graphTools/breakThresholdGroup.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/graphTools/mstThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/automaticSpreadingThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinButton.h"
#include "gui/sidebars/right/graphTools/splitButton.h"
#include "gui/sidebars/right/graphTools/shatterButton.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"
#include "system/omStateManager.h"

GraphTools::GraphTools(om::sidebars::rightImpl* d, OmViewGroupState& vgs)
    : OmWidget(d),
      mParent(d),
      vgs_(vgs),
      joinButton(new JoinButton(this)),
      splitButton(new SplitButton(this)),
      breakButton(new BreakButton(this)),
      shatterButton(new ShatterButton(this)) {
  QFormLayout* box = new QFormLayout(this);
  box->addWidget(thresholdBox());

  QWidget* wbox = new QWidget(this);
  QGridLayout* box2 = new QGridLayout(wbox);
  wbox->setLayout(box2);
  box->addWidget(wbox);

  box2->addWidget(joinButton, 0, 0, 1, 1);
  box2->addWidget(new JoinAllButton(this), 0, 1, 1, 1);
  box2->addWidget(splitButton, 1, 0, 1, 1);
  box2->addWidget(shatterButton, 1, 1, 1, 1);
  box2->addWidget(breakButton, 2, 0, 1, 2);

  breakThresholdBox_ = makeBreakThresholdBox();
  breakThresholdBox_->hide();
  box->addWidget(breakThresholdBox_);

  supportedTools_.push_back(om::tool::mode::JOIN);
  supportedTools_.push_back(om::tool::mode::SPLIT);
  supportedTools_.push_back(om::tool::mode::SHATTER);
}

QWidget* GraphTools::makeBreakThresholdBox() {
  QGroupBox* widget = new QGroupBox("Break Threshold", this);
  BreakThresholdGroup* breakThresholdGroup =
      new BreakThresholdGroup(this, vgs_);

  QHBoxLayout* layout = new QHBoxLayout(widget);
  layout->addWidget(breakThresholdGroup);

  return widget;
}

QWidget* GraphTools::thresholdBox() {
  QGroupBox* widget = new QGroupBox("Minimum Affinity to Join", this);

  QLabel *mstThresholdLabel = new QLabel(tr("Global Threshold"));
  threshold_ = new MSTThresholdSpinBox(this);
  QLabel *automaticSpreadThresholdLabel = new QLabel(tr("Auto Spread"));
  asthreshold_ = new AutomaticSpreadingThresholdSpinBox(this);

  QVBoxLayout* layout = new QVBoxLayout(widget);
  layout->addWidget(mstThresholdLabel);
  layout->addWidget(threshold_);
  layout->addWidget(automaticSpreadThresholdLabel);
  layout->addWidget(asthreshold_);

  return widget;
}

SegmentationDataWrapper GraphTools::GetSDW() { return mParent->GetSDW(); }

void GraphTools::ToolModeChangeEvent() {
  ActivateToolButton(OmStateManager::GetToolMode());
}

void GraphTools::ActivateToolButton(om::tool::mode tool) {
  for (auto supportedTool : supportedTools_) {
    QPushButton* button = getButton(supportedTool);
    // by definition all supported tools have buttons
    button->setChecked(tool == supportedTool);
  }

  // if we use split, we automatically set the break button to true!
  breakButton->setChecked(tool==om::tool::mode::SPLIT);
}

QPushButton* GraphTools::getButton(om::tool::mode tool) {
  switch (tool) {
    case om::tool::mode::JOIN:
      return joinButton;
    case om::tool::mode::SPLIT:
      return splitButton;
    case om::tool::mode::SHATTER:
      return shatterButton;
    default:
      return nullptr;
  }
}

void GraphTools::HideBreakThreasholdBox() { breakThresholdBox_->hide(); }

void GraphTools::ShowBreakThreasholdBox() { breakThresholdBox_->show(); }
