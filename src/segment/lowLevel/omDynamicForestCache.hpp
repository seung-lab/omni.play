#pragma once

#include "common/common.h"
#include "segment/lowLevel/DynamicForestPool.hpp"
#include "utility/omLockedPODs.hpp"

class OmDynamicForestCache {
private:
    boost::scoped_ptr<zi::DynamicForestPool<segId> > graph_;

    LockedUint64 freshness_;

    bool batchMode_;

    inline void clearCacheIfNotBatch()
    {
        if(batchMode_){
            return;
        }

        ClearCache();
    }

public:
    OmDynamicForestCache(const size_t size)
        : graph_(new zi::DynamicForestPool<segId>(size))
        , batchMode_(false)
    {
        freshness_.set(1);
    }

    ~OmDynamicForestCache()
    {}

    inline uint64_t Freshness() const {
        return freshness_.get();
    }

    inline void SetBatch(const bool batchModeOn)
    {
        ClearCache();
        batchMode_ = batchModeOn;
    }

    inline void ClearCache(){
        ++freshness_;
    }

    inline void Cut(const segId segID)
    {
        clearCacheIfNotBatch();
        graph_->Cut(segID);
    }

    inline segId Root(const segId segID){
        return graph_->Root(segID);
    }

    inline void Join(const segId childRootID, const segId parentRootID)
    {
        clearCacheIfNotBatch();
        graph_->Join(childRootID, parentRootID);
    }

    inline size_t Size() const {
        return graph_->Size();
    }

    inline void Resize(const size_t size){
        graph_->Resize(size);
    }
};

