#pragma once

#include "container/vector.hpp"
#include "datalayer/QFileStoragePolicy.hpp"

namespace om {
namespace data {

template <typename T>
class QFileVector : public vector<T, QFileStoragePolicy<T>> {
 public:
  QFileVector(file::path fnp)
      : vector<T, QFileStoragePolicy<T>>(QFileStoragePolicy<T>(fnp)) {}
};
}
}  // namespace om::data::