#pragma once
#include "precomp.h"
#include "utility/dataWrappers.h"
#include <functional>

/*
 * Decorator to allow finding segmentDataWrapper for input x,y
 */
class FindSegmentControl {
 public:
  FindSegmentControl() {}
  FindSegmentControl(std::function<
      std::shared_ptr<SegmentDataWrapper>(int, int)> findSegmentFunction)
    : findSegmentFunction_(findSegmentFunction) {}

  // allow implementation cleanup
  virtual ~FindSegmentControl() = default;
  // prevent copying (Rule of 5)
  FindSegmentControl(FindSegmentControl const &) = delete;
  FindSegmentControl(FindSegmentControl &&) = delete;
  FindSegmentControl& operator=(FindSegmentControl const &) = delete;
  FindSegmentControl& operator=(FindSegmentControl &&) = delete;

  std::shared_ptr<SegmentDataWrapper> getSegmentDataWrapper(int x, int y) {
    return findSegmentFunction_(x, y);
  }

 protected:
  std::function<std::shared_ptr<SegmentDataWrapper>(int, int)> findSegmentFunction_;
};
