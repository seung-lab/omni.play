#pragma once
#include "precomp.h"


#include "volume/io/omMemMappedVolumeImpl.hpp"
//#include "volume/omVolumeTypes.hpp"
#include "datalayer/hdf5/omHdf5ChunkUtils.hpp"

class omMipVolume;

class OmMemMappedVolume {
 public:
  OmMemMappedVolume() {}

  template <typename VOL>
  void load(VOL* vol) {
    SetDataType(vol);
    loadMemMapFiles();
    log_infos << "loaded data";
  }

  template <typename VOL>
  void create(VOL* vol, const std::map<int, Vector3i>& levDims) {
    SetDataType(vol);
    allocMemMapFiles(levDims);
  }

  int GetBytesPerVoxel() const;
  OmRawDataPtrs GetVolPtr(const int level);
  OmRawDataPtrs getChunkPtrRaw(const om::coords::Chunk& coord);

  void downsample(OmMipVolume* vol);

  template <typename VOL>
  void SetDataType(VOL* vol) {
    log_infos << "setting up volume data...";

    if (om::common::DataType::UNKNOWN == vol->mVolDataType.index()) {
      log_infos << "unknown data type--old file? attempting to infer type...";

      if (OmProject::HasOldHDF5()) {
        vol->mVolDataType = OmHdf5ChunkUtils::DetermineOldVolType(vol);

      } else {
        throw om::IoException("can not resolve volume type");
      }
    }

    volData_ = makeVolData(vol);
  }

 private:
  OmVolDataSrcs volData_;

  void loadMemMapFiles();
  void allocMemMapFiles(const std::map<int, Vector3i>& levDims);
  OmRawDataPtrs GetVolPtrType();

  OmVolDataSrcs makeVolData(OmMipVolume* vol) {
    switch (vol->mVolDataType.index()) {
      case om::common::DataType::INT8:
        return OmMemMappedVolumeImpl<int8_t>(vol);
      case om::common::DataType::UINT8:
        return OmMemMappedVolumeImpl<uint8_t>(vol);
      case om::common::DataType::INT32:
        return OmMemMappedVolumeImpl<int32_t>(vol);
      case om::common::DataType::UINT32:
        return OmMemMappedVolumeImpl<uint32_t>(vol);
      case om::common::DataType::FLOAT:
        return OmMemMappedVolumeImpl<float>(vol);
      case om::common::DataType::UNKNOWN:
        throw om::IoException("unknown data type--probably old file?");
    }

    throw om::ArgException("type not know");
  }
};
