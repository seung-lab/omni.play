#pragma once

#include "common/common.h"

class OmSegment;

struct SegInfo {
  OmSegment* seg;
  om::common::SegID segID;
  int64_t sizeIncludingChildren;
  int64_t numChildren;
};

struct GUIPageOfSegments {
  std::vector<SegInfo> segs;
  int pageNum;
};

struct GUIPageRequest {
  uint32_t offset;
  int numToGet;
  om::common::SegID startSeg;
};

namespace om {
namespace segLists {

// largest first; if sizes are the same, sort by ID ascending
static bool CmpSegInfo(const SegInfo& a, const SegInfo& b) {
  if (a.sizeIncludingChildren != b.sizeIncludingChildren) {
    return a.sizeIncludingChildren > b.sizeIncludingChildren;
  }

  return a.segID < b.segID;
}

struct LargestSegInfoFirst : std::binary_function<SegInfo, SegInfo, bool> {
  bool operator()(const SegInfo& a, const SegInfo& b) const {
    return CmpSegInfo(a, b);
  }
};

std::shared_ptr<GUIPageOfSegments> getPage(const std::vector<SegInfo> list,
                                           const uint32_t startIndex,
                                           const uint32_t numToGet);

}  // gui
}  // om
