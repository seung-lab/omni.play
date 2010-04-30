#ifndef SEGMENT_LIST_H
#define SEGMENT_LIST_H

#include <QtGui>
#include <QWidget> 

#include "inspectors/inspectorProperties.h"
#include "inspectors/segObjectInspector.h"
#include "common/omStd.h"
#include "utility/dataWrappers.h"
#include "system/events/omSegmentEvent.h"
#include "common/omDebug.h"
#include "gui/elementListBox.h"

class SegmentList : public QWidget
{
	Q_OBJECT

public:
	SegmentList( QWidget * , InspectorProperties *, ElementListBox * );
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

private slots: 
	void leftClickOnSegment(QTreeWidgetItem * current, const int column);
	void addToSplitterDataElementSegment(SegmentDataWrapper sdw );
	void showContextMenu(const QPoint & menuPoint);
	void segmentRightClickMenu(QAction * act);

protected:
	void keyPressEvent (QKeyEvent *event);

private:
	static const int ENABLED_COL = 0;
	static const int NAME_COL = 1;
	static const int ID_COL = 2;
	static const int NOTE_COL = 3;
	static const int USER_DATA_COL = 4;

	QTreeWidget * dataElementsWidget;
 	InspectorProperties * inspectorProperties;
	ElementListBox * elementListBox;
	SegObjectInspector * segObjectInspectorWidget;

	SegmentationDataWrapper currentSDW;
	bool haveValidSDW;

	void sendSegmentChangeEvent(SegmentDataWrapper sdw, const bool augment_selection);

	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	QList< SEGMENT_DATA_TYPE > * getSegmentsToDisplay( const OmId firstSegmentID );
	QList< SEGMENT_DATA_TYPE > * doGetSegmentsToDisplay( const unsigned int offset );

	SegmentDataWrapper getCurrentlySelectedSegment();
	QMenu * makeSegmentContextMenu(QTreeWidget * parent);
	void setupDataElementList();
	QMenu * contextMenu;
	QAction * propAct;
	void showSegmentContextMenu();
	int getNumSegmentsPerPage();
	void dealWithButtons();
	bool isSegmentSelected();

	int mNumSegmentsPerPage;
	int currentPageNum;
	unsigned int mNumSegments;
};

#endif
