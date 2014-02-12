#pragma once

class OmSegment;

struct SegPtrAndColor {
  OmSegment* seg;
  Vector3f color;
};
typedef std::deque<SegPtrAndColor> SegPtrAndColorList;
