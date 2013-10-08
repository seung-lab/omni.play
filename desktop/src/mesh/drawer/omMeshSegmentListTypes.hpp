#pragma once

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

// segmentation ID, segment ID, mip level, x, y, z
typedef boost::tuple<OmID, OmSegID, int, int, int, int> OmMeshSegListKey;

class OmSegPtrListValid {
 public:
  OmSegPtrListValid() : isValid(false), freshness(0), isFetching(false) {}
  explicit OmSegPtrListValid(const bool isFetching)
      : isValid(false), freshness(0), isFetching(isFetching) {}
  OmSegPtrListValid(const OmSegPtrList& L, const uint32_t f)
      : isValid(true), list(L), freshness(f), isFetching(false) {}

  bool isValid;
  OmSegPtrList list;
  uint32_t freshness;
  bool isFetching;
};
