#pragma once

#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "common/common.h"
#include "utility/dataWrappers.h"
#include "segment/lists/omSegmentLists.h"

class SegmentListValid : public SegmentListBase
{
    Q_OBJECT

    public:
    SegmentListValid(QWidget * parent,
                     OmViewGroupState* vgs)
        : SegmentListBase(parent, vgs)
    {}

private:
    QString getTabTitle(){
        return QString("Valid");
    }

    uint64_t Size(){
        return sdw_.SegmentLists()->Size(om::common::SegListType::VALID);
    }

    std::shared_ptr<GUIPageOfSegments>
    getPageSegments(const GUIPageRequest& request)
    {
        return sdw_.SegmentLists()->GetSegmentGUIPage(om::common::SegListType::VALID, request);
    }

    int getPreferredTabIndex(){
        return 1;
    }

    void makeTabActiveIfContainsJumpedToSegment(){
        ElementListBox::SetActiveTab( this );
    }

    bool shouldSelectedSegmentsBeAddedToRecentList(){
        return true;
    }
};

