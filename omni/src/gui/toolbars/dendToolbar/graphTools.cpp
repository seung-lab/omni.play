#include "gui/toolbars/dendToolbar/autoBreakCheckbox.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "system/omEvents.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

GraphTools::GraphTools(DendToolBar * d)
	: QGroupBox("Graph Tools", d)
	, mParent(d)
	, autoBreakCheckbox(new AutoBreakCheckbox(this))
	, splitButton(new SplitButton(this))
{
	QVBoxLayout* box = new QVBoxLayout(this);
	box->addWidget(new ThresholdGroup(this));
	box->addWidget(new JoinButton(this));
	box->addWidget(splitButton);
	//	box->addWidget(autoBreakCheckbox);
	autoBreakCheckbox->hide();
	box->addWidget(new BreakButton(this));
}

void GraphTools::SetSplittingOff()
{
	splitButton->setChecked(false);
}

OmViewGroupState * GraphTools::getViewGroupState()
{
	return mParent->getViewGroupState(); 
}

void GraphTools::updateGui()
{
	OmEvents::Redraw();
}

SegmentationDataWrapper GraphTools::getSegmentationDataWrapper()
{
	return mParent->getSegmentationDataWrapper();
}
