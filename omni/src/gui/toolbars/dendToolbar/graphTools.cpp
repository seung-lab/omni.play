#include "gui/toolbars/dendToolbar/autoBreakCheckbox.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "gui/toolbars/dendToolbar/cutButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "system/omEvents.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

GraphTools::GraphTools(DendToolBar * d)
	: OmWidget(d)
	, mParent(d)
	, autoBreakCheckbox(new AutoBreakCheckbox(this))
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
	autoBreakCheckbox->hide();
	box2->addWidget(new BreakButton(this), 1, 1, 1, 1);

	box->addWidget(breakThresholdBox());
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
        ThresholdGroup* thresholdBox = new ThresholdGroup(this);

        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(thresholdBox);

        return widget;
}

void GraphTools::SetSplittingOff()
{
	splitButton->setChecked(false);
}

void GraphTools::SetCuttingOff()
{
	printf("GraphTools::SetCuttingOff\n");
	cutButton->setChecked(false);
}

OmViewGroupState * GraphTools::getViewGroupState()
{
	return mParent->getViewGroupState();
}

void GraphTools::updateGui()
{
	OmEvents::Redraw2d();
}

SegmentationDataWrapper GraphTools::GetSegmentationDataWrapper()
{
	return mParent->GetSegmentationDataWrapper();
}
