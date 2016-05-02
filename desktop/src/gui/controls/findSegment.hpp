#pragma once
#include "precomp.h"
#include "utility/dataWrappers.h"
#include <functional>

/*
 * Decorator to allow finding segmentDataWrapper for input x,y
 */
class FindSegment {
 public:
  FindSegment() {}
  FindSegment(std::function<
      boost::optional<SegmentDataWrapper>(int, int)> findSegmentFunction)
    : findSegmentFunction_(findSegmentFunction) {}

  // allow implementation cleanup
  virtual ~FindSegment() = default;
  // prevent copying (Rule of 5)
  FindSegment(FindSegment const &) = delete;
  FindSegment(FindSegment &&) = delete;
  FindSegment& operator=(FindSegment const &) = delete;
  FindSegment& operator=(FindSegment &&) = delete;

  boost::optional<SegmentDataWrapper> findSegmentDataWrapper(int x, int y) {
    return findSegmentFunction_(x, y);
  }

 protected:
  std::function<boost::optional<SegmentDataWrapper>(int, int)> findSegmentFunction_;
};
