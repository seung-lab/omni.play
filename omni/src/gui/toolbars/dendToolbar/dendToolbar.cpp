#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/autoBreakCheckbox.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"

DendToolBar::DendToolBar( MainWindow * mw )
	: QToolBar("Dend", mw)
	, mMainWindow(mw)
	, mViewGroupState(NULL)
{
	this->setFloatable( true );
	mMainWindow->addToolbarRight(this);

	addToolbars();
}

SegmentationDataWrapper DendToolBar::getSegmentationDataWrapper()
{
	const OmId segmentationID = 1;
	return SegmentationDataWrapper(segmentationID);
}

void DendToolBar::addToolbars()
{
	autoBreakCheckbox = new AutoBreakCheckbox(this);

	splitButton = new SplitButton(this);
	QGroupBox* firstBox = new QGroupBox(this);
	QVBoxLayout* firstLayout = new QVBoxLayout(firstBox);
	firstLayout->addWidget(splitButton);
	firstLayout->addWidget(autoBreakCheckbox);
	firstBox->setLayout(firstLayout);
	this->addWidget(firstBox);
	
	this->addWidget(new JoinButton(this));
	this->addWidget(new ThresholdGroup(this));
	this->addWidget(new BreakButton(this));
	//this->addWidget(new BreakThresholdGroup(this));
	this->addWidget(new ValidationGroup(this));
	this->addWidget(new Dust3DThresholdGroup(this));
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
	splitButton->setChecked(false);
}
