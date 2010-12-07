#ifndef ELEMENT_LIST_BOX_IMPL_H
#define ELEMENT_LIST_BOX_IMPL_H

#include "common/omDebug.h"
#include "gui/segmentLists/details/segmentListRecent.h"
#include "gui/segmentLists/details/segmentListUncertain.h"
#include "gui/segmentLists/details/segmentListValid.h"
#include "gui/segmentLists/details/segmentListWorking.h"
#include "gui/segmentLists/segmentListKeyPressEventListener.h"
#include "gui/widgets/omProgressBar.hpp"
#include "system/events/omSegmentEvent.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

#include <QtGui>
#include <QWidget>

class OmViewGroupState;

class ElementListBoxImpl : public QGroupBox,
						   public OmSegmentEventListener,
						   public SegmentListKeyPressEventListener {
Q_OBJECT

protected:
	void keyPressEvent(QKeyEvent* event){
		SegmentListKeyPressEventListener::keyPressEvent(event);
	}

private:
	QTabWidget* mDataElementsTabs;
	QVBoxLayout* mOverallContainer;
	OmProgressBar<uint64_t>* mValidProgress;

	int mCurrentlyActiveTab;

	SegmentListRecent * recentList_;
	SegmentListUncertain* uncertainList_;
	SegmentListValid * validList_;
	SegmentListWorking * workingList_;

	void SegmentDataModificationEvent() {}
	void SegmentEditSelectionChangeEvent() {}
	void SegmentObjectModificationEvent(OmSegmentEvent * event)
	{
		reset();
		setTitle("");

		const SegmentationDataWrapper sdw
			= workingList_->dealWithSegmentObjectModificationEvent(event);
		validList_->dealWithSegmentObjectModificationEvent(event);
		uncertainList_->dealWithSegmentObjectModificationEvent(event);
		recentList_->dealWithSegmentObjectModificationEvent(event);

		if(sdw.IsSegmentationValid()){
			setTitle(GetSegmentationGroupBoxTitle(sdw));
			updateValidBar(sdw);
		}
	}

	void updateValidBar(const SegmentationDataWrapper& sdw)
	{
		const uint64_t valid = sdw.GetSegmentLists()->Valid().VoxelCount();
		const uint64_t working = sdw.GetSegmentLists()->Working().VoxelCount();
		const uint64_t uncertain = sdw.GetSegmentLists()->Uncertain().VoxelCount();


		mValidProgress->setMaximum(valid + working + uncertain);
		mValidProgress->setValue(valid);
	}

	QString GetSegmentationGroupBoxTitle(SegmentationDataWrapper sdw) {
		return QString("Segmentation %1: Segments").arg(sdw.getID());
	}

public:

	ElementListBoxImpl(OmViewGroupState* vgs)
		: QGroupBox("")
		, mCurrentlyActiveTab(-1)
	{
		workingList_ = new SegmentListWorking(this, vgs);
		validList_ = new SegmentListValid(this, vgs);
		recentList_ = new SegmentListRecent(this, vgs);
		uncertainList_ = new SegmentListUncertain(this, vgs);

		mDataElementsTabs = new QTabWidget( this );
		mOverallContainer = new QVBoxLayout( this );
		mOverallContainer->addWidget( mDataElementsTabs );

		mValidProgress = new OmProgressBar<uint64_t>(this);
		mOverallContainer->addWidget(mValidProgress);
	}

	void reset()
	{
		mDataElementsTabs->clear();
		setTitle("");
	}

	void setActiveTab(QWidget* tab)
	{
		mCurrentlyActiveTab = mDataElementsTabs->indexOf(tab);
	}

	void addTab(const int preferredIndex, QWidget* tab, const QString& tabTitle)
	{
		if( -1 != mDataElementsTabs->indexOf(tab) ){
			return; // tab was already added, don't add again
		}

		const int insertedIndex =
			mDataElementsTabs->insertTab(preferredIndex, tab, tabTitle);

		if( -1 == mCurrentlyActiveTab ){ // first time here
			mCurrentlyActiveTab = insertedIndex;
		}

		// keep the tab widget fixed on the user-specified tab
		mDataElementsTabs->setCurrentIndex( mCurrentlyActiveTab );
	}

	void PopulateLists()
	{
		workingList_->populate();
		validList_->populate();
		recentList_->populate();
		uncertainList_->populate();
	}

	void RebuildLists(const SegmentDataWrapper& sdw)
	{
		workingList_->rebuildSegmentList(sdw);
		validList_->rebuildSegmentList(sdw);
		recentList_->rebuildSegmentList(sdw);
		uncertainList_->rebuildSegmentList(sdw);

		updateValidBar(sdw.MakeSegmentationDataWrapper());
	}

	void UpdateSegmentListBox(const SegmentationDataWrapper& sdw)
	{
		reset();
		setTitle(GetSegmentationGroupBoxTitle(sdw));
		makeSegmentationActive(sdw);

		updateValidBar(sdw);
	}

private:

	void makeSegmentationActive(const SegmentationDataWrapper& sdw)
	{
		SegmentDataWrapper s(sdw, 0);

		workingList_->makeSegmentationActive(s, true );
		validList_->makeSegmentationActive(s, true );
		recentList_->makeSegmentationActive(s, true );
		uncertainList_->makeSegmentationActive(s, true);

		updateValidBar(sdw);
	}
};

#endif
