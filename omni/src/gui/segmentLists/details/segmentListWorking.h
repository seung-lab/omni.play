#ifndef SEGMENT_LIST_WORKING_H
#define SEGMENT_LIST_WORKING_H

#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "segment/lists/omSegmentLists.h"

class SegmentListWorking : public SegmentListBase {
    Q_OBJECT

public:
    SegmentListWorking(QWidget * parent,
                       OmViewGroupState* vgs)
        : SegmentListBase(parent, vgs)
    {}

private:
    QString getTabTitle(){
        return QString("Working");
    }

    uint64_t Size() {
        return sdw_.SegmentLists()->Size(om::WORKING);
    }

    om::shared_ptr<GUIPageOfSegments>
    getPageSegments(const GUIPageRequest& request){
        return sdw_.SegmentLists()->GetSegmentGUIPage(om::WORKING, request);
    }

    int getPreferredTabIndex(){
        return 0;
    }

    void makeTabActiveIfContainsJumpedToSegment(){
        ElementListBox::SetActiveTab( this );
    }

    bool shouldSelectedSegmentsBeAddedToRecentList(){
        return true;
    }
};

#endif
