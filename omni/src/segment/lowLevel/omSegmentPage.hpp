#ifndef OM_SEGMENT_PAGE_HPP
#define OM_SEGMENT_PAGE_HPP

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "utility/omSmartPtr.hpp"

class OmSegmentPage {
public:
	OmSegmentPage()
		: pageSize_(0)
		, segmentsRawPtr_(NULL)
	{}

	OmSegmentPage(const uint32_t pageSize)
		: pageSize_(pageSize)
	{
		segments_ = OmSmartPtr<OmSegment>::MallocNumElements(pageSize_,
															 om::ZERO_FILL);
		segmentsRawPtr_ = segments_.get();
	}

	inline OmSegment& operator[](const uint32_t index){
		assert(pageSize_ && index < pageSize_);
		return segmentsRawPtr_[index];
	}

	inline const OmSegment& operator[](const uint32_t index) const {
		assert(pageSize_ && index < pageSize_);
		return segmentsRawPtr_[index];
	}

private:
	uint32_t pageSize_;
	boost::shared_ptr<OmSegment> segments_;
	OmSegment* segmentsRawPtr_;
};

#endif
