#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "segment/actions/segment/omSegmentJoinAction.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omToolModeEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omViewEvent.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "project/omProject.h"
#include "utility/dataWrappers.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "gui/toolbars/dendToolbar/splitButton.h"
#include "gui/toolbars/dendToolbar/breakButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"

bool mShowGroups = false;
static OmId mSeg = 1;

DendToolBar::DendToolBar( MainWindow * mw )
	: QWidget(mw)
	, mMainWindow(mw)
	, mViewGroupState(NULL)
	, splitButton(NULL)
{
	createToolbar();
}

////////////////////////////////////////////////////////////
// Toolbars
////////////////////////////////////////////////////////////
void DendToolBar::createToolbar()
{
	createToolbarActions();
	addToolbars();
}

void DendToolBar::setToolbarDisabled()
{
	splitButton->setEnabled(false);
}

OmId DendToolBar::getSegmentationID()
{
	return mSeg;
}

SegmentationDataWrapper DendToolBar::getSegmentationDataWrapper()
{
	return SegmentationDataWrapper(mSeg);
}

void DendToolBar::createToolbarActions()
{
	

	autoBreakCheckbox = new QCheckBox(mMainWindow);
	autoBreakCheckbox->setText(tr("Show Breaks"));
	autoBreakCheckbox->setChecked(true);
        connect(autoBreakCheckbox, SIGNAL(stateChanged(int)),
                this, SLOT(autoBreakChecked()));


        joinAct = new QPushButton(mMainWindow);
	joinAct->setText("Join");
        joinAct->setStatusTip(tr("Join objects mode"));
        connect(joinAct, SIGNAL(pressed()),
                this, SLOT(join()));


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

	splitButton = new SplitButton(this);
	firstLayout->addWidget(splitButton);

	firstLayout->addWidget(autoBreakCheckbox);
	firstBox->setLayout(firstLayout);
	dendToolBar->addWidget(firstBox);

	QGroupBox* secondBox = new QGroupBox(this);
	QVBoxLayout* secondLayout = new QVBoxLayout(secondBox);
	secondBox->setLayout(secondLayout);	
	secondLayout->addWidget(joinAct);
	dendToolBar->addWidget(secondBox);

	dendToolBar->addWidget(new ThresholdGroup(this));

	breakButton = new BreakButton(this);
	dendToolBar->addWidget(breakButton);
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
        debug("dendbar", "DendToolBar::setupToolbarInitially\n");
	// FIXME tie in the hard code statics.
}

void DendToolBar::ChangeModeModify(const bool )
{
        debug("dendbar", "DendToolBar::ChangeModeModify\n");
}

void DendToolBar::resetTool(QAction * tool, const bool enabled)
{
        debug("dendbar", "DendToolBar::resetTool\n");
	tool->setChecked(false);
	tool->setEnabled(enabled);
}

void DendToolBar::updateReadOnlyRelatedWidgets()
{
        debug("dendbar", "DendToolBar::updateReadOnlyRelatedWidgets\n");
	bool toBeEnabled = false;

	if ( mMainWindow->isProjectOpen() && !OmProjectData::IsReadOnly() ){
		toBeEnabled = true;
	}

	splitButton->setEnabled(toBeEnabled);
}

void DendToolBar::join()
{
        debug("dendbar", "DendToolBar::join\n");

	if (OmProject::IsSegmentationValid(getSegmentationID())) {
		OmSegmentation & seg = OmProject::GetSegmentation(getSegmentationID());
		OmIDsSet ids = seg.GetSegmentCache()->GetSelectedSegmentIds();
		(new OmSegmentJoinAction(mSeg, ids))->Run();
	}	

	updateGui();
}

void DendToolBar::updateGuiFromProjectLoadOrOpen( OmViewGroupState * vgs )
{
        debug("dendbar", "DendToolBar::updateGuiFromProjectLoadOrOpen\n");

	mViewGroupState = vgs;

	updateGui();
}

void DendToolBar::updateGui()
{
	debug("dendbar", "DendToolBar::updateGui\n");

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
}

// FIXME: need to be moved to somewhere else.
void DendToolBar::addGroup()
{
	debug("dendbar", "DendToolBar::addGroup\n");
        if (OmProject::IsSegmentationValid(getSegmentationID())) {
                OmSegmentation & seg = OmProject::GetSegmentation(getSegmentationID());
                seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), VALIDROOT, QString("Valid"));
		OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
        }
}

void DendToolBar::specialGroupAdd()
{
        debug("dendbar", "DendToolBar::specialGroupAdd\n");
        if (OmProject::IsSegmentationValid(getSegmentationID())) {
                OmSegmentation & seg = OmProject::GetSegmentation(getSegmentationID());
                seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), GROUPROOT, mGroupName->text());
                OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
        }
}

// FIXME: need to be moved to somewhere else.
void DendToolBar::deleteGroup()
{
        debug("dendbar", "DendToolBar::addGroup\n");
        if (OmProject::IsSegmentationValid(getSegmentationID())) {
                OmSegmentation & seg = OmProject::GetSegmentation(getSegmentationID());
                seg.SetGroup(seg.GetSegmentCache()->GetSelectedSegmentIds(), NOTVALIDROOT, QString("Not Valid"));
		OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
        }
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
