#ifndef OM_SEGMENT_LIST_WIDGET_H
#define OM_SEGMENT_LIST_WIDGET_H

#include <QtGui>
#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"
#include "gui/omSegmentContextMenu.h"
#include "volume/omSegmentation.h"

class SegmentDataWrapper;
class SegmentationDataWrapper;
class InspectorProperties;
class SegmentListBase;

class OmSegmentListWidget : public QTreeWidget {

 Q_OBJECT

 public:
	OmSegmentListWidget(SegmentListBase*, InspectorProperties *);

	bool populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
					       const OmId segmentJustSelectedID,
					       SegmentationDataWrapper seg,
					       OmSegPtrList * segs );
	static string eventSenderName();

 private:
	void mousePressEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);

	SegmentListBase * segmentListBase;
 	InspectorProperties * inspectorProperties;

	SegmentDataWrapper getCurrentlySelectedSegment();
	bool isSegmentSelected();
	void segmentRightClick(QMouseEvent* event);
	void segmentLeftClick();

	void addToSplitterDataElementSegment(SegmentDataWrapper sdw );
	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	void segmentShowContexMenu(QMouseEvent* event);

	static const int ENABLED_COL = 0;
	static const int NAME_COL = 1;
	static const int ID_COL = 2;
	static const int NOTE_COL = 3;
	static const int USER_DATA_COL = 4;

	OmSegmentContextMenu mSegmentContextMenu;
};

#endif
