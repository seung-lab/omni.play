#pragma once

#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/io/omSegmentPage.hpp"

class OmCacheSegStore {
private:
    OmSegmentsStore *const store_;
    const uint32_t pageSize_;

    const std::vector<OmSegmentPage*> initialPages_;
    const uint32_t numInitialPages_;

    std::vector<OmSegmentPage*> initialAndAddedPages_;

    zi::mutex initialAndAddedPagesLock_;

public:
    OmCacheSegStore(OmSegmentsStore* store)
        : store_(store)
        , pageSize_(store->PageSize())
        , initialPages_(store->Pages())
        , numInitialPages_(initialPages_.size())
    {}

    /**
     * returns NULL if segment was never instantiated;
     **/
    OmSegment* GetSegment(const OmSegID segID){
        return getSegment(segID, om::SAFE);
    }

    OmSegment* GetSegmentUnsafe(const OmSegID segID){
        return getSegment(segID, om::NOT_SAFE);
    }

private:
    inline OmSegment* getSegment(const OmSegID segID, const om::Safe isSafe)
    {
        const uint32_t pageNum = segID / pageSize_;

        if(pageNum < numInitialPages_){
            // no locking needed--initialPages_ will never change
            return doGetSegment(initialPages_, pageNum, segID, isSafe);

        } else {
            zi::guard g(initialAndAddedPagesLock_);

            if(pageNum >= initialAndAddedPages_.size())
            {
                if(store_->NumPages() != initialAndAddedPages_.size())
                {
                    initialAndAddedPages_ = store_->Pages();
                }
            }

            return doGetSegment(initialAndAddedPages_, pageNum, segID, isSafe);
        }
    }

    inline OmSegment* doGetSegment(const std::vector<OmSegmentPage*>& pages,
                                   const uint32_t pageNum, const OmSegID segID,
                                   const om::Safe isSafe)
    {
        if(om::NOT_SAFE == isSafe)
        {
            OmSegmentPage& page = *pages[pageNum];
            return &(page[segID % pageSize_]);
        }

        if(pageNum >= pages.size()){
            return NULL;
        }

        if(!pages[pageNum]){
            return NULL;
        }

        OmSegmentPage& page = *pages[pageNum];
        OmSegment* ret = &(page[segID % pageSize_]);

        const OmSegID loadedID = ret->data_->value;

        if(!loadedID){
            return NULL;
        }

        if(loadedID != segID){
            throw OmIoException("corruption detected in segment page");
        }

        return ret;
    }
};

