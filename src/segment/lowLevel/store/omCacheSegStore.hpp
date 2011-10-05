#pragma once

#include "segment/lowLevel/store/segmentStore.hpp"
#include "segment/io/segmentPage.hpp"

class OmCacheSegStore {
private:
    segmentsStore *const store_;
    const uint32_t pageSize_;

    const std::vector<segmentPage*> initialPages_;
    const uint32_t numInitialPages_;

public:
    OmCacheSegStore(segmentsStore* store)
        : store_(store)
        , pageSize_(store->PageSize())
        , initialPages_(store->Pages())
        , numInitialPages_(initialPages_.size())
    {}

    /**
     * returns NULL if segment was never instantiated;
     **/
    segment* GetSegment(const segId segID){
        return getSegment(segID, om::SAFE);
    }

    segment* GetSegmentUnsafe(const segId segID){
        return getSegment(segID, om::NOT_SAFE);
    }

private:
    inline segment* getSegment(const segId segID, const om::Safe isSafe)
    {
        const uint32_t pageNum = segID / pageSize_;

        if(pageNum < numInitialPages_)
        {
            // no locking needed--initialPages_ will never change
            return doGetSegment(initialPages_, pageNum, segID, isSafe);

        } else {

            // TODO: keep a copy of this vector as a member variable; some freshness value,
            //   though, could need to be kept by segmentStore to avoid repeated copy;
            //   comparing vector sizes is NOT sufficient, as segments can be added in any order,
            //   thus there may be NULL pages in the copy of the vector that don't get updated
            //   when initialized in the sgemtn store
            const std::vector<segmentPage*> curPages = store_->Pages();

            return doGetSegment(curPages, pageNum, segID, isSafe);
        }
    }

    inline segment* doGetSegment(const std::vector<segmentPage*>& pages,
                                   const uint32_t pageNum, const segId segID,
                                   const om::Safe isSafe)
    {
        if(om::NOT_SAFE == isSafe)
        {
            segmentPage& page = *pages[pageNum];
            return &(page[segID % pageSize_]);
        }

        if(pageNum >= pages.size()){
            return NULL;
        }

        if(!pages[pageNum]){
            return NULL;
        }

        segmentPage& page = *pages[pageNum];
        segment* ret = &(page[segID % pageSize_]);

        const segId loadedID = ret->data_->value;

        if(!loadedID){
            return NULL;
        }

        if(loadedID != segID){
            throw OmIoException("corruption detected in segment page");
        }

        return ret;
    }
};

