#ifndef OM_SEGMENT_PAGE_HPP
#define OM_SEGMENT_PAGE_HPP

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "utility/omSmartPtr.hpp"

class OmSegmentPage {
public:
	OmSegmentPage()
		: pageSize_(0)
	{}

	OmSegmentPage(const uint32_t pageSize)
		: pageSize_(pageSize)
	{
//		segments_ = OmSmartPtr<OmSegmentPage>::
//			makeMallocPtrNumElements(pageSize_,
//									 om::ZERO_FILL);
		segments_.resize(pageSize_);
	}

	inline OmSegment& operator[](const uint32_t index){
		return segments_[index];
	}

	inline const OmSegment& operator[](const uint32_t index) const {
		return segments_[index];
	}

private:
	int pageSize_;
//	boost::shared_ptr<OmSegment> segments_;
	std::vector<OmSegment> segments_;
};

#endif
