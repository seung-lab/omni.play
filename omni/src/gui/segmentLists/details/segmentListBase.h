#ifndef SEGMENT_LIST_BASE_H
#define SEGMENT_LIST_BASE_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

#include <QtGui>
#include <QWidget>

class OmSegIDsListWithPage;
class OmViewGroupState;
class OmSegmentEvent;
class OmSegmentListWidget;
class SegObjectInspector;
class OmViewGroupState;

class SegmentListBase : public QWidget {
	Q_OBJECT

public:
	SegmentListBase(QWidget*, OmViewGroupState*);

	void populate(const bool doScrollToSelectedSegment = false,
				  const OmSegID segmentJustSelectedID = 0,
				  const bool useOffset = false);

	void makeSegmentationActive(const SegmentDataWrapper& sdw,
								const bool doScroll );

	SegmentationDataWrapper
	dealWithSegmentObjectModificationEvent(OmSegmentEvent* event);

	void userJustClickedInThisSegmentList();
	void rebuildSegmentList(const SegmentDataWrapper& sdw);

	virtual bool shouldSelectedSegmentsBeAddedToRecentList() = 0;

public slots:
	void goToNextPage();
	void goToPrevPage();
	void goToEndPage();
	void goToStartPage();
	void searchChanged();

protected:
	virtual QString getTabTitle() = 0;
	virtual uint64_t Size() = 0;
	virtual int getPreferredTabIndex() = 0;
	virtual void makeTabActiveIfContainsJumpedToSegment() = 0;
	virtual boost::shared_ptr<OmSegIDsListWithPage>
	getPageSegments(const unsigned int offset,
					const int numToGet,
					const OmSegID startSeg) = 0;

	QVBoxLayout* layout;
	QPushButton* prevButton;
	QPushButton* nextButton;
	QPushButton* startButton;
	QPushButton* endButton;
	QLineEdit* searchEdit;

	OmSegmentListWidget* segmentListWidget;

	SegmentationDataWrapper sdw_;
	bool haveValidSDW;

	boost::shared_ptr<OmSegIDsListWithPage>
	getSegmentsToDisplay( const OmID firstSegmentID, const bool  );

	QMenu* contextMenu;
	QAction* propAct;
	int getNumSegmentsPerPage();
	void setupPageButtons();

	int currentPageNum;
	quint32 getTotalNumberOfSegments();

	OmViewGroupState * vgs_;
};

#endif
