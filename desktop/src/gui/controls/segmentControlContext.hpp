#pragma once
#include "precomp.h"

/*
 * The tool mode is the input context for control.
 */
class SegmentControlContext {
 public:
  ToolControlContext(OmViewGroupState* viewGroupState,
      SegmentDataWrapper* segmentDataWrapper)
    : viewGroupState_(viewGroupState)
      segmentDataWrapper_(segmentDataWrapper) {
    }

 protected:
  OmViewGroupState* const viewGroupState_;
  SegmentDataWrapper* const segmentDataWrapper_;
}
