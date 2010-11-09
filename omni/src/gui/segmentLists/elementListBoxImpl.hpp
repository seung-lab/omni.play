#ifndef ELEMENT_LIST_BOX_IMPL_H
#define ELEMENT_LIST_BOX_IMPL_H

#include "gui/segmentLists/details/segmentListRecent.h"
#include "gui/segmentLists/details/segmentListUncertain.h"
#include "gui/segmentLists/details/segmentListValid.h"
#include "gui/segmentLists/details/segmentListWorking.h"
#include "system/events/omSegmentEvent.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

#include <QtGui>
#include <QWidget>

class OmViewGroupState;

class ElementListBoxImpl : public QGroupBox, public OmSegmentEventListener {
Q_OBJECT

private:
	QTabWidget * mDataElementsTabs;
	QVBoxLayout * mOverallContainer;
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

		const OmID segmentationID1 = workingList_->dealWithSegmentObjectModificationEvent(event);
		const OmID segmentationID2 = validList_->dealWithSegmentObjectModificationEvent(event);
		uncertainList_->dealWithSegmentObjectModificationEvent(event);
		recentList_->dealWithSegmentObjectModificationEvent(event);

		if( segmentationID1 > 0 && segmentationID1 == segmentationID2 ){
			SegmentationDataWrapper sdw(segmentationID1);
			setTitle(GetSegmentationGroupBoxTitle(sdw));
		}
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
	}

	void reset()
	{
		mDataElementsTabs->clear();
		setTitle("");
	}

	void setActiveTab( QWidget * tab )
	{
		mCurrentlyActiveTab = mDataElementsTabs->indexOf(tab);
	}

	void addTab( const int preferredIndex, QWidget * tab, const QString & tabTitle)
	{
		if( -1 != mDataElementsTabs->indexOf(tab) ){
			return; // tab was already added, don't add again
		}

		const int insertedIndex = mDataElementsTabs->insertTab(preferredIndex, tab, tabTitle);

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
		workingList_->rebuildSegmentList(sdw.GetSegmentationID(), sdw.getID());
		validList_->rebuildSegmentList(sdw.GetSegmentationID(), sdw.getID());
		recentList_->rebuildSegmentList(sdw.GetSegmentationID(), sdw.getID());
		uncertainList_->rebuildSegmentList(sdw.GetSegmentationID(), sdw.getID());
	}

	void UpdateSegmentListBox(const SegmentationDataWrapper& sdw)
	{
		reset();
		setTitle(GetSegmentationGroupBoxTitle(sdw));
		makeSegmentationActive(sdw);
	}

private:

	void makeSegmentationActive(const SegmentationDataWrapper& sdw)
	{
		workingList_->makeSegmentationActive(sdw, 0, true );
		validList_->makeSegmentationActive(sdw, 0, true );
		recentList_->makeSegmentationActive(sdw, 0, true );
		uncertainList_->makeSegmentationActive(sdw, 0, true);
	}
};

#endif
