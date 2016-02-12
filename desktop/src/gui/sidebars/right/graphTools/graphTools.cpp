#include "events/events.h"
#include "gui/sidebars/right/graphTools/breakButton.h"
#include "gui/sidebars/right/graphTools/breakThresholdGroup.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/graphTools/mstThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/sizeThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/automaticSpreadingThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinButton.h"
#include "gui/sidebars/right/graphTools/splitButton.h"
#include "gui/sidebars/right/graphTools/shatterButton.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

GraphTools::GraphTools(om::sidebars::rightImpl* d, OmViewGroupState& vgs)
    : OmWidget(d),
      mParent(d),
      vgs_(vgs),
      joinButton(new JoinButton(this)),
      splitButton(new SplitButton(this)),
      shatterButton(new ShatterButton(this)) {
  QFormLayout* box = new QFormLayout(this);
  box->addWidget(thresholdBox());

  QWidget* wbox = new QWidget(this);
  QGridLayout* box2 = new QGridLayout(wbox);
  wbox->setLayout(box2);
  box->addWidget(wbox);

  box2->addWidget(joinButton, 0, 0, 1, 1);
  box2->addWidget(splitButton, 0, 1, 1, 1);
  box2->addWidget(shatterButton, 1, 0, 1, 1);
  box2->addWidget(new BreakButton(this), 1, 1, 1, 1);

  breakThresholdBox_ = makeBreakThresholdBox();
  breakThresholdBox_->hide();
  box->addWidget(breakThresholdBox_);

  om::connect(this, SIGNAL(signalJoiningOff()), this,
              SLOT(setJoiningOff()));
  om::connect(this, SIGNAL(signalSplittingOff()), this,
              SLOT(setSplittingOff()));
  om::connect(this, SIGNAL(signalShatteringOff()), this,
              SLOT(setShatteringOff()));
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
  QGroupBox* widget = new QGroupBox("Join Thresholds", this);

  QLabel *mstThresholdLabel = new QLabel(tr("Min Affinity"));
  threshold_ = new MSTThresholdSpinBox(this);
  QLabel *sizeThresholdLabel = new QLabel(tr("Max Size (Disabled!)"));
  sizeThreshold_ = new SizeThresholdSpinBox(this);
  QLabel *automaticSpreadThresholdLabel = new QLabel(tr("Min Affinity Auto Spread"));
  asthreshold_ = new AutomaticSpreadingThresholdSpinBox(this);

  QVBoxLayout* layout = new QVBoxLayout(widget);
  layout->addWidget(mstThresholdLabel);
  layout->addWidget(threshold_);
  layout->addWidget(sizeThresholdLabel);
  layout->addWidget(sizeThreshold_);
  layout->addWidget(automaticSpreadThresholdLabel);
  layout->addWidget(asthreshold_);

  return widget;
}

void GraphTools::updateGui() { om::event::Redraw2d(); }

SegmentationDataWrapper GraphTools::GetSDW() { return mParent->GetSDW(); }

void GraphTools::SetJoiningSplittingOff(om::common::JoinOrSplit joinOrSplit) {
  if (joinOrSplit == om::common::JoinOrSplit::JOIN) {
    signalJoiningOff();
  } else {
    signalSplittingOff();
  }
}

void GraphTools::setJoiningOff() { joinButton->setChecked(false); }

void GraphTools::setSplittingOff() { splitButton->setChecked(false); }

void GraphTools::SetShatteringOff() { signalShatteringOff(); }

void GraphTools::setShatteringOff() { shatterButton->setChecked(false); }

void GraphTools::HideBreakThreasholdBox() { breakThresholdBox_->hide(); }

void GraphTools::ShowBreakThreasholdBox() { breakThresholdBox_->show(); }
