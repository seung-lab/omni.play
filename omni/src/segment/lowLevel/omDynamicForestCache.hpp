#ifndef OM_DYNAMIC_FOREST_CACHE_HPP
#define OM_DYNAMIC_FOREST_CACHE_HPP

#include "common/omCommon.h"
#include "segment/lowLevel/DynamicForestPool.hpp"

class OmDynamicForestCache {
private:
	zi::DynamicForestPool<OmSegID> *const graph_;
	boost::unordered_map<OmSegID, OmSegID> cache_;

	bool batchMode_;

	inline void clearCache()
	{
		if(batchMode_){
			return;
		}

		cache_.clear();
	}

public:
	OmDynamicForestCache(const size_t size)
		: graph_(new zi::DynamicForestPool<OmSegID>(size))
		, batchMode_(false)
	{}

	~OmDynamicForestCache(){
		delete graph_;
	}

	void SetBatch(const bool batchModeOn){
		batchMode_ = batchModeOn;
	}

	void ClearCache(){
		cache_.clear();
	}

	void Cut(const OmSegID segID)
	{
		clearCache();
		graph_->cut(segID);
	}

	OmSegID Root(const OmSegID segID)
	{
		if(batchMode_){
			return graph_->root(segID);
		}

		if(!cache_.count(segID)){
			return cache_[segID] = graph_->root(segID);
		}
		return cache_[segID];
	}

	void Join(const OmSegID childRootID, const OmSegID parentRootID)
	{
		clearCache();
		graph_->join(childRootID, parentRootID);
	}

	size_t Size() const {
		return graph_->size();
	}

	void Resize(const size_t size){
		graph_->resize(size);
	}
};

#endif
