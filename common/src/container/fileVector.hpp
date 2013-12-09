#pragma once

#include "container/vector.hpp"
#include "container/fileStoragePolicy.hpp"

namespace om {
namespace data {

template <typename T>
class FileVector : public vector<T, FileStoragePolicy<T>> {
 public:
  FileVector(file::path fnp)
      : vector<T, FileStoragePolicy<T>>(FileStoragePolicy<T>(fnp)) {}
};
}
}  // namespace om::data::