#ifndef SEGMENT_LIST_ABSTRACT_H
#define SEGMENT_LIST_ABSTRACT_H

#include <QtGui>
#include <QWidget> 

#include "utility/dataWrappers.h"

class ElementListBox;
class InspectorProperties;
class OmSegmentEvent;
class OmTreeWidget;
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
	void addToSplitterDataElementSegment(SegmentDataWrapper sdw );
	void segmentRightClick();
	void segmentLeftClick();

protected:
	virtual QString getTabTitle() = 0;
	virtual bool shouldSegmentBeAdded( SegmentDataWrapper & seg ) = 0;

	static const int ENABLED_COL = 0;
	static const int NAME_COL = 1;
	static const int ID_COL = 2;
	static const int NOTE_COL = 3;
	static const int USER_DATA_COL = 4;

        QVBoxLayout * layout;
        QPushButton * prevButton;
        QPushButton * nextButton;

	OmTreeWidget * dataElementsWidget;
 	InspectorProperties * inspectorProperties;
	ElementListBox * elementListBox;
	SegObjectInspector * segObjectInspectorWidget;

	SegmentationDataWrapper currentSDW;
	bool haveValidSDW;

	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	OmSegIDsListPtr getSegmentsToDisplay( const OmId firstSegmentID );
	OmSegIDsListPtr doGetSegmentsToDisplay( const unsigned int offset );

	SegmentDataWrapper getCurrentlySelectedSegment();

	void setupDataElementList();
	QMenu * contextMenu;
	QAction * propAct;
	int getNumSegmentsPerPage();
	void dealWithButtons();
	bool isSegmentSelected();

	int currentPageNum;
	quint32 getMaxSegmentValue();
};

#endif
