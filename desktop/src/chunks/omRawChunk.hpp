#pragma once

#include "common/common.h"
#include "datalayer/fs/omFileNames.hpp"
#include "utility/malloc.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"
#include "zi/omMutex.h"
#include "zi/omThreads.h"

#include <QFile>

template <typename T>
class OmRawChunk {
 private:
  OmMipVolume* const vol_;
  const om::coords::Chunk coord_;
  const uint64_t chunkOffset_;
  const QString memMapFileName_;
  const uint64_t numBytes_;

  std::shared_ptr<T> data_;
  T* dataRaw_;

  bool dirty_;

  struct raw_chunk_mutex_pool_tag;
  typedef typename zi::spinlock::pool<raw_chunk_mutex_pool_tag>::guard
      mutex_guard_t;

 public:
  OmRawChunk(OmMipVolume* vol, const om::coords::Chunk& coord)
      : vol_(vol),
        coord_(coord),
        chunkOffset_(OmChunkOffset::ComputeChunkPtrOffsetBytes(vol, coord)),
        memMapFileName_(OmFileNames::GetMemMapFileNameQT(vol,
                                                         coord.mipLevel())),
        numBytes_(128 * 128 * 128 * vol_->GetBytesPerVoxel()),
        dataRaw_(nullptr),
        dirty_(false) {
    readData();
  }

  ~OmRawChunk() { Flush(); }

  void SetDirty() { dirty_ = true; }

  void Flush() {
    if (dirty_) {
      log_infos << "flushing " << coord_;
      writeData();
      dirty_ = false;
    }
  }

  inline void Set(const uint64_t index, const T val) {
    dirty_ = true;
    dataRaw_[index] = val;
  }

  inline T Get(const uint64_t index) const { return dataRaw_[index]; }

  uint64_t NumBytes() const { return numBytes_; }

  T* Data() { return dataRaw_; }

  std::shared_ptr<T> SharedPtr() { return data_; }

 private:
  void readData() {
    mutex_guard_t g(chunkOffset_);  // prevent read-while-writing errors

    QFile file(memMapFileName_);
    if (!file.open(QIODevice::ReadOnly)) {
      throw om::IoException("could not open");
    }

    file.seek(chunkOffset_);

    data_ = om::mem::Malloc<T>::NumBytes(numBytes_, om::mem::ZeroFill::DONT);
    char* dataAsCharPtr = (char*)(data_.get());
    const uint64_t readSize = file.read(dataAsCharPtr, numBytes_);

    if (readSize != numBytes_) {
      throw om::IoException("read failed");
    }

    dataRaw_ = data_.get();
  }

  void writeData() {
    mutex_guard_t g(chunkOffset_);  // prevent read-while-writing errors

    QFile file(memMapFileName_);
    if (!file.open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open");
    }

    file.seek(chunkOffset_);
    const uint64_t writeSize =
        file.write(reinterpret_cast<const char*>(dataRaw_), numBytes_);

    if (writeSize != numBytes_) {
      throw om::IoException("write failed");
    }
  }
};
