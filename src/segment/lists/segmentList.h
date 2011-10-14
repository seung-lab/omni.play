#pragma once

#include "segment/lists/segmentListsTypes.hpp"
#include "zi/mutex.h"

class segmentListLowLevel;
class segmentListGlobal;
class segmentListForGUI;
class segmentListByMRU;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class segment;

class segmentLists {
private:
    zi::rwmutex lock_;

    boost::scoped_ptr<segmentListLowLevel> segmentListsLL_;

    boost::shared_ptr<segmentListGlobal> globalList_;
    boost::shared_ptr<segmentListForGUI> working_;
    boost::shared_ptr<segmentListForGUI> uncertain_;
    boost::shared_ptr<segmentListForGUI> valid_;

    boost::scoped_ptr<segmentListByMRU> recent_;

    inline segmentListForGUI* get(const om::SegListType type)
    {
        switch(type){
        case om::VALID:
            return valid_.get();
        case om::WORKING:
            return working_.get();
        case om::UNCERTAIN:
            return uncertain_.get();
        default:
            throw common::argException("unknown type");
        }
    }

public:
    segmentLists();
    ~segmentLists();

    segmentListLowLevel* LowLevelList() {
        return segmentListsLL_.get();
    }

    void RefreshGUIlists();

    void TouchRecent(segment* seg);

    void Swap(boost::shared_ptr<segmentListForGUI>& list);
    void Swap(boost::shared_ptr<segmentListGlobal>& globalList);

    size_t Size(const om::SegListType type);
    size_t SizeRecent();

    uint64_t GetNumTopLevelSegs();
    int64_t TotalNumVoxels();
    int64_t NumVoxels(const om::SegListType type);

    boost::shared_ptr<GUIPageOfSegments>
    GetSegmentGUIPageRecent(const GUIPageRequest& request);

    boost::shared_ptr<GUIPageOfSegments>
    GetSegmentGUIPage(const om::SegListType type, const GUIPageRequest& request);

    segId GetNextSegIDinWorkingList(const SegmentDataWrapper&);
    segId GetNextSegIDinWorkingList(const SegmentationDataWrapper&);

    int64_t GetSizeWithChildren(const segId segID);
    int64_t GetSizeWithChildren(segment* seg);

    int64_t GetNumChildren(const segId segID);
    int64_t GetNumChildren(segment* seg);
};

