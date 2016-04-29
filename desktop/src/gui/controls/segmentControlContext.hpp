#pragma once
#include "precomp.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

/*
 * Control context that requires the viewgroupstate and a target
 * segment as the actor for control.
 */
class SegmentControlContext {
 public:
  SegmentControlContext(OmViewGroupState* viewGroupState,
      SegmentDataWrapper* segmentDataWrapper)
    : viewGroupState_(viewGroupState),
      segmentDataWrapper_(segmentDataWrapper) {}

  // allow implementation cleanup
  virtual ~SegmentControlContext() = default;

  // prevent copying
  SegmentControlContext(SegmentControlContext const &) = delete;
  SegmentControlContext& operator=(SegmentControlContext const &) = delete;

 protected:
  OmViewGroupState* const viewGroupState_;
  SegmentDataWrapper* const segmentDataWrapper_;
};
