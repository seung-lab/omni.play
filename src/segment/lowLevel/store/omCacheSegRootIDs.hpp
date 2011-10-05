#pragma once

#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/segments.h"
#include "volume/segmentation.h"

class OmCacheSegRootIDs {
private:
    segmentation *const segmentation_;
    segments *const segments_;
    segmentsStore *const segmentPages_;

    const uint32_t numInitialPages_;
    const int pageSize_;

    struct RootAndMSTFreshnes {
        uint64_t freshness;
        uint32_t segID;
    };

    typedef std::vector<RootAndMSTFreshnes> page_t;

    std::vector<page_t*> pages_;
    struct cache_root_segment_id_initial_tag;

public:
    OmCacheSegRootIDs(segmentation* segmentation,
                      segmentsStore* segmentPages)
        : segmentation_(segmentation)
        , segments_(segmentation->Segments())
        , segmentPages_(segmentPages)
        , numInitialPages_(segmentPages->NumPages())
        , pageSize_(segmentPages->PageSize())
    {
        pages_.resize(numInitialPages_);

        for(uint32_t i = 0; i < numInitialPages_; ++i){
            pages_[i] = new page_t(pageSize_);
        }
    }

    ~OmCacheSegRootIDs()
    {
        for(uint32_t i = 0; i < numInitialPages_; ++i){
            delete pages_[i];
        }
    }

    segId Root(const segId segID)
    {
        const uint32_t pageNum = segID / pageSize_;

        if(pageNum >= numInitialPages_){ // not caching those pages, yet
            return segments_->findRootIDnoCache(segID);
        }

        const uint64_t mstFreshness = segments_->MSTfreshness();
        page_t& page = *pages_[pageNum];
        const uint32_t offset = segID % pageSize_;

        {
            zi::spinlock::pool<cache_root_segment_id_initial_tag>::guard g(segID);
            if(page[offset].freshness == mstFreshness){
                return page[offset].segID;
            }
        }

        const segId rootSegID = segments_->findRootIDnoCache(segID);

        {
            zi::spinlock::pool<cache_root_segment_id_initial_tag>::guard g(segID);
            page[offset].freshness = mstFreshness;
            page[offset].segID = rootSegID;
        }

        return rootSegID;
    }
};

