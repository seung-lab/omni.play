#ifndef OM_SEGMENT_SEARCHED_H
#define OM_SEGMENT_SEARCHED_H

#include "actions/omActions.h"
#include "common/omCommon.h"
#include "system/cache/omCacheManager.h"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"
#include "zi/omUtility.h"

class OmSegmentSearched : private om::singletonBase<OmSegmentSearched> {
public:
	static void Delete(){
		instance().sdw_ = SegmentDataWrapper();
	}

	static void Set(const SegmentDataWrapper& sdw)
	{
		instance().sdw_ = sdw;
	}

	static SegmentDataWrapper Get() {
		return instance().sdw_;
	}

private:
	OmSegmentSearched(){}
	~OmSegmentSearched(){}

	SegmentDataWrapper sdw_;

	friend class zi::singleton<OmSegmentSearched>;
};

#endif
