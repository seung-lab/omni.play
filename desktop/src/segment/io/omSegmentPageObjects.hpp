#pragma once

#include "segment/omSegment.h"
#include "utility/omSmartPtr.hpp"
#include "utility/omTempFile.hpp"

class OmSegmentPageObjects {
private:
    const uint32_t pageSize_;

    boost::shared_ptr<OmSegment> segmentsPtr_;

    boost::shared_ptr<OmTempFile<OmSegment> > tmpFile_;

public:
    OmSegmentPageObjects(const uint32_t pageSize)
        : pageSize_(pageSize)
    {}

    OmSegment* MakeSegmentObjectPoolInMemory()
    {
        segmentsPtr_ = OmSmartPtr<OmSegment>::NewNumElements(pageSize_);
        return segmentsPtr_.get();
    }

    OmSegment* MakeSegmentObjectPoolOnDisk()
    {
        tmpFile_.reset(new OmTempFile<OmSegment>());

        tmpFile_->ResizeNumElements(pageSize_);

        return tmpFile_->Map();
    }
};

