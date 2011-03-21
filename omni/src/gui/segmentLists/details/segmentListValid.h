#ifndef SEGMENT_LIST_VALID_H
#define SEGMENT_LIST_VALID_H

#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "common/omCommon.h"
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
        return sdw_.SegmentLists()->Size(om::VALID);
    }

    boost::shared_ptr<GUIPageOfSegments>
    getPageSegments(const GUIPageRequest& request)
    {
        return sdw_.SegmentLists()->GetSegmentGUIPage(om::VALID, request);
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

#endif
