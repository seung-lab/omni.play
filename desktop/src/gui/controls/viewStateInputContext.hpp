#pragma once
#include "precomp.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

/*
 * Control context decorator for modifying the viewgroupstate
 */
class ViewStateInputContext {
 public:
  ViewStateInputContext(OmViewGroupState* viewGroupState)
    : viewGroupState_(viewGroupState) {}

  // allow implementation cleanup
  virtual ~ViewStateInputContext() = default;
  // prevent copying (Rule of 5)
  ViewStateInputContext(ViewStateInputContext const &) = delete;
  ViewStateInputContext(ViewStateInputContext &&) = delete;
  ViewStateInputContext& operator=(ViewStateInputContext const &) = delete;
  ViewStateInputContext& operator=(ViewStateInputContext &&) = delete;

 protected:
  OmViewGroupState* viewGroupState_;
};
