#pragma once

#include "common/common.h"
#include "common/colors.h"
#include "coordinates/coordinates.h"

namespace om {
namespace segment {

const size_t PageSize = 100000;

struct Data {
  common::SegID value;
  common::Color color;
  uint64_t size;
  AxisAlignedBoundingBox<int> bounds;
};

struct ImportEdge {
  common::SegID seg1;
  common::SegID seg2;
  double threshold;
};

struct Edge {
  uint32_t number;
  uint32_t node1ID;
  uint32_t node2ID;
  double threshold;
  uint8_t userJoin;
  uint8_t userSplit;
  uint8_t wasJoined;  // transient state
};

struct UserEdge {
  om::common::SegID parentID;
  om::common::SegID childID;
  double threshold;
  bool valid;
  bool operator==(const UserEdge& other) const {
    return std::tie(parentID, childID, threshold, valid) ==
           std::tie(other.parentID, other.childID, other.threshold,
                    other.valid);
  }
};
}
}
