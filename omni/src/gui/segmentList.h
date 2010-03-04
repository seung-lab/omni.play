#ifndef SEGMENT_LIST_H
#define SEGMENT_LIST_H

#include <QtGui>
#include <QWidget> 

#include "inspectors/inspectorProperties.h"
#include "inspectors/segObjectInspector.h"
#include "common/omStd.h"
#include "system/omSystemTypes.h"
#include "utility/dataWrappers.h"
#include "system/events/omSegmentEvent.h"
#include "common/omDebug.h"

class SegmentList : public QWidget
{
	Q_OBJECT

public:
	SegmentList( QWidget * , InspectorProperties *, QTabWidget * );
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

private slots: 
	void leftClickOnSegment(QTreeWidgetItem * current, const int column);
	void addToSplitterDataElementSegment(QTreeWidgetItem * current, const int column);

private:
	static const int ENABLED_COL = 0;
	static const int NAME_COL = 1;
	static const int ID_COL = 2;
	static const int NOTE_COL = 3;
	static const int USER_DATA_COL = 4;

	QTreeWidget *dataElementsWidget;

	// keep local hash of segmentation --> segments to maintain 
	//  GUI state information about which segments are selected
	QHash < OmId, QHash < OmId, SegmentDataWrapper > >hashOfSementationsAndSegments;
	InspectorProperties * inspectorProperties;

	void sendSegmentChangeEvent(SegmentDataWrapper sdw, const bool augment_selection);

	SegmentationDataWrapper currentSDW;
	bool haveValidSDW;

	QTreeWidget * setupDataElementList();
	QTabWidget * dataElementsTabs;
	void setTabEnabled( QWidget * tab, QString title );
	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	SegObjectInspector *segObjectInspectorWidget;
};

#endif
