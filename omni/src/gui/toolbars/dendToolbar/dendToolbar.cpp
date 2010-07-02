#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "gui/toolbars/dendToolbar/joinButton.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "segment/omSegmentCache.h"
#include "system/omEvents.h"
#include "system/omProjectData.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

DendToolBar::DendToolBar( MainWindow * mw )
	: QWidget(mw)
	, mMainWindow(mw)
	, mViewGroupState(NULL)
	, splitButton(new SplitButton(this))
{
	createToolbarActions();
	addToolbars();
}

OmId DendToolBar::getSegmentationID()
{
	return 1;
}

SegmentationDataWrapper DendToolBar::getSegmentationDataWrapper()
{
	return SegmentationDataWrapper(getSegmentationID());
}

void DendToolBar::createToolbarActions()
{
	autoBreakCheckbox = new QCheckBox(mMainWindow);
	autoBreakCheckbox->setText(tr("Show Breaks"));
	autoBreakCheckbox->setChecked(true);
        connect(autoBreakCheckbox, SIGNAL(stateChanged(int)),
                this, SLOT(autoBreakChecked()));

        addGroupAct = new QPushButton(mMainWindow);
        addGroupAct->setText(tr("Set Selection Valid"));
        addGroupAct->setStatusTip(tr("Locking selected objects"));
        connect(addGroupAct, SIGNAL(pressed()),
                this, SLOT(addGroup()));

        deleteGroupAct = new QPushButton(mMainWindow);
        deleteGroupAct->setText(tr("Set Selection Not Valid"));
        deleteGroupAct->setStatusTip(tr("Unlock selected objects"));
        connect(deleteGroupAct, SIGNAL(pressed()),
                this, SLOT(deleteGroup()));

        colorMapAct = new QPushButton(mMainWindow);
        colorMapAct->setText(tr("Show Validated"));
        colorMapAct->setStatusTip(tr("Validated object mode"));
        connect(colorMapAct, SIGNAL(pressed()),
                this, SLOT(mapColors()));
        colorMapAct->setCheckable(true);

	validGroup = new QButtonGroup();
	showValid = new QRadioButton("In Color");
	validGroup->addButton(showValid);
        connect(showValid, SIGNAL(toggled(bool)),
                this, SLOT(changeMapColors()));

	dontShowValid = new QRadioButton("As Black");
	dontShowValid->setChecked(true);
	validGroup->addButton(dontShowValid);
        connect(dontShowValid, SIGNAL(toggled(bool)),
                this, SLOT(changeMapColors()));

	specialGroupAct = new QPushButton(mMainWindow);
	specialGroupAct->setText(tr("Group As:"));
        connect(specialGroupAct, SIGNAL(pressed()),
                this, SLOT(specialGroupAdd()));

        mGroupName = new QLineEdit(mMainWindow);
        mGroupName->setText("Glia");
}

void DendToolBar::addToolbars()
{
	dendToolBar = new QToolBar( "Dend", this );
	dendToolBar->setFloatable( true );
	mMainWindow->addToolbarRight(dendToolBar);
	
	QGroupBox* firstBox = new QGroupBox(this);
	QVBoxLayout* firstLayout = new QVBoxLayout(firstBox);
	firstLayout->addWidget(splitButton);
	firstLayout->addWidget(autoBreakCheckbox);
	firstBox->setLayout(firstLayout);
	dendToolBar->addWidget(firstBox);
	
	dendToolBar->addWidget(new JoinButton(this));
	
	dendToolBar->addWidget(new ThresholdGroup(this));
	
	dendToolBar->addWidget(new BreakButton(this));
	//dendToolBar->addWidget(new BreakThresholdGroup(this));

        QGroupBox* sixthBox = new QGroupBox(this);
	sixthBox->setTitle("Validation");
        QGridLayout* sixthLayout = new QGridLayout(sixthBox);
        sixthLayout->addWidget(addGroupAct,0,0,1,2);
        sixthLayout->addWidget(deleteGroupAct,1,0,1,2);
        sixthLayout->addWidget(colorMapAct,2,0,1,2);
        sixthLayout->addWidget(showValid,3,0,1,1);
        sixthLayout->addWidget(dontShowValid,3,1,1,1);
        sixthLayout->addWidget(specialGroupAct,4,0,1,1);
        sixthLayout->addWidget(mGroupName,4,1,1,1);
        sixthBox->setLayout(sixthLayout);
        dendToolBar->addWidget(sixthBox);

	dendToolBar->addWidget(new Dust3DThresholdGroup(this));
}

void DendToolBar::setupToolbarInitially()
{
}

void DendToolBar::ChangeModeModify(const bool )
{
}

void DendToolBar::updateReadOnlyRelatedWidgets()
{
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

// FIXME: need to be moved to somewhere else.
void DendToolBar::addGroup()
{
	debug("dendbar", "DendToolBar::addGroup\n");
	SegmentationDataWrapper sdw = getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}
	
	OmSegmentation & seg = sdw.getSegmentation();
	seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), VALIDROOT, QString("Valid"));
	OmEvents::SegmentModified();
}

void DendToolBar::specialGroupAdd()
{
        debug("dendbar", "DendToolBar::specialGroupAdd\n");
	SegmentationDataWrapper sdw = getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}
	
	OmSegmentation & seg = sdw.getSegmentation();
	seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), GROUPROOT, mGroupName->text());
	OmEvents::SegmentModified();
}

// FIXME: need to be moved to somewhere else.
void DendToolBar::deleteGroup()
{
        debug("dendbar", "DendToolBar::deleteGroup\n");
	SegmentationDataWrapper sdw = getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}

	OmSegmentation & seg = sdw.getSegmentation();
	seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), NOTVALIDROOT, QString("Not Valid"));
	OmEvents::SegmentModified();
}

void DendToolBar::mapColors()
{
	debug("valid", "DendToolBar::mapColors(%i)\n", colorMapAct->isChecked());
	// Using !(not) because check happens after this fuction.
	mViewGroupState->SetShowValidMode(!colorMapAct->isChecked(), showValid->isChecked());
}

void DendToolBar::changeMapColors()
{
	debug("valid", "DendToolBar::mapColors(%i)\n", colorMapAct->isChecked());
	// Using !(not) because check happens after this fuction.
	mViewGroupState->SetShowValidMode(colorMapAct->isChecked(), showValid->isChecked());
}

void DendToolBar::SetSplittingOff()
{
	splitButton->setChecked(false);
}

void DendToolBar::autoBreakChecked()
{
	debug("dendbar", "DendToolBar::autoBreakChecked\n");
	mViewGroupState->SetBreakOnSplitMode(autoBreakCheckbox->isChecked());
}
