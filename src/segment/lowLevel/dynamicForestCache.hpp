#pragma once

#include "common/common.h"
#include "segment/lowLevel/dynamicForestPool.hpp"
#include "utility/omLockedPODs.hpp"

class dynamicForestCache {
private:
    boost::scoped_ptr<zi::dynamicForestPool<common::segId> > graph_;

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
    dynamicForestCache(const size_t size)
        : graph_(new zi::dynamicForestPool<common::segId>(size))
        , batchMode_(false)
    {
        freshness_.set(1);
    }

    ~dynamicForestCache()
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

    inline void Cut(const common::segId segID)
    {
        clearCacheIfNotBatch();
        graph_->Cut(segID);
    }

    inline common::segId Root(const common::segId segID){
        return graph_->Root(segID);
    }

    inline void Join(const common::segId childRootID, const common::segId parentRootID)
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

