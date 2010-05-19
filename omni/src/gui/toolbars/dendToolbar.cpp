#include "gui/toolbars/dendToolbar.h"
#include "gui/mainwindow.h"
#include "system/omProjectData.h"
#include "system/omCacheManager.h"

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
	//	setToolbarDisabled();
}

void DendToolBar::setToolbarDisabled()
{
	toolbarSplitAct->setEnabled(false);
}

void DendToolBar::createToolbarActions()
{
	toolbarSplitAct = new QAction(tr("Split"), mMainWindow);
	toolbarSplitAct->setStatusTip(tr("Split object mode"));
	connect(toolbarSplitAct, SIGNAL(triggered()), 
		this, SLOT(split()));
	toolbarSplitAct->setCheckable(true);

	decreaseThresholdAct = new QAction(tr("-"), mMainWindow);
	decreaseThresholdAct->setStatusTip(tr("Split object mode"));
	connect(decreaseThresholdAct, SIGNAL(triggered()), 
		this, SLOT(decreaseThreshold()));

	thresholdLabel = new QLabel("Threshold",mMainWindow);
	thresholdLabel->setText("Threshold");

	mThreshold = new QLineEdit(mMainWindow);
	QString value;

	setThresholdValue();
	connect(mThreshold, SIGNAL(editingFinished()), 
		this, SLOT(thresholdChanged()));

	increaseThresholdAct =  new QAction(tr("+"), mMainWindow);
	decreaseThresholdAct->setStatusTip(tr("Split object mode"));
	connect(increaseThresholdAct, SIGNAL(triggered()), 
		this, SLOT(increaseThreshold()));

        joinAct = new QAction(tr("Join"), mMainWindow);
        joinAct->setStatusTip(tr("Join objects mode"));
        connect(joinAct, SIGNAL(triggered()),
                this, SLOT(join()));

        toolbarShatterAct = new QAction(tr("Shatter"), mMainWindow);
        toolbarShatterAct->setStatusTip(tr("Shatter object mode"));
        connect(toolbarShatterAct, SIGNAL(triggered()),
                this, SLOT(toggledShatter()));
        toolbarShatterAct->setCheckable(true);

        mergeHintAct = new QAction(tr("Show Merge Hints"), mMainWindow);
        mergeHintAct->setStatusTip(tr("Merge hint mode"));
        connect(mergeHintAct, SIGNAL(triggered()),
                this, SLOT(toggledHint()));
        mergeHintAct->setCheckable(true);

        mHint = new QLineEdit(mMainWindow);
        value.setNum(1);
        mHint->setText(value);

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

void DendToolBar::addToolbars()
{
	dendToolBar = new QToolBar( "Dend", this );
	dendToolBar->setFloatable( true );
	mMainWindow->addToolBar( Qt::RightToolBarArea, dendToolBar );

	dendToolBar->addAction(toolbarSplitAct);

	dendToolBar->addAction(decreaseThresholdAct);
	dendToolBar->addWidget(thresholdLabel);
	dendToolBar->addWidget(mThreshold);
	dendToolBar->addAction(increaseThresholdAct);
	dendToolBar->addAction(joinAct);
	dendToolBar->addAction(toolbarShatterAct);
	dendToolBar->addAction(mergeHintAct);
	dendToolBar->addWidget(mHint);
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

	if( DEND_MODE == OmStateManager::GetSystemModePrev() ){
		toolbarSplitAct->setChecked( false );
	}
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
        debug("dendbar", "DendToolBar::split\n");
	if( toolbarSplitAct->isChecked() ) {
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

void DendToolBar::updateGuiFromProjectLoadOrOpen()
{
        debug("dendbar", "DendToolBar::updateGuiFromProjectLoadOrOpen\n");

	setThresholdValue();
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

bool DendToolBar::GetSplitMode(OmId & seg, OmId & segment)
{
	seg = mSeg;
	segment = mSegment;
	return mSplitting;
}

void DendToolBar::SetSplitMode(bool onoroff)
{
	mSplitting = onoroff;
}
void DendToolBar::SetSplitMode(OmId seg, OmId segment)
{
	mSeg = seg;
	mSegment = segment;
	mSplitting = true;
}
