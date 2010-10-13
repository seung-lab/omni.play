#ifndef SEGMENT_LIST_BASE_H
#define SEGMENT_LIST_BASE_H

#include <QtGui>
#include <QWidget>

#include "segment/omSegmentPointers.h"

class SegmentationDataWrapper;
class ElementListBox;
class InspectorProperties;
class OmSegmentEvent;
class OmSegmentListWidget;
class SegObjectInspector;

class SegmentListBase : public QWidget
{
	Q_OBJECT

	public:
	SegmentListBase( QWidget * , InspectorProperties *, ElementListBox * );

	void populateSegmentElementsListWidget(const bool doScrollToSelectedSegment = false,
										   const OmSegID segmentJustSelectedID = 0,
										   const bool useOffset = false);

	void makeSegmentationActive(SegmentationDataWrapper sdw,
								const OmSegID segmentJustSelectedID,
								const bool doScroll );

	int dealWithSegmentObjectModificationEvent(OmSegmentEvent * event);

	void userJustClickedInThisSegmentList();
	void rebuildSegmentList(const OmId segmentationID, const OmSegID segmentJustAddedID);

	virtual bool shouldSelectedSegmentsBeAddedToRecentList() = 0;

public slots:
	void goToNextPage();
	void goToPrevPage();
	void goToEndPage();
	void goToStartPage();
	void searchChanged();

protected:
	virtual QString getTabTitle() = 0;
	virtual OmSegIDRootType getRootSegType() = 0;
	virtual int getPreferredTabIndex() = 0;
	virtual void makeTabActiveIfContainsJumpedToSegment() = 0;

	QVBoxLayout * layout;
	QPushButton * prevButton;
	QPushButton * nextButton;
	QPushButton * startButton;
	QPushButton * endButton;
	QLineEdit * searchEdit;

	OmSegmentListWidget * segmentListWidget;
	ElementListBox * elementListBox;

	boost::shared_ptr<SegmentationDataWrapper> currentSDW;
	bool haveValidSDW;

	OmSegPtrList * getSegmentsToDisplay( const OmId firstSegmentID, const bool  );

	QMenu * contextMenu;
	QAction * propAct;
	int getNumSegmentsPerPage();
	void setupPageButtons();

	int currentPageNum;
	quint32 getTotalNumberOfSegments();
};

#endif
