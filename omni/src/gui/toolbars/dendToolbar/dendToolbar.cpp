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
#include "gui/toolbars/dendToolbar/groupButtonAdd.h"
#include "gui/toolbars/dendToolbar/groupButtonDelete.h"
#include "gui/toolbars/dendToolbar/groupButtonTag.h"
#include "gui/toolbars/dendToolbar/showValidatedButton.h"

DendToolBar::DendToolBar( MainWindow * mw )
	: QToolBar("Dend", mw)
	, mMainWindow(mw)
	, mViewGroupState(NULL)
{
	this->setFloatable( true );
	mMainWindow->addToolbarRight(this);

	createToolbarActions();
	addToolbars();
}

SegmentationDataWrapper DendToolBar::getSegmentationDataWrapper()
{
	const OmId segmentationID = 1;
	return SegmentationDataWrapper(segmentationID);
}

void DendToolBar::createToolbarActions()
{
	autoBreakCheckbox = new QCheckBox(mMainWindow);
	autoBreakCheckbox->setText(tr("Show Breaks"));
	autoBreakCheckbox->setChecked(true);
        connect(autoBreakCheckbox, SIGNAL(stateChanged(int)),
                this, SLOT(autoBreakChecked()));

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

        mGroupName = new QLineEdit(mMainWindow);
        mGroupName->setText("Glia");
}

void DendToolBar::addToolbars()
{
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

	groupButtonAdd = new GroupButtonAdd(this);
	groupButtonDelete = new GroupButtonDelete(this);
	groupButtonTag = new GroupButtonTag(this);
	showValidatedButton = new ShowValidatedButton(this);

        QGroupBox* sixthBox = new QGroupBox(this);
	sixthBox->setTitle("Validation");
        QGridLayout* sixthLayout = new QGridLayout(sixthBox);
        sixthLayout->addWidget(groupButtonAdd,0,0,1,2);
        sixthLayout->addWidget(groupButtonDelete,1,0,1,2);
        sixthLayout->addWidget(showValidatedButton,2,0,1,2);
        sixthLayout->addWidget(showValid,3,0,1,1);
        sixthLayout->addWidget(dontShowValid,3,1,1,1);
        sixthLayout->addWidget(groupButtonTag,4,0,1,1);
        sixthLayout->addWidget(mGroupName,4,1,1,1);
        sixthBox->setLayout(sixthLayout);
        this->addWidget(sixthBox);

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

void DendToolBar::changeMapColors()
{
	debug("valid", "DendToolBar::changeMapColors(%i)\n", showValidatedButton->isChecked());
	// Using !(not) because check happens after this fuction.
	mViewGroupState->SetShowValidMode(showValidatedButton->isChecked(), showValid->isChecked());
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

QString DendToolBar::getGroupNameFromGUI()
{
	return mGroupName->text();
}

bool DendToolBar::isShowValidChecked()
{
	return showValid->isChecked();
}
