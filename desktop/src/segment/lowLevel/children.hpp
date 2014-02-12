#pragma once

#include "common/common.h"
#include "segment/omSegment.h"
#include "segment/lowLevel/omSegmentLowLevelTypes.h"

namespace om {
namespace segment {

class Children {

  typedef std::set<OmSegment*> segChildCont_t;

 private:
  std::vector<segChildCont_t> list_;

 public:
  Children(const size_t size) { list_.resize(size); }

  void Resize(const size_t size) { list_.resize(size); }

  // Child segments not including itself
  const segChildCont_t& GetChildren(const om::common::SegID segID) const {
    return list_[segID];
  }

  const segChildCont_t& GetChildren(OmSegment* seg) const {
    return list_[seg->value()];
  }

  void AddChild(const om::common::SegID segID, OmSegment* child) {
    list_[segID].insert(child);
  }

  void AddChild(OmSegment* seg, OmSegment* child) {
    list_[seg->value()].insert(child);
  }

  void RemoveChild(const om::common::SegID segID, OmSegment* child) {
    list_[segID].erase(child);
  }

  void RemoveChild(OmSegment* seg, OmSegment* child) {
    list_[seg->value()].erase(child);
  }

  SizeAndNumPieces ComputeSegmentSizeWithChildren(OmSegment* seg) {
    int64_t numVoxels = 0;
    int32_t numPieces = 0;

    std::vector<OmSegment*> tmp;
    tmp.push_back(seg);

    while (!tmp.empty()) {
      OmSegment* segRet = tmp.back();
      tmp.pop_back();

      for (OmSegment* seg : GetChildren(segRet)) {
        tmp.push_back(seg);
      }

      const int64_t segSize = segRet->size();

      assert(segSize >= 0);

      numVoxels += segSize;

      assert(numVoxels >= 0);

      ++numPieces;
    }

    SizeAndNumPieces ret = { numVoxels, numPieces };
    return ret;
  }
};
}
}  // namespace om::segment::
