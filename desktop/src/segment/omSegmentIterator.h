#pragma once

#include "common/common.h"
#include "segment/omSegmentPointers.h"

class OmSegment;
class OmSegments;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmSegmentIterator {
 public:
  explicit OmSegmentIterator(OmSegments*);
  explicit OmSegmentIterator(const SegmentationDataWrapper& sdw);
  explicit OmSegmentIterator(const SegmentDataWrapper& sdw);

  void iterOverSegmentID(const om::common::SegID segID);
  void iterOverSegmentIDs(const om::common::SegIDSet& set);
  void iterOverSelectedIDs();
  void iterOverEnabledIDs();

  OmSegment* getNextSegment();
  bool empty();

  OmSegmentIterator& operator=(const OmSegmentIterator& other);

 private:
  OmSegments* segments_;
  std::deque<OmSegment*> segs_;
};
