#pragma once

#include "segment/segment.h"
#include "system/cache/omCacheObjects.hpp"
#include "segment/lists/segmentListsTypes.hpp"

class segmentLists;

class segmentListByMRU {
private:
    segmentLists *const segmentLists_;

    KeyMultiIndex<segment*> lru_;
    std::vector<SegInfo> vec_;
    bool dirty_;

    zi::mutex lock_;

public:
    segmentListByMRU(segmentLists* segmentLists)
        : segmentLists_(segmentLists)
        , dirty_(true)
    {}

    void Touch(segment* seg)
    {
        zi::guard g(lock_);
        lru_.touch(seg);
        dirty_ = true;
    }

    boost::shared_ptr<GUIPageOfSegments>
    GetSegmentGUIPage(const GUIPageRequest& request)
    {
        zi::guard g(lock_);

        if(dirty_){
            rebuildList();
        }

        return om::segLists::getPage(vec_, request.offset, request.numToGet);
    }

    size_t Size()
    {
        zi::guard g(lock_);
        return lru_.Size();
    }

private:
    void rebuildList();
};

