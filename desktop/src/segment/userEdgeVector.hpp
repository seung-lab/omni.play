#pragma once

#include "segment/types.hpp"
#include "datalayer/QFileVector.hpp"

namespace om {
namespace segment {

class UserEdgeVector : public om::data::QFileVector<om::segment::UserEdge> {
 public:
  UserEdgeVector(file::path fnp)
      : om::data::QFileVector<om::segment::UserEdge>(fnp) {}

  virtual ~UserEdgeVector() {}
};
}
}  // ::om::segment
