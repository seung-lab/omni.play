#include "events/omEvents.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "gui/toolbars/dendToolbar/cutButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

GraphTools::GraphTools(DendToolBar * d)
    : OmWidget(d)
    , mParent(d)
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

    box->addWidget(breakThresholdBox());

    om::connect(this, SIGNAL(signalSplitterBusy(bool)),
                this, SLOT(showSplitterBusy(bool)));

    om::connect(this, SIGNAL(signalSplittingOff()),
                this, SLOT(setSplittingOff()));

    om::connect(this, SIGNAL(signalSetCutButtonOff()),
                this, SLOT(setCutButtonOff()));
}

QWidget* GraphTools::breakThresholdBox()
{
    QGroupBox* widget = new QGroupBox("Break Threshold", this);
    BreakThresholdGroup* breakThresholdBox = new BreakThresholdGroup(this);

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(breakThresholdBox);

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

OmViewGroupState* GraphTools::getViewGroupState(){
    return mParent->getViewGroupState();
}

void GraphTools::updateGui(){
    OmEvents::Redraw2d();
}

SegmentationDataWrapper GraphTools::GetSDW(){
    return mParent->GetSDW();
}

void GraphTools::ShowSplitterBusy(const bool showBusy){
    signalSplitterBusy(showBusy);
}

void GraphTools::SetSplittingOff(){
    signalSplittingOff();
}

void GraphTools::setSplittingOff(){
    splitButton->setChecked(false);
}

void GraphTools::showSplitterBusy(const bool showBusy)
{
    if(showBusy){
        splitButton->SetIcon(":/toolbars/mainToolbar/icons/ajax-loader.gif");
    } else {
        splitButton->SetIcon();
    }
}

void GraphTools::RefreshThreshold(){
    threshold_->RefreshThreshold();
}
