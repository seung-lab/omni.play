#ifndef OM_SEGMENT_BAG_HPP
#define OM_SEGMENT_BAG_HPP

#include "common/omCommon.h"

class OmSegmentBag {
private:
	struct SegAndID{
		OmSegID segID;
		OmSegment* seg;
	};

	SegAndID root_;
	std::vector<SegAndID> segs_;

	int64_t totalVoxels_;

	zi::mutex lock_;

public:
	OmSegmentBag(const OmSegID segID)
		: totalVoxels_(0)
	{
		root_ = {0,0};
	}

	zi::mutex& Mutex(){
		return lock_;
	}

	int64_t TotalVoxels() const {
		return totalVoxels_;
	}

};

#endif
