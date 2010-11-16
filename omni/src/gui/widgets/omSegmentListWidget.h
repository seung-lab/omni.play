#ifndef OM_SEGMENT_LIST_WIDGET_H
#define OM_SEGMENT_LIST_WIDGET_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"
#include "gui/widgets/omSegmentContextMenu.h"
#include "volume/omSegmentation.h"

#include <QtGui>

class SegmentDataWrapper;
class SegmentationDataWrapper;
class SegmentListBase;
class OmSegIDsListWithPage;

class OmSegmentListWidget : public QTreeWidget {
 Q_OBJECT
public:
	OmSegmentListWidget(SegmentListBase*, OmViewGroupState*);

	bool populate(const bool doScrollToSelectedSegment,
				  const OmID segmentJustSelectedID,
				  SegmentationDataWrapper seg,
				  boost::shared_ptr<OmSegIDsListWithPage>);

	static std::string eventSenderName();

 private:
	void mousePressEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);

						SegmentListBase * segmentListBase;
						OmViewGroupState *const vgs_;

	SegmentDataWrapper getCurrentlySelectedSegment();
	bool isSegmentSelected();
	void segmentRightClick(QMouseEvent* event);
	void segmentLeftClick();

	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	void segmentShowContexMenu(QMouseEvent* event);

	static const int ENABLED_COL = 0;
	static const int NAME_COL = 1;
	static const int ID_COL = 2;
	static const int NOTE_COL = 3;
	static const int USER_DATA_COL = 4;

	OmSegmentContextMenu mSegmentContextMenu;

	void centerSegment(const SegmentationDataWrapper&);
};

#endif
