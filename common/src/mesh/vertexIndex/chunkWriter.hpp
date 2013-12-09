#pragma once

#include "common/common.h"

namespace om {
namespace mesh {

class ChunkWriter {
 private:
  static const int defaultFileSizeMB = 2;
  static const int defaultFileExpansionFactor = 5;

 private:
  std::string fnp_;
  std::ofstream file_;

  size_t curEndOfFile_;

  zi::rwmutex lock_;

 public:
  ChunkWriter() {}

  ChunkWriter(std::string fnp) : fnp_(fnp), curEndOfFile_(0) {
    openOrCreateFile();
  }

  ~ChunkWriter() { shrinkFileIfNeeded(); }

  void Open(const std::string& fnp) {
    if (IsOpen()) {
      throw InvalidOperationException("May not call Open() twice.");
    }
    fnp_ = fnp;
    openOrCreateFile();
  }

  inline bool IsOpen() const { return fnp_.empty(); }

  template <typename T>
  void Append(FilePart& entry, const T* data, size_t numBytes) {
    if (!IsOpen()) {
      throw IoException("ChunkWriter does not have a file to write to.");
    }

    entry.offsetIntoFile = curEndOfFile_;
    entry.totalBytes = numBytes;
    entry.numElements = numBytes / sizeof(T);
    entry.count =
        numBytes / sizeof(T);  // I still have no idea what this is for...

    expandFileIfNeeded(entry, numBytes);

    file_.write(reinterpret_cast<const char*>(&data[0]), numBytes);

    if (!file_.good()) {
      throw IoException("Failed writing to " + fnp_);
    }

    curEndOfFile_ += numBytes;
  }

 private:
  void openOrCreateFile() {
    zi::rwmutex::write_guard g(lock_);

    const bool fileDidNotExist = !file::exists(fnp_);

    file_.open(fnp_.c_str(), std::ios_base::out | std::ios_base::binary);

    if (!file_.good()) {
      throw IoException("could not open", fnp_);
    }

    if (fileDidNotExist) {
      file::resizeFileNumBytes(fnp_, defaultFileSizeMB * math::bytesPerMB);
      curEndOfFile_ = 0;
    } else {
      file_.seekp(0, std::ios_base::end);
      curEndOfFile_ = file::numBytes(fnp_);
    }
  }

  void expandFileIfNeeded(FilePart& entry, const int64_t numBytes) {
    zi::rwmutex::write_guard g(lock_);

    entry.offsetIntoFile = curEndOfFile_;
    curEndOfFile_ += numBytes;

    if (file::numBytes(fnp_) <= curEndOfFile_) {
      file::resizeFileNumBytes(fnp_,
                               curEndOfFile_ * defaultFileExpansionFactor);
    }
  }

  void shrinkFileIfNeeded() {
    zi::rwmutex::write_guard g(lock_);

    if (file::numBytes(fnp_) > curEndOfFile_) {
      file::resizeFileNumBytes(fnp_, curEndOfFile_);
    }
  }
};
}
}  // namespace om::mesh::
