#pragma once
#include "precomp.h"

class OmSegment;

namespace om {
namespace v3d {

struct SegPtrAndColor {
  OmSegment* seg;
  Vector3f color;
};
typedef std::deque<SegPtrAndColor> SegPtrAndColorList;
}
}  // namespace
