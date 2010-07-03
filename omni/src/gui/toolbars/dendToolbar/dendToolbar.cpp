#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/displayTools.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"

DendToolBar::DendToolBar( MainWindow * mw )
	: QToolBar("Dend", mw)
	, mMainWindow(mw)
	, mViewGroupState(NULL)
	, graphTools(new GraphTools(this))
{
	mMainWindow->addToolbarRight(this);

	//addWidget(new BreakThresholdGroup(this));
	addWidget(graphTools);
	addWidget(new ValidationGroup(this));
	addWidget(new DisplayTools(this));
}

SegmentationDataWrapper DendToolBar::getSegmentationDataWrapper()
{
	const OmId segmentationID = 1;
	return SegmentationDataWrapper(segmentationID);
}

void DendToolBar::updateGuiFromProjectLoadOrOpen( OmViewGroupState * vgs )
{
        debug("dendbar", "DendToolBar::updateGuiFromProjectLoadOrOpen\n");
	mViewGroupState = vgs;
	updateGui();
}

void DendToolBar::updateGui()
{
	OmEvents::Redraw();
}

void DendToolBar::SetSplittingOff()
{
	graphTools->SetSplittingOff();
}
