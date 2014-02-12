#include "segment/omSegment.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegments.h"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "zi/utility.h"
#include "segment/selection.hpp"
#include "segment/lowLevel/children.hpp"

OmSegmentIterator::OmSegmentIterator(const OmSegments& cache)
    : segments_(&cache) {}

OmSegmentIterator::OmSegmentIterator(const SegmentationDataWrapper& sdw)
    : segments_(sdw.Segments()) {}

OmSegmentIterator::OmSegmentIterator(const SegmentDataWrapper& sdw)
    : segments_(sdw.Segments()) {}

void OmSegmentIterator::iterOverSegmentID(const om::common::SegID segID) {
  if (segments_) {
    segs_.push_back(segments_->GetSegment(segID));
  }
}

void OmSegmentIterator::iterOverSelectedIDs() {
  if (segments_) {
    for (auto id : segments_->Selection().GetSelectedSegmentIDs()) {
      segs_.push_back(segments_->GetSegment(id));
    }
  }
}

void OmSegmentIterator::iterOverSegmentIDs(const om::common::SegIDSet& set) {
  if (segments_) {
    for (auto id : set) {
      segs_.push_back(segments_->GetSegment(id));
    }
  }
}

bool OmSegmentIterator::empty() { return segs_.empty(); }

OmSegment* OmSegmentIterator::getNextSegment() {
  if (segs_.empty()) {
    return nullptr;
  }

  OmSegment* segRet = segs_.back();
  segs_.pop_back();

  if (segments_) {
    for (OmSegment* seg : segments_->Children().GetChildren(segRet->value())) {
      segs_.push_back(seg);
    }
  }

  return segRet;
}

OmSegmentIterator& OmSegmentIterator::operator=(
    const OmSegmentIterator& other) {
  if (this == &other) {
    return *this;
  }

  segments_ = other.segments_;
  segs_ = other.segs_;

  return *this;
}
