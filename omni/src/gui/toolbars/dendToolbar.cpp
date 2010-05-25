#include "gui/toolbars/dendToolbar.h"
#include "gui/mainwindow.h"
#include "system/omProjectData.h"
#include "system/omCacheManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "system/events/omToolModeEvent.h"
#include "system/events/omView3dEvent.h"

// FIXME
bool mShatter = false;
bool mSplitting = false;
OmId mSegment = 0;
OmId mSeg = 1;


DendToolBar::DendToolBar( MainWindow * mw )
	: QWidget(mw)
{
	mMainWindow = mw;
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
	toolbarSplitAct->setEnabled(false);
}

void DendToolBar::createToolbarActions()
{
	toolbarSplitAct = new QPushButton(mMainWindow);
	toolbarSplitAct->setText(tr("split"));
	toolbarSplitAct->setStatusTip(tr("Split object mode"));
	connect(toolbarSplitAct, SIGNAL(pressed()), 
		this, SLOT(split()));
	toolbarSplitAct->setCheckable(true);
 

	decreaseThresholdAct = new QPushButton(mMainWindow);
	decreaseThresholdAct->setText(tr("-"));
	decreaseThresholdAct->setStatusTip(tr("Split object mode"));

	connect(decreaseThresholdAct, SIGNAL(pressed()), 
		this, SLOT(decreaseThreshold()));

	thresholdLabel = new QLabel(mMainWindow);
	thresholdLabel->setText("Overall Threshold");

	mThreshold = new QLineEdit(mMainWindow);
	QString value;

	setThresholdValue();
	connect(mThreshold, SIGNAL(editingFinished()), 
		this, SLOT(thresholdChanged()));

	increaseThresholdAct =  new QPushButton(mMainWindow);
	
	increaseThresholdAct->setText(tr("+"));	

	decreaseThresholdAct->setStatusTip(tr("Split object mode"));
	connect(increaseThresholdAct, SIGNAL(pressed()), 
		this, SLOT(increaseThreshold()));

        joinAct = new QPushButton(mMainWindow);
	joinAct->setText("Join");
        joinAct->setStatusTip(tr("Join objects mode"));
        connect(joinAct, SIGNAL(pressed()),
                this, SLOT(join()));

        toolbarShatterAct = new QPushButton(mMainWindow);
	toolbarShatterAct->setText("Break");
        toolbarShatterAct->setStatusTip(tr("Shatter object mode"));
        connect(toolbarShatterAct, SIGNAL(pressed()),
                this, SLOT(toggledShatter()));
        toolbarShatterAct->setCheckable(true);

	mBreakThreshold = new QLineEdit(mMainWindow);
	setBreakThresholdValue();
	connect(mBreakThreshold, SIGNAL(editingFinished()), 
		this, SLOT(breakThresholdChanged()));

	increaseBreakThresholdAct = new QPushButton(mMainWindow);
	increaseBreakThresholdAct->setText(tr("+"));
	increaseBreakThresholdAct->setStatusTip(tr("Increase threshold"));
	connect(increaseBreakThresholdAct, SIGNAL(pressed()), 
		this, SLOT(increaseBreakThreshold()));

	decreaseBreakThresholdAct = new QPushButton(mMainWindow);
	decreaseBreakThresholdAct->setText(tr("-"));
	decreaseBreakThresholdAct->setStatusTip(tr("Decrease threshold"));
	connect(decreaseBreakThresholdAct, SIGNAL(pressed()), 
		this, SLOT(decreaseBreakThreshold()));	


        mergeHintAct = new QPushButton(mMainWindow);
	mergeHintAct->setText(tr("Show Merge Hints"));
        mergeHintAct->setStatusTip(tr("Merge hint mode"));
        connect(mergeHintAct, SIGNAL(pressed()),
                this, SLOT(toggledHint()));
        mergeHintAct->setCheckable(true);

        mHint = new QLineEdit(mMainWindow);
        value.setNum(1);
        mHint->setText(value);

        addGroupAct = new QPushButton(mMainWindow);
        addGroupAct->setText(tr("Group Selected"));
        addGroupAct->setStatusTip(tr("group object action"));
        connect(addGroupAct, SIGNAL(pressed()),
                this, SLOT(addGroup()));

        colorMapAct = new QPushButton(mMainWindow);
        colorMapAct->setText(tr("Show Validated Colors"));
        colorMapAct->setStatusTip(tr("Validated object mode"));
        connect(colorMapAct, SIGNAL(pressed()),
                this, SLOT(mapColors()));
        colorMapAct->setCheckable(true);

}

void DendToolBar::setThresholdValue()
{
	QString value;

	if (OmProject::IsSegmentationValid(mSeg)) {
		OmSegmentation & seg = OmProject::GetSegmentation(mSeg);
		value.setNum( seg.GetDendThreshold() );
	} else {
		value.setNum(0.95);
	}
	mThreshold->setText(value);
}

void DendToolBar::setBreakThresholdValue()
{
	QString value;
	value.setNum(0.95);

	mBreakThreshold->setText(value);
}

void DendToolBar::addToolbars()
{
	dendToolBar = new QToolBar( "Dend", this );
	dendToolBar->setFloatable( true );
	mMainWindow->addToolBar( Qt::RightToolBarArea, dendToolBar );

	QGroupBox* firstBox = new QGroupBox(this);
	QVBoxLayout* firstLayout = new QVBoxLayout(firstBox);
	
	firstLayout->addWidget(toolbarSplitAct);
	firstBox->setLayout(firstLayout);
	dendToolBar->addWidget(firstBox);

	QGroupBox* secondBox = new QGroupBox(this);
	QVBoxLayout* secondLayout = new QVBoxLayout(secondBox);
	secondBox->setLayout(secondLayout);	
	secondLayout->addWidget(joinAct);
	dendToolBar->addWidget(secondBox);

	QGroupBox* thirdBox = new QGroupBox(this);
	QGridLayout* thirdLayout = new QGridLayout(thirdBox);
	thirdLayout->addWidget(thresholdLabel,0,0,1,2);
	thirdLayout->addWidget(decreaseThresholdAct,2,0,1,1);
	thirdLayout->addWidget(mThreshold,1,0,1,2);
	thirdLayout->addWidget(increaseThresholdAct,2,1,1,1);
	thirdBox->setLayout(thirdLayout);
	dendToolBar->addWidget(thirdBox);

	QGroupBox* fourthBox = new QGroupBox(this);
	QGridLayout* fourthLayout = new QGridLayout(fourthBox);
	fourthLayout->addWidget(mergeHintAct,0,0,1,1);
	fourthLayout->addWidget(mHint,1,0,1,1);
	fourthBox->setLayout(fourthLayout);
	dendToolBar->addWidget(fourthBox);

	QGroupBox* fifthBox = new QGroupBox(this);
	QGridLayout* fifthLayout = new QGridLayout(fifthBox);
	QLabel* breakThresholdLabel = new QLabel(mMainWindow);
	breakThresholdLabel->setText("Threshold:");
	fifthLayout->addWidget(toolbarShatterAct,0,0,1,2);
	fifthLayout->addWidget(breakThresholdLabel,1,0,1,1);
	fifthLayout->addWidget(mBreakThreshold,2,0,1,2);
	fifthLayout->addWidget(increaseBreakThresholdAct,3,1,1,1);
	fifthLayout->addWidget(decreaseBreakThresholdAct,3,0,1,1);
	fifthBox->setLayout(fifthLayout);
	dendToolBar->addWidget(fifthBox);

        QGroupBox* sixthBox = new QGroupBox(this);
        QGridLayout* sixthLayout = new QGridLayout(sixthBox);
        QLabel* groupColorLabel = new QLabel(mMainWindow);
        groupColorLabel->setText("Groups & Colors:");
        sixthLayout->addWidget(groupColorLabel,0,0,1,2);
        sixthLayout->addWidget(addGroupAct,1,0,1,2);
        sixthLayout->addWidget(colorMapAct,2,0,1,2);
        sixthBox->setLayout(sixthLayout);
        dendToolBar->addWidget(sixthBox);

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

void DendToolBar::toolbarSplit(const bool )
{
        debug("dendbar", "DendToolBar::toolbarSplit\n");
}

void DendToolBar::resetTool(QAction * tool, const bool enabled)
{
        debug("dendbar", "DendToolBar::resetTool\n");
	tool->setChecked(false);
	tool->setEnabled(enabled);
}

void DendToolBar::SystemModeChangeEvent()
{
        debug("dendbar", "DendToolBar::SystemModeChangeEvent\n");

/*
	if( DEND_MODE != OmStateManager::GetSystemMode() ){
		toolbarSplitAct->setChecked( false );
	}
*/
}

void DendToolBar::updateReadOnlyRelatedWidgets()
{
        debug("dendbar", "DendToolBar::updateReadOnlyRelatedWidgets\n");
	bool toBeEnabled = false;

	if ( mMainWindow->isProjectOpen() && !OmProjectData::IsReadOnly() ){
		toBeEnabled = true;
	}

	toolbarSplitAct->setEnabled(toBeEnabled);
}

void DendToolBar::split()
{
	//toolbarSplitAct->setChecked(!toolbarSplitAct->isChecked());
        debug("dendbar", "DendToolBar::split(%i)\n", toolbarSplitAct->isChecked());
	if(!toolbarSplitAct->isChecked()) {
		OmStateManager::SetSystemMode(DEND_MODE);	
		OmStateManager::SetDendToolMode(SPLIT);
	} else {
		OmStateManager::SetSystemModePrev();
	}
}

void DendToolBar::addToThreshold(float num)
{
        QString value = mThreshold->text();
        float threshold = value.toFloat();
        threshold += num;
	if(threshold > 1.0) {
		threshold = 1.0;
	}
	if(threshold < 0.0) {
		threshold = 0.0;
	}
        value.setNum(threshold);
        mThreshold->setText(value);

	if (OmProject::IsSegmentationValid(mSeg)) {
		OmSegmentation & seg = OmProject::GetSegmentation(mSeg);
		seg.SetDendThresholdAndReload(threshold);
	}
}

void DendToolBar::addToBreakThreshold(float num)
{
        QString value = mBreakThreshold->text();
        float threshold = value.toFloat();
        threshold += num;
	if(threshold > 1.0) {
		threshold = 1.0;
	}
	if(threshold < 0.0) {
		threshold = 0.0;
	}
        value.setNum(threshold);
        mBreakThreshold->setText(value);

	mViewGroupState->setBreakThreshold( threshold );
}

void DendToolBar::increaseBreakThreshold()
{
	addToBreakThreshold(0.0002);

	updateGui();
}

void DendToolBar::decreaseBreakThreshold()
{
	debug("dendbar", "DendToolBar::decreaseBreakThreshold\n");
	addToBreakThreshold(-0.0002);

	updateGui();
}

void DendToolBar::increaseThreshold()
{
        debug("dendbar", "DendToolBar::increaseThreshold\n");
	addToThreshold(0.0002);

	updateGui();
}

void DendToolBar::decreaseThreshold()
{
	debug("dendbar", "DendToolBar::decreaseThreshold\n");
	addToThreshold(-0.0002);

	updateGui();
}

void DendToolBar::join()
{
        debug("dendbar", "DendToolBar::join\n");

	if (OmProject::IsSegmentationValid(mSeg)) {
		OmSegmentation & seg = OmProject::GetSegmentation(mSeg);
		seg.JoinAllSegmentsInSelectedList();
	}

	updateGui();
}

void DendToolBar::toggledShatter()
{
	debug("dendbar", "DendToolBar::toggle shatter\n");

	OmCacheManager::Freshen(true);
	mShatter = !mShatter;

	updateGui();
}

void DendToolBar::toggledHint()
{
}

void DendToolBar::thresholdChanged()
{
	debug("dendbar", "DendToolBar::thresholdChanged\n");

	float threshold = mThreshold->text().toFloat();
	if (OmProject::IsSegmentationValid(mSeg)) {
		OmSegmentation & seg = OmProject::GetSegmentation(mSeg);
		seg.SetDendThresholdAndReload(threshold);
	}

	updateGui();
}

void DendToolBar::breakThresholdChanged()
{
	debug("dendbar", "DendToolBar::breakThresholdChanged\n");

	const float threshold = mBreakThreshold->text().toFloat();
	mViewGroupState->setBreakThreshold( threshold );

	updateGui();
}

void DendToolBar::updateGuiFromProjectLoadOrOpen( OmViewGroupState * vgs )
{
        debug("dendbar", "DendToolBar::updateGuiFromProjectLoadOrOpen\n");

	setThresholdValue();
	mViewGroupState = vgs;
	updateGui();
}

void DendToolBar::updateGui()
{
	debug("dendbar", "DendToolBar::updateGui\n");

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
}

bool DendToolBar::GetShatterMode()
{
	return mShatter;
}

bool DendToolBar::GetSplitMode()
{
	return mSplitting;
}

bool DendToolBar::GetSplitMode(OmId & seg, OmId & segment)
{
	seg = mSeg;
	segment = mSegment;
	return mSplitting;
}

void DendToolBar::SetSplitMode(bool onoroff)
{
	mSplitting = onoroff;
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void DendToolBar::SetSplitMode(OmId seg, OmId segment)
{
	mSeg = seg;
	mSegment = segment;
	SetSplitMode(true);
}

// FIXME: need to be moved to somewhere else.
void DendToolBar::addGroup()
{
	debug("group", "DendToolBar::addGroup\n");
        if (OmProject::IsSegmentationValid(mSeg)) {
                OmSegmentation & seg = OmProject::GetSegmentation(mSeg);
                seg.AddGroup((OmIds&) seg.GetSelectedSegmentIds());
        }

}

void DendToolBar::mapColors()
{
	debug("map", "DendToolBar::mapColors\n");
}

