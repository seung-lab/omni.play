#ifndef OM_SEGMENT_LISTS_HPP
#define OM_SEGMENT_LISTS_HPP

#include "segment/lists/omSegmentListsTypes.hpp"
#include "zi/omMutex.h"

class OmSegmentListLowLevel;
class OmSegmentListGlobal;
class OmSegmentListForGUI;
class OmSegmentListByMRU;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegment;

class OmSegmentLists {
private:
    zi::rwmutex lock_;

    boost::scoped_ptr<OmSegmentListLowLevel> segmentListsLL_;

    boost::shared_ptr<OmSegmentListGlobal> globalList_;
    boost::shared_ptr<OmSegmentListForGUI> working_;
    boost::shared_ptr<OmSegmentListForGUI> uncertain_;
    boost::shared_ptr<OmSegmentListForGUI> valid_;

    boost::scoped_ptr<OmSegmentListByMRU> recent_;

    inline OmSegmentListForGUI* get(const om::SegListType type)
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
    OmSegmentLists();
    ~OmSegmentLists();

    OmSegmentListLowLevel* LowLevelList() {
        return segmentListsLL_.get();
    }

    void RefreshGUIlists();

    void TouchRecent(OmSegment* seg);

    void Swap(boost::shared_ptr<OmSegmentListForGUI>& list);
    void Swap(boost::shared_ptr<OmSegmentListGlobal>& globalList);

    size_t Size(const om::SegListType type);
    size_t SizeRecent();

    uint64_t GetNumTopLevelSegs();
    int64_t TotalNumVoxels();
    int64_t NumVoxels(const om::SegListType type);

    boost::shared_ptr<GUIPageOfSegments>
    GetSegmentGUIPageRecent(const GUIPageRequest& request);

    boost::shared_ptr<GUIPageOfSegments>
    GetSegmentGUIPage(const om::SegListType type, const GUIPageRequest& request);

    OmSegID GetNextSegIDinWorkingList(const SegmentDataWrapper&);
    OmSegID GetNextSegIDinWorkingList(const SegmentationDataWrapper&);

    int64_t GetSizeWithChildren(const OmSegID segID);
    int64_t GetSizeWithChildren(OmSegment* seg);

    int64_t GetNumChildren(const OmSegID segID);
    int64_t GetNumChildren(OmSegment* seg);
};

#endif
