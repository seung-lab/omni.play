#pragma once

#include "common/common.h"
#include "segment/lists/omSegmentListsTypes.hpp"
#include "zi/omUtility.h"

class OmSegmentListForGUI {
private:
    const om::common::SegListType listType_;

    std::vector<SegInfo> bySize_;
    boost::unordered_map<om::common::SegID, uint32_t> index_;

    int64_t totalNumVoxels_;

public:
    OmSegmentListForGUI(const om::common::SegListType listType)
        : listType_(listType)
        , totalNumVoxels_(0)
    {}

    inline om::common::SegListType Type() const {
        return listType_;
    }

    void Build(const std::vector<SegInfo>& globalList)
    {
        bySize_.clear();
        bySize_.reserve(globalList.size());

        index_.clear();

        totalNumVoxels_ = 0;

        makeSizeList(globalList);
        makeIndex();
    }

    int64_t Size() const {
        return bySize_.size();
    }

    int64_t TotalNumVoxels() const {
        return totalNumVoxels_;
    }

    om::shared_ptr<GUIPageOfSegments> GetSegmentGUIPage(const GUIPageRequest& request)
    {
        if(!request.startSeg) {
            return om::segLists::getPage(bySize_, request.offset, request.numToGet);
        }

        return getPageContainingSegment(request.startSeg, request.numToGet);
    }

    boost::optional<SegInfo> GetNextSegID(const om::common::SegID segID)
    {
        if(!index_.count(segID)){
            return boost::optional<SegInfo>();
        }

        const om::common::SegID segIndex = index_[segID];
        const om::common::SegID nextSegIndex = segIndex + 1;

        if(bySize_.size() == nextSegIndex){
            return boost::optional<SegInfo>();
        }

        return bySize_[nextSegIndex];
    }

    boost::optional<SegInfo> Get(const om::common::SegID segID)
    {
        if(!index_.count(segID)){
            return boost::optional<SegInfo>();
        }
        const om::common::SegID segIndex = index_[segID];
        return boost::optional<SegInfo>(bySize_[segIndex]);
    }

private:
    om::shared_ptr<GUIPageOfSegments>
    getPageContainingSegment(const om::common::SegID segID, const int numToGet)
    {
        if(!index_.count(segID)){
            return om::segLists::getPage(bySize_, 0, numToGet);
        }

        const int indexIntoSizeList = index_[segID];
        const uint32_t roundedOffset = om::math::roundDown(indexIntoSizeList, numToGet);

        return om::segLists::getPage(bySize_, roundedOffset, numToGet);
    }

    void makeIndex()
    {
        for(uint32_t i = 0; i < bySize_.size(); ++i)
        {
            index_[bySize_[i].seg->value()] = i;
        }
    }

    void makeSizeList(const std::vector<SegInfo>& globalList)
    {
        FOR_EACH(iter, globalList){
            processSegInfo(*iter);
        }

        zi::sort(bySize_.begin(), bySize_.end(), om::segLists::CmpSegInfo);
    }

    void processSegInfo(const SegInfo& segInfo)
    {
        if(!segInfo.sizeIncludingChildren){
            return;
        }

        if(segInfo.seg->GetListType() != listType_){
            return;
        }

        bySize_.push_back(segInfo);
        totalNumVoxels_ += segInfo.sizeIncludingChildren;
    }
};

