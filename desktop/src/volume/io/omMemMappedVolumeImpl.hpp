#pragma once

#include "common/logging.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omMemMappedFileQT.hpp"
#include "datalayer/omIDataVolume.hpp"
#include "datalayer/omIDataVolume.hpp"
#include "project/omProject.h"
#include "utility/omStringHelpers.h"
#include "volume/io/omChunkOffset.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolumeTypes.hpp"

#include <zi/mutex.hpp>
#include <QFile>

template <typename T>
class OmIOnDiskFile;

template <typename T>
class OmMemMappedVolumeImpl : public IDataVolume<T> {
 private:
  OmMipVolume* vol_;
  std::vector<std::shared_ptr<OmIOnDiskFile<T> > > maps_;

  typedef OmMemMappedFileReadQT<T> reader_t;
  typedef OmMemMappedFileWriteQT<T> writer_t;

 public:
  // for boost::varient
  OmMemMappedVolumeImpl() {}

  OmMemMappedVolumeImpl(OmMipVolume* vol) : vol_(vol) {}

  virtual ~OmMemMappedVolumeImpl() {}

  OmRawDataPtrs GetType() const { return (T*)0; }

  void Load() {
    log_infos << "loaded mem maps";

    resizeMapsVector();

    for (size_t level = 0; level < maps_.size(); ++level) {
      maps_[level] = reader_t::Reader(getFileName(level));
    }
  }

  void Create(const std::map<int, Vector3i>& levelsAndDims) {
    resizeMapsVector();

    const int64_t bps = GetBytesPerVoxel();

    FOR_EACH(it, levelsAndDims) {
      const int level = it->first;
      const Vector3<int64_t> dims = it->second;
      const int64_t size = dims.x * dims.y * dims.z * bps;

      log_infos << "mip " << level
                << ": size is: " << om::string::humanizeNum(size) << " ("
                << dims.x << "," << dims.y << "," << dims.z << ")";

      maps_[level] = writer_t::WriterNumBytes(
          getFileName(level), size, om::common::ZeroMem::DONT_ZERO_FILL);
    }

    log_infos << "OmMemMappedVolume: done allocating data";
  }

  T* GetPtr(const int level) const { return maps_[level]->GetPtr(); }

  T* GetChunkPtr(const om::chunkCoord& coord) const {
    const int level = coord.Level;
    const uint64_t offset =
        OmChunkOffset::ComputeChunkPtrOffsetBytes(vol_, coord);
    T* ret = maps_[level]->GetPtrWithOffset(offset);
    assert(ret);
    return ret;
  }

  int GetBytesPerVoxel() const { return sizeof(T); }

 private:
  void resizeMapsVector() {
    maps_.resize(vol_->Coords().GetRootMipLevel() + 1);
  }

  std::string getFileName(const int level) const {
    return OmFileNames::GetMemMapFileName(vol_, level);
  }
};
