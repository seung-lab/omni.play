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
inline std::ostream& operator<<(std::ostream& out, const ImportEdge& e) {
  return out << "{" << e.seg1 << " - " << e.seg2 << " : " << e.threshold << "}";
}

struct Edge {
  uint32_t number;
  uint32_t node1ID;
  uint32_t node2ID;
  double threshold;
  uint8_t userJoin;
  uint8_t userSplit;
  uint8_t wasJoined;  // transient state
  bool operator< (const Edge &edge) {
      if ( threshold > edge.threshold ) return true;
      if ( threshold == edge.threshold )
      {
          if ( node1ID < edge.node1ID ) return true;
          if ( node1ID == edge.node1ID && node2ID < edge.node2ID ) return true;
          return false;
      }
      return false;
  }

  uint32_t otherNodeID(uint32_t nodeID) {
    if (node1ID == nodeID) {
      return node2ID;
    } else {
      return node1ID;
    }
  }
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
