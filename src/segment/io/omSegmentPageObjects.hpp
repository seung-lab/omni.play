#pragma once

#include "segment/segment.h"
#include "utility/omSmartPtr.hpp"
#include "utility/omTempFile.hpp"

class segmentPageObjects {
private:
    const uint32_t pageSize_;

    om::shared_ptr<segment> segmentsPtr_;

    om::shared_ptr<OmTempFile<segment> > tmpFile_;

public:
    segmentPageObjects(const uint32_t pageSize)
        : pageSize_(pageSize)
    {}

    segment* MakeSegmentObjectPoolInMemory()
    {
        segmentsPtr_ = OmSmartPtr<segment>::NewNumElements(pageSize_);
        return segmentsPtr_.get();
    }

    segment* MakeSegmentObjectPoolOnDisk()
    {
        tmpFile_.reset(new OmTempFile<segment>());

        tmpFile_->ResizeNumElements(pageSize_);

        return tmpFile_->Map();
    }
};

