#pragma once

#include "common/omCommon.h"

class OmSegmentEdge {
 public:
  OmSegmentEdge() : parentID(0), childID(0), threshold(-100), valid(false) {}

  OmSegmentEdge(const OmSegID p, const OmSegID c, const double t)
      : parentID(p), childID(c), threshold(t), valid(true) {}

  bool isValid() const { return valid; }

  bool operator==(const OmSegmentEdge &rhs) const {
    return (parentID == rhs.parentID && childID == rhs.childID &&
            qFuzzyCompare(threshold, rhs.threshold) && valid == rhs.valid);
  }

  OmSegID parentID;
  OmSegID childID;
  double threshold;
  bool valid;

  friend class QDataStream &operator<<(QDataStream &out,
                                       const OmSegmentEdge &e);
  friend class QDataStream &operator>>(QDataStream &in, OmSegmentEdge &e);
};
