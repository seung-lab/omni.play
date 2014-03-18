#pragma once
#include "precomp.h"

#include "common/common.h"
#include "mesh/drawer/omSegmentPointers.h"

class OmSegment;
class OmSegments;
class SegmentDataWrapper;
class SegmentationDataWrapper;
namespace om {
namespace segment {
class Selection;
}
}

class OmSegmentIterator {
 public:
  explicit OmSegmentIterator(const OmSegments&);
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
  const OmSegments* segments_;
  std::deque<OmSegment*> segs_;
};
