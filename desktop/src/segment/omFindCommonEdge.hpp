#pragma once

#include "segment/omSegment.h"
#include "segment/omSegments.h"

class OmFindCommonEdge {
 public:
  static om::segment::UserEdge FindClosestCommonEdge(OmSegments* segments,
                                                     OmSegment* seg1,
                                                     OmSegment* seg2) {
    if (!seg1 || !seg2) {
      return om::segment::UserEdge();
    }

    if (segments->FindRoot(seg1) != segments->FindRoot(seg2)) {
      log_debugs(unknown) << "can't split disconnected objects";
      return om::segment::UserEdge();
    }

    if (seg1 == seg2) {
      log_debugs(unknown) << "can't split object from self";
      return om::segment::UserEdge();
    }

    OmSegment* s1 = seg1;
    while (0 != s1->getParent()) {
      if (s1->getParent() == seg2) {
        // log_debugs(split) << "splitting child from a direct parent";
        return om::segment::UserEdge { s1->getParent()->value(), s1->value(),
                                       s1->getThreshold(), true };
      }
      s1 = s1->getParent();
    }

    OmSegment* s2 = seg2;
    while (0 != s2->getParent()) {
      if (s2->getParent() == seg1) {
        // log_debugs(split) << "splitting child from a direct parent";
        return om::segment::UserEdge { s2->getParent()->value(), s2->value(),
                                       s2->getThreshold(), true };
      }
      s2 = s2->getParent();
    }

    OmSegment* nearestCommonPred = 0;

    OmSegment* one;

    for (one = seg1; one != nullptr; one = one->getParent()) {
      OmSegment* two;
      for (two = seg2; two != nullptr && one != two; two = two->getParent()) {
      }

      if (one == two) {
        nearestCommonPred = one;
        break;
      }
    }

    assert(nearestCommonPred != 0);

    double minThresh = 100.0;
    OmSegment* minChild = 0;
    for (one = seg1; one != nearestCommonPred; one = one->getParent()) {
      if (one->getThreshold() < minThresh) {
        minThresh = one->getThreshold();
        minChild = one;
      }
    }

    for (one = seg2; one != nearestCommonPred; one = one->getParent()) {
      if (one->getThreshold() < minThresh) {
        minThresh = one->getThreshold();
        minChild = one;
      }
    }

    assert(minChild != 0);
    return om::segment::UserEdge { minChild->getParent()->value(),
                                   minChild->value(), minChild->getThreshold(),
                                   true };
  }
};
