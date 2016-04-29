#pragma once
#include "precomp.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

/*
 * Control context for modifying the viewgroupstate
 */
class ViewStateControl {
 public:
  ViewStateControl(OmViewGroupState* viewGroupState,
      SegmentDataWrapper* segmentDataWrapper)
    : viewGroupState_(viewGroupState) {}

  // allow implementation cleanup
  virtual ~ViewStateControl() = default;
  // prevent copying (Rule of 5)
  ViewStateControl(ViewStateControl const &) = delete;
  ViewStateControl(ViewStateControl &&) = delete;
  ViewStateControl& operator=(ViewStateControl const &) = delete;
  ViewStateControl& operator=(ViewStateControl &&) = delete;

 protected:
  OmViewGroupState* const viewGroupState_;
};
