#pragma once

#include "segment/segment.h"
#include "utility/smartPtr.hpp"

class segmentPageObjects {
private:
    const uint32_t pageSize_;

    boost::shared_ptr<segment> segmentsPtr_;

public:
    segmentPageObjects(const uint32_t pageSize)
        : pageSize_(pageSize)
    {}

    segment* MakeSegmentObjectPoolInMemory()
    {
        segmentsPtr_ = utility::smartPtr<segment>::NewNumElements(pageSize_);
        return segmentsPtr_.get();
    }
};

