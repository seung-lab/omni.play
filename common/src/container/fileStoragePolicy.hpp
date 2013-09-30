#pragma once

#include "datalayer/file.h"

namespace om {
namespace data {

template <typename T> class FileStoragePolicy {
 public:
  FileStoragePolicy(file::path fnp) : fnp_(fnp) { file::readAll(fnp_, data_); }

  size_t size() const { return data_.size(); }
  void resize(size_t n, const T& val) { data_.resize(n, val); }
  T& doGet(size_t i) { return data_[i]; }
  const T& doGet(size_t i) const { return data_[i]; }
  void flush() { file::writeAll(fnp_, data_); }

 private:
  file::path fnp_;
  std::vector<T> data_;
};
}
}  // namespace om::datalayer::
