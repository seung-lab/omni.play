#pragma once

#include "boost/iostreams/device/mapped_file.hpp"
#include "boost/filesystem.hpp"
#include "boost/make_shared.hpp"
#include <iosfwd>

#include "datalayer/IOnDiskFile.h"
#include "datalayer/file.h"

typedef boost::iostreams::mapped_file mapped_file;

namespace om {
namespace datalayer {

template <typename T>
class MemMappedFile : public IOnDiskFile<T> {
 public:
  static MemMappedFile<T> CreateNumElements(const std::string& fnp,
                                            const int64_t numElements) {
    file::createFileNumElements<T>(fnp, numElements);

    return MemMappedFile<T>(fnp);
  }

  static MemMappedFile<T> CreateNumBytes(const std::string& fnp,
                                         const int64_t numBytes) {
    file::resizeFileNumBytes(fnp, numBytes);

    return MemMappedFile<T>(fnp);
  }

  static MemMappedFile<T> CreateFromData(const std::string& fnp,
                                         std::shared_ptr<T> d,
                                         const int64_t numElements) {
    file::writeNumElements<T>(fnp, d, numElements);

    return MemMappedFile<T>(fnp);
  }

 private:
  file::path fnp_;

  std::shared_ptr<mapped_file> file_;

  int64_t numBytes_;

 public:
  // for boost::variant
  MemMappedFile() : fnp_("") {}

  MemMappedFile(const file::path& fnp)
      : fnp_(fnp), numBytes_(boost::filesystem::file_size(fnp_)) {
    file_.reset(new mapped_file(fnp_.string()));
  }

  virtual ~MemMappedFile() {}

  virtual uint64_t Size() const { return numBytes_; }

  virtual size_t Length() const { return numBytes_ / sizeof(T); }

  virtual void Flush() {}

  virtual T* GetPtr() const { return reinterpret_cast<T*>(file_->data()); }

  virtual T* GetPtrWithOffset(const int64_t offset) const {
    return reinterpret_cast<T*>(file_->data() + offset);
  }

  virtual T* begin() const { return GetPtr(); }

  virtual T* end() const { return &GetPtr()[Length()]; }

  virtual file::path GetBaseFileName() const { return fnp_; }

  virtual bool IsMapped() const { return (bool)file_; }
};

template <typename T>
class MemMappedFileRO {
 public:
  // for boost::variant
  MemMappedFileRO() : fnp_("") {}

  MemMappedFileRO(const file::path& fnp)
      : fnp_(fnp), numBytes_(boost::filesystem::file_size(fnp_)) {
    file_.reset(new mapped_file(fnp_.string(),
                                std::ios_base::binary | std::ios_base::out));
  }

  virtual ~MemMappedFileRO() {}

  virtual uint64_t Size() const { return numBytes_; }

  virtual size_t Length() const { return numBytes_ / sizeof(T); }

  virtual void Flush() {}

  virtual T* GetPtr() const { return reinterpret_cast<T*>(file_->data()); }

  virtual T* GetPtrWithOffset(const int64_t offset) const {
    return reinterpret_cast<T*>(file_->data() + offset);
  }

  virtual T* begin() const { return GetPtr(); }

  virtual T* end() const { return &GetPtr()[Length()]; }

  virtual file::path GetBaseFileName() const { return fnp_; }

  virtual bool IsMapped() const { return (bool)file_; }

 private:
  file::path fnp_;
  std::shared_ptr<mapped_file> file_;
  int64_t numBytes_;
};

}  // namespace datalayer
}  // namespace om
