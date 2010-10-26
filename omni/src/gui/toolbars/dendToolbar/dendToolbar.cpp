#include "common/omDebug.h"
#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/displayTools.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"

DendToolBar::DendToolBar(MainWindow* mw, OmViewGroupState* vgs)
	: QToolBar("Dend", mw)
	, mMainWindow(mw)
	, mViewGroupState(vgs)
	, graphToolsDock(NULL)
	, validationGroupDock(NULL)
	, displayToolsDock(NULL)
{
	graphTools = new GraphTools(this);
	validationGroup = new ValidationGroup(this);
	displayTools = new DisplayTools(this);

	mMainWindow->addToolbarRight(this);
	addWidget(wrapWithGroupBox(graphTools));
	addWidget(wrapWithGroupBox(validationGroup));
	addWidget(wrapWithGroupBox(displayTools));

	//addWidget(new BreakThresholdGroup(this));

	/*
	graphToolsDock = makeDockWidget(graphTools);
	validationGroupDock = makeDockWidget(validationGroup);
	displayToolsDock = makeDockWidget(displayTools);
	updateToolBarsPos(QPoint(0,0));
	*/
}

QDockWidget* DendToolBar::makeDockWidget(OmWidget* widget)
{
	QDockWidget* toolDock = new QDockWidget(widget->getName(), this, Qt::Tool );

	toolDock->setWidget(widget);
	widget->setParent(toolDock);

	return toolDock;
}

void DendToolBar::updateToolBarsPos(QPoint)
{
	QPoint newPos = QPoint(mMainWindow->width(), 0);
	const int extraVerticalPadding = 22;

	int height = 0;
	if(graphToolsDock){
		height += moveToolDock(graphToolsDock, recalcPos(newPos, height));
		height += extraVerticalPadding;
	}
	if(validationGroupDock){
		height += moveToolDock(validationGroupDock, recalcPos(newPos, height));
		height += extraVerticalPadding;
	}
	if(displayToolsDock){
		height += moveToolDock(displayToolsDock, recalcPos(newPos, height));
	}
}

QPoint DendToolBar::recalcPos(QPoint newPos, const int height)
{
	return QPoint(newPos.x(),
		      newPos.y() + height);
}

int DendToolBar::moveToolDock(QDockWidget* toolDock, QPoint newPos )
{
	toolDock->move(mapToGlobal(newPos));
	toolDock->show();
	return toolDock->height();
}

QWidget* DendToolBar::wrapWithGroupBox(OmWidget* widget)
{
	QGroupBox* gbox = new QGroupBox(widget->getName(), this);
	QVBoxLayout* vbox = new QVBoxLayout(gbox);
	vbox->addWidget(widget);
	gbox->setContentsMargins(0,0,0,0);

	return gbox;
}

SegmentationDataWrapper DendToolBar::getSegmentationDataWrapper()
{
	const OmID segmentationID = 1;
	return SegmentationDataWrapper(segmentationID);
}

void DendToolBar::updateGui()
{
	OmEvents::Redraw();
}

void DendToolBar::SetSplittingOff()
{
	graphTools->SetSplittingOff();
}
