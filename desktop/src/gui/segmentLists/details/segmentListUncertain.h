#pragma once

#include "common/common.h"
#include "gui/segmentLists/details/segmentListBase.h"
#include "segment/lists/omSegmentLists.h"
#include "utility/dataWrappers.h"

class SegmentListUncertain : public SegmentListBase {
    Q_OBJECT

public:
    SegmentListUncertain(QWidget * parent,
                         OmViewGroupState* vgs)
        : SegmentListBase(parent, vgs)
    {}

private:
    QString getTabTitle(){
        return QString("Uncertain");
    }

    uint64_t Size() {
        return sdw_.SegmentLists()->Size(om::common::SegListType::UNCERTAIN);
    }

    std::shared_ptr<GUIPageOfSegments>
    getPageSegments(const GUIPageRequest& request)
    {
        return sdw_.SegmentLists()->GetSegmentGUIPage(om::common::SegListType::UNCERTAIN, request);
    }

    int getPreferredTabIndex(){
        return 2;
    }

    void makeTabActiveIfContainsJumpedToSegment(){
        // don't jump to this tab
    }

    bool shouldSelectedSegmentsBeAddedToRecentList(){
        return true;
    }
};

