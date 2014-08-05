#pragma once

#include "precomp.h"
#include "boost/iostreams/device/mapped_file.hpp"
#include "boost/filesystem.hpp"
#include "boost/make_shared.hpp"

#include "datalayer/IOnDiskFile.h"
#include "datalayer/file.h"

namespace om {
namespace mesh {
struct DataEntry;
}
namespace datalayer {

template <typename T>
class ROMemMappedFile : public IOnDiskFile<const T> {
  typedef boost::iostreams::mapped_file_source mapped_file;

 public:
  static ROMemMappedFile<T> CreateNumElements(const file::path& fnp,
                                              const int64_t numElements) {
    file::createFileNumElements<T>(fnp, numElements);

    return ROMemMappedFile<T>(fnp);
  }

  static ROMemMappedFile<T> CreateNumBytes(const file::path& fnp,
                                           const int64_t numBytes) {
    file::resizeFileNumBytes(fnp, numBytes);

    return ROMemMappedFile<T>(fnp);
  }

  static ROMemMappedFile<T> CreateFromData(const file::path& fnp,
                                           std::shared_ptr<T> d,
                                           const int64_t numElements) {
    file::writeNumElements<T>(fnp, d, numElements);

    return ROMemMappedFile<T>(fnp);
  }
  static ROMemMappedFile<T> CreateFromData(const file::path& fnp,
                                           const std::vector<T> data) {
    file::writeAll<T>(fnp, data);

    return ROMemMappedFile<T>(fnp);
  }

 private:
  file::path fnp_;

  std::shared_ptr<mapped_file> file_;

  int64_t numBytes_;

 public:
  // for boost::variant
  ROMemMappedFile() : fnp_("") {}

  ROMemMappedFile(const file::path& fnp)
      : fnp_(fnp), numBytes_(boost::filesystem::file_size(fnp_)) {
    try {
      file_.reset(new mapped_file(fnp_.string()));
    }
    catch (std::exception e) {
      log_errors << "Unable to map " << fnp_.string() << ": " << e.what();
      throw;
    }
  }

  virtual ~ROMemMappedFile() {}

  virtual uint64_t Size() const { return numBytes_; }

  virtual size_t Length() const { return numBytes_ / sizeof(T); }

  virtual void Flush() {}

  virtual const T* GetPtr() const {
    return reinterpret_cast<const T*>(file_->data());
  }

  virtual const T* GetPtrWithOffset(const int64_t offset) const {
    return reinterpret_cast<const T*>(file_->data() + offset);
  }

  virtual const T* begin() const { return GetPtr(); }

  virtual const T* end() const { return &GetPtr()[Length()]; }

  virtual file::path GetBaseFileName() const { return fnp_; }

  virtual bool IsMapped() const { return (bool)file_; }
};

extern template class ROMemMappedFile<mesh::DataEntry>;
extern template class ROMemMappedFile<char>;
extern template class ROMemMappedFile<int8_t>;
extern template class ROMemMappedFile<uint8_t>;
extern template class ROMemMappedFile<int32_t>;
extern template class ROMemMappedFile<uint32_t>;
extern template class ROMemMappedFile<float>;

}  // namespace datalayer
}  // namespace om
