#pragma once

#include "common/common.h"
#include "segment/omSegmentPointers.h"
#include "gui/widgets/omSegmentContextMenu.h"
#include "volume/omSegmentation.h"

#include <QtGui>

class GUIPageOfSegments;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class SegmentListBase;
class GUIPageOfSegment;

class OmSegmentListWidget : public QTreeWidget {
Q_OBJECT

public:
    OmSegmentListWidget(SegmentListBase*, OmViewGroupState*);

    bool populate(const bool doScrollToSelectedSegment,
                  const SegmentDataWrapper segmentJustSelected,
                  boost::shared_ptr<GUIPageOfSegments>);

    static std::string eventSenderName();

private:
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

    SegmentListBase* segmentListBase;
    OmViewGroupState *const vgs_;

    SegmentDataWrapper getCurrentlySelectedSegment();
    bool isSegmentSelected();
    void segmentRightClick(QMouseEvent* event);
    void segmentLeftClick();

    void setRowFlags(QTreeWidgetItem* row);

    void segmentShowContexMenu(QMouseEvent* event);

    static const int ID_COL = 0;
    static const int NUM_PIECES_COL = 1;
    static const int SIZE_COL = 2;
    static const int USER_DATA_COL = 3;

    OmSegmentContextMenu mSegmentContextMenu;

    void centerSegment(const SegmentationDataWrapper&);
};

