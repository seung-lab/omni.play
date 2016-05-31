#pragma once
#include "precomp.h"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

/*
 * Control context decorator that uses and modifies things
 * in the view.
 */
class ViewInputContext {
 public:
  ViewInputContext(ViewInputConversion* viewInputConversion,
      OmViewGroupState* viewGroupState)
    : viewInputConversion_( viewInputConversion),
      viewGroupState_(viewGroupState) {}

  // allow implementation cleanup
  virtual ~ViewInputContext() = default;
  // prevent copying (Rule of 5)
  ViewInputContext(ViewInputContext const &) = delete;
  ViewInputContext(ViewInputContext &&) = delete;
  ViewInputContext& operator=(ViewInputContext const &) = delete;
  ViewInputContext& operator=(ViewInputContext &&) = delete;

 protected:
  ViewInputConversion* viewInputConversion_;
  OmViewGroupState* viewGroupState_;
};
