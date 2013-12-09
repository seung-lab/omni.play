#include "events/events.h"
#include "gui/sidebars/right/graphTools/breakButton.h"
#include "gui/sidebars/right/graphTools/breakThresholdGroup.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/graphTools/mstThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/sizeThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinButton.h"
#include "gui/sidebars/right/graphTools/splitButton.h"
#include "gui/sidebars/right/graphTools/shatterButton.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

GraphTools::GraphTools(om::sidebars::rightImpl* d, OmViewGroupState* vgs)
    : OmWidget(d),
      mParent(d),
      vgs_(vgs),
      splitButton(new SplitButton(this)),
      shatterButton(new ShatterButton(this)) {
  QFormLayout* box = new QFormLayout(this);
  box->addWidget(thresholdBox());

  QWidget* wbox = new QWidget(this);
  QGridLayout* box2 = new QGridLayout(wbox);
  wbox->setLayout(box2);
  box->addWidget(wbox);

  box2->addWidget(new JoinButton(this), 0, 0, 1, 1);
  box2->addWidget(splitButton, 0, 1, 1, 1);
  box2->addWidget(shatterButton, 1, 0, 1, 1);
  box2->addWidget(new BreakButton(this), 1, 1, 1, 1);

  breakThresholdBox_ = makeBreakThresholdBox();
  breakThresholdBox_->hide();
  box->addWidget(breakThresholdBox_);

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
  QGroupBox* widget = new QGroupBox("Overall Threshold", this);
  threshold_ = new MSTThresholdSpinBox(this);
  sizeThreshold_ = new SizeThresholdSpinBox(this);

  QVBoxLayout* layout = new QVBoxLayout(widget);
  layout->addWidget(threshold_);
  layout->addWidget(sizeThreshold_);

  return widget;
}

void GraphTools::updateGui() { om::event::Redraw2d(); }

SegmentationDataWrapper GraphTools::GetSDW() { return mParent->GetSDW(); }

void GraphTools::SetSplittingOff() { signalSplittingOff(); }

void GraphTools::setSplittingOff() { splitButton->setChecked(false); }

void GraphTools::SetShatteringOff() { signalShatteringOff(); }

void GraphTools::setShatteringOff() { shatterButton->setChecked(false); }

void GraphTools::HideBreakThreasholdBox() { breakThresholdBox_->hide(); }

void GraphTools::ShowBreakThreasholdBox() { breakThresholdBox_->show(); }
