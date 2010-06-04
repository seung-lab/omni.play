#ifndef SEGMENT_LIST_ABSTRACT_H
#define SEGMENT_LIST_ABSTRACT_H

#include <QtGui>
#include <QWidget> 

#include "utility/dataWrappers.h"
#include "segment/omSegmentPointers.h"

class ElementListBox;
class InspectorProperties;
class OmSegmentEvent;
class OmSegmentListWidget;
class SegObjectInspector;

class SegmentListAbstract : public QWidget
{
	Q_OBJECT

public:
	SegmentListAbstract( QWidget * , InspectorProperties *, ElementListBox * );
	void populateSegmentElementsListWidget(const bool doScrollToSelectedSegment =
					       false, const OmId segmentJustSelectedID = 0);

	void makeSegmentationActive(const OmId segmentationID);
	void makeSegmentationActive(SegmentationDataWrapper sdw);
	void makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID);
	void makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID);

	void rebuildSegmentList(const OmId segmentationID, const OmId segmentJustAddedID);
	void dealWithSegmentObjectModificationEvent(OmSegmentEvent * event);

public slots:
	void rebuildSegmentList(const OmId segmentationID);
	void goToNextPage();
	void goToPrevPage();

protected slots: 


protected:
	virtual QString getTabTitle() = 0;
	virtual bool shouldSegmentBeAdded( OmSegment * seg ) = 0;
	virtual int getPreferredTabIndex() = 0;

        QVBoxLayout * layout;
        QPushButton * prevButton;
        QPushButton * nextButton;

	OmSegmentListWidget * segmentListWidget;
	ElementListBox * elementListBox;

	SegmentationDataWrapper currentSDW;
	bool haveValidSDW;

	OmSegPtrList * getSegmentsToDisplay( const OmId firstSegmentID );
	OmSegPtrList * doGetSegmentsToDisplay( const unsigned int offset );

	QMenu * contextMenu;
	QAction * propAct;
	int getNumSegmentsPerPage();
	void setupPageButtons();
	
	int currentPageNum;
	quint32 getMaxSegmentValue();
};

#endif
