#pragma once

#include "segment/omSegment.h"
#include "utility/malloc.hpp"
#include "utility/omTempFile.hpp"

class OmSegmentPageObjects {
 private:
  const uint32_t pageSize_;

  std::shared_ptr<OmSegment> segmentsPtr_;

  std::shared_ptr<OmTempFile<OmSegment> > tmpFile_;

 public:
  OmSegmentPageObjects(const uint32_t pageSize) : pageSize_(pageSize) {}

  OmSegment* MakeSegmentObjectPoolInMemory() {
    segmentsPtr_ = om::mem::Malloc<OmSegment>::NewNumElements(pageSize_);
    return segmentsPtr_.get();
  }

  OmSegment* MakeSegmentObjectPoolOnDisk() {
    tmpFile_.reset(new OmTempFile<OmSegment>());

    tmpFile_->ResizeNumElements(pageSize_);

    return tmpFile_->Map();
  }
};
