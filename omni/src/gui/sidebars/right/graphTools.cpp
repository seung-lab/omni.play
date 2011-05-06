#include "events/omEvents.h"
#include "gui/sidebars/right/breakButton.h"
#include "gui/sidebars/right/breakThresholdGroup.h"
#include "gui/sidebars/right/cutButton.h"
#include "gui/sidebars/right/dendToolbar.h"
#include "gui/sidebars/right/graphTools.h"
#include "gui/sidebars/right/joinButton.h"
#include "gui/sidebars/right/splitButton.h"
#include "gui/sidebars/right/thresholdGroup.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

GraphTools::GraphTools(DendToolBar* d, OmViewGroupState* vgs)
    : OmWidget(d)
    , mParent(d)
    , vgs_(vgs)
    , splitButton(new SplitButton(this))
    , cutButton(new CutButton(this))
{
    QVBoxLayout* box = new QVBoxLayout(this);
    box->addWidget(thresholdBox());

    QWidget * wbox = new QWidget(this);
    QGridLayout * box2 = new QGridLayout(wbox);
    wbox->setLayout(box2);
    box->addWidget(wbox);

    box2->addWidget(new JoinButton(this), 0, 0, 1, 1);
    box2->addWidget(splitButton, 0, 1, 1, 1);
    box2->addWidget(cutButton, 1, 0, 1, 1);
    box2->addWidget(new BreakButton(this), 1, 1, 1, 1);

    breakThresholdBox_ = makeBreakThresholdBox();
    breakThresholdBox_->hide();
    box->addWidget(breakThresholdBox_);

    om::connect(this, SIGNAL(signalSplittingOff()),
                this, SLOT(setSplittingOff()));

    om::connect(this, SIGNAL(signalSetCutButtonOff()),
                this, SLOT(setCutButtonOff()));
}

QWidget* GraphTools::makeBreakThresholdBox()
{
    QGroupBox* widget = new QGroupBox("Break Threshold", this);
    BreakThresholdGroup* breakThresholdGroup = new BreakThresholdGroup(this, vgs_);

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(breakThresholdGroup);

    return widget;
}

QWidget* GraphTools::thresholdBox()
{
    QGroupBox* widget = new QGroupBox("Overall Threshold", this);
    threshold_ = new ThresholdGroup(this);

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(threshold_);

    return widget;
}

void GraphTools::SetCuttingOff(){
    signalSetCutButtonOff();
}

void GraphTools::setCutButtonOff()
{
    printf("GraphTools::SetCuttingOff\n");
    cutButton->setChecked(false);
}

void GraphTools::updateGui(){
    OmEvents::Redraw2d();
}

SegmentationDataWrapper GraphTools::GetSDW(){
    return mParent->GetSDW();
}

void GraphTools::SetSplittingOff(){
    signalSplittingOff();
}

void GraphTools::setSplittingOff(){
    splitButton->setChecked(false);
}

void GraphTools::RefreshThreshold(){
    threshold_->RefreshThreshold();
}

void GraphTools::HideBreakThreasholdBox(){
    breakThresholdBox_->hide();
}

void GraphTools::ShowBreakThreasholdBox(){
    breakThresholdBox_->show();
}
