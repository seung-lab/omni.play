#pragma once

#include "segment/lists/segmentListsTypes.hpp"
#include "zi/omMutex.h"

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

    om::shared_ptr<segmentListGlobal> globalList_;
    om::shared_ptr<segmentListForGUI> working_;
    om::shared_ptr<segmentListForGUI> uncertain_;
    om::shared_ptr<segmentListForGUI> valid_;

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
            throw OmArgException("unknown type");
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

    void Swap(om::shared_ptr<segmentListForGUI>& list);
    void Swap(om::shared_ptr<segmentListGlobal>& globalList);

    size_t Size(const om::SegListType type);
    size_t SizeRecent();

    uint64_t GetNumTopLevelSegs();
    int64_t TotalNumVoxels();
    int64_t NumVoxels(const om::SegListType type);

    om::shared_ptr<GUIPageOfSegments>
    GetSegmentGUIPageRecent(const GUIPageRequest& request);

    om::shared_ptr<GUIPageOfSegments>
    GetSegmentGUIPage(const om::SegListType type, const GUIPageRequest& request);

    segId GetNextSegIDinWorkingList(const SegmentDataWrapper&);
    segId GetNextSegIDinWorkingList(const SegmentationDataWrapper&);

    int64_t GetSizeWithChildren(const segId segID);
    int64_t GetSizeWithChildren(segment* seg);

    int64_t GetNumChildren(const segId segID);
    int64_t GetNumChildren(segment* seg);
};

