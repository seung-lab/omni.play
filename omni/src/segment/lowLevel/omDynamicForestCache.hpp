#ifndef OM_DYNAMIC_FOREST_CACHE_HPP
#define OM_DYNAMIC_FOREST_CACHE_HPP

#include "common/omCommon.h"
#include "segment/lowLevel/DynamicForestPool.hpp"

class OmDynamicForestCache {
private:
	boost::scoped_ptr<zi::DynamicForestPool<OmSegID> > graph_;
	std::vector<OmSegID> cache_;

	bool batchMode_;

	inline void zeroFillCache(){
		std::fill(cache_.begin(), cache_.end(), 0);
	}

	inline void clearCacheIfNotBatch()
	{
		if(batchMode_){
			return;
		}

		zeroFillCache();
	}

public:
	OmDynamicForestCache(const size_t size)
		: graph_(new zi::DynamicForestPool<OmSegID>(size))
		, batchMode_(false)
	{
		cache_.resize(size, 0);
	}

	~OmDynamicForestCache()
	{}

	inline void SetBatch(const bool batchModeOn){
		batchMode_ = batchModeOn;
	}

	inline void ClearCache(){
		zeroFillCache();
	}

	inline void Cut(const OmSegID segID)
	{
		clearCacheIfNotBatch();
		graph_->cut(segID);
	}

	inline OmSegID Root(const OmSegID segID)
	{
		if(batchMode_){
			return graph_->root(segID);
		}

		if(!cache_[segID]){
			return cache_[segID] = graph_->root(segID);
		}
		return cache_[segID];
	}

	inline void Join(const OmSegID childRootID, const OmSegID parentRootID)
	{
		clearCacheIfNotBatch();
		graph_->join(childRootID, parentRootID);
	}

	inline size_t Size() const {
		return graph_->size();
	}

	inline void Resize(const size_t size)
	{
		graph_->resize(size);
		cache_.resize(size, 0);
	}
};

#endif
