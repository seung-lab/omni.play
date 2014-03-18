#pragma once
#include "precomp.h"

#include "segment/omSegments.h"
#include "segment/omSegmentEdgeUtils.hpp"

class OmFindCommonEdge {
 public:
  static OmSegmentEdge FindClosestCommonEdge(OmSegments* segments,
                                             OmSegment* seg1, OmSegment* seg2) {
    if (!seg1 || !seg2) {
      return om::segmentEdge::MakeEdge();
    }

    if (segments->findRoot(seg1) != segments->findRoot(seg2)) {
      log_infos << "can't split disconnected objects";
      return om::segmentEdge::MakeEdge();
    }

    if (seg1 == seg2) {
      log_infos << "can't split object from self";
      return om::segmentEdge::MakeEdge();
    }

    OmSegment* s1 = seg1;
    while (0 != s1->parent_) {
      if (s1->parent_ == seg2) {
        return om::segmentEdge::MakeEdge(s1);
      }
      s1 = s1->parent_;
    }

    OmSegment* s2 = seg2;
    while (0 != s2->parent_) {
      if (s2->parent_ == seg1) {
        return om::segmentEdge::MakeEdge(s2);
      }
      s2 = s2->parent_;
    }

    OmSegment* nearestCommonPred = 0;

    OmSegment* one;

    for (one = seg1; one != nullptr; one = one->parent_) {
      OmSegment* two;
      for (two = seg2; two != nullptr && one != two; two = two->parent_) {
      }

      if (one == two) {
        nearestCommonPred = one;
        break;
      }
    }

    assert(nearestCommonPred != 0);

    double minThresh = 100.0;
    OmSegment* minChild = 0;
    for (one = seg1; one != nearestCommonPred; one = one->parent_) {
      if (one->threshold_ < minThresh) {
        minThresh = one->threshold_;
        minChild = one;
      }
    }

    for (one = seg2; one != nearestCommonPred; one = one->parent_) {
      if (one->threshold_ < minThresh) {
        minThresh = one->threshold_;
        minChild = one;
      }
    }

    assert(minChild != 0);
    return om::segmentEdge::MakeEdge(minChild);
  }
};
