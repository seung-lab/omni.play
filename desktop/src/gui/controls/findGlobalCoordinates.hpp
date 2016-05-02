#pragma once
#include "precomp.h"
#include "utility/dataWrappers.h"
#include "coordinates/global.h"
#include <functional>

/*
 * Decorator to allow finding segmentDataWrapper for input x,y
 */
class FindGlobalCoordinates {
 public:
  FindGlobalCoordinates() {}
  FindGlobalCoordinates(std::function<boost::optional<om::coords::Global>(int, int)>
      findGlobalCoordinatesFunction)
    : findGlobalCoordinatesFunction_(findGlobalCoordinatesFunction) {}

  // allow implementation cleanup
  virtual ~FindGlobalCoordinates() = default;
  // prevent copying (Rule of 5)
  FindGlobalCoordinates(FindGlobalCoordinates const &) = delete;
  FindGlobalCoordinates(FindGlobalCoordinates &&) = delete;
  FindGlobalCoordinates& operator=(FindGlobalCoordinates const &) = delete;
  FindGlobalCoordinates& operator=(FindGlobalCoordinates &&) = delete;

  boost::optional<om::coords::Global> findGlobalCoordinates(int x, int y) {
    return findGlobalCoordinatesFunction_(x, y);
  }

 protected:
  std::function<boost::optional<om::coords::Global>(int, int)> 
    findGlobalCoordinatesFunction_;
};
