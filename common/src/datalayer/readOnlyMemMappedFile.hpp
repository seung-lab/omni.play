#pragma once

#include "boost/iostreams/device/mapped_file.hpp"
#include "boost/filesystem.hpp"

#include "datalayer/file.h"

typedef boost::iostreams::mapped_file mapped_file;

namespace om {
namespace datalayer {

template <typename T> class ReadOnlyMemMappedFile {
 public:
  // for boost::variant
  ReadOnlyMemMappedFile() : fnp_(std::string("")) {}

  ReadOnlyMemMappedFile(const std::string& fnp)
      : fnp_(fnp), numBytes_(boost::filesystem::file_size(fnp_)) {
    file_.reset(
        new mapped_file(fnp_, boost::iostreams::mapped_file_base::readonly));
  }

  ~ReadOnlyMemMappedFile() {}

  uint64_t Size() const { return numBytes_; }

  size_t Length() const { return numBytes_ / sizeof(T); }

  const T* GetPtr() const {
    return reinterpret_cast<const T*>(file_->const_data());
  }

  const T* GetPtrWithOffset(const int64_t offset) const {
    return reinterpret_cast<const T*>(file_->const_data() + offset);
  }

  T* begin() const { return GetPtr(); }

  T* end() const { return &GetPtr()[Length()]; }

  std::string GetBaseFileName() const { return fnp_; }

  bool IsMapped() const { return (bool) file_; }

 private:
  std::string fnp_;

  std::shared_ptr<mapped_file> file_;

  int64_t numBytes_;
};

}  // namespace datalayer
}  // namespace om
