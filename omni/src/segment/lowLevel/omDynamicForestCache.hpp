#ifndef OM_DYNAMIC_FOREST_CACHE_HPP
#define OM_DYNAMIC_FOREST_CACHE_HPP

#include "common/omCommon.h"
#include "segment/lowLevel/DynamicForestPool.hpp"
#include "utility/omLockedPODs.hpp"

class OmDynamicForestCache {
private:
    boost::scoped_ptr<zi::DynamicForestPool<OmSegID> > graph_;

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
        : graph_(new zi::DynamicForestPool<OmSegID>(size))
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

    inline void Cut(const OmSegID segID)
    {
        clearCacheIfNotBatch();
        graph_->cut(segID);
    }

    inline OmSegID Root(const OmSegID segID){
        return graph_->root(segID);
    }

    inline void Join(const OmSegID childRootID, const OmSegID parentRootID)
    {
        clearCacheIfNotBatch();
        graph_->join(childRootID, parentRootID);
    }

    inline size_t Size() const {
        return graph_->size();
    }

    inline void Resize(const size_t size){
        graph_->resize(size);
    }
};

#endif
