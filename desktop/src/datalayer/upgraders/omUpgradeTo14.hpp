#pragma once
#include "precomp.h"

#include "datalayer/hdf5/omHdf5ChunkUtils.hpp"
#include "utility/dataWrappers.h"
#include "volume/io/omVolumeData.h"
#include "volume/omVolumeTypes.hpp"

// extract volumes from hdf5 to mem-map

class OmUpgradeTo14 {
 public:
  void copyDataOutFromHDF5() { doConvert(); }

 private:
  void doConvert() {
    FOR_EACH(iter, ChannelDataWrapper::ValidIDs()) {
      log_infos << "converting channel " << *iter;
      ChannelDataWrapper cdw(*iter);
      convertVolume(*cdw.GetChannel());
    }

    FOR_EACH(iter, SegmentationDataWrapper::ValidIDs()) {
      log_infos << "converting segmentation " << *iter;
      SegmentationDataWrapper sdw(*iter);
      OmSegmentation& seg = *sdw.GetSegmentation();
      convertVolume(seg);
      seg.LoadVolData();
    }
  }

  template <typename T>
  void convertVolume(T& vol) {
    allocate(vol);
    copyData(vol);
  }

  template <typename T>
  void allocate(T& vol) {
    std::map<int, Vector3i> levelsAndDims;

    for (int level = 0; level <= vol.Coords().RootMipLevel(); level++) {
      levelsAndDims[level] = vol.Coords().DimsRoundedToNearestChunk(level);
    }

    // allocate mem-mapped files...
    vol.VolData().create(&vol, levelsAndDims);

    vol.UpdateFromVolResize();
  }

  template <typename T>
  void copyData(T& vol) {
    const uint32_t numChunks = vol.Coords().ComputeTotalNumChunks();
    uint32_t counter = 0;

    for (int level = 0; level <= vol.Coords().RootMipLevel(); ++level) {
      if (!OmHdf5ChunkUtils::VolumeExistsInHDF5(&vol, level)) {
        log_infos << "no HDF5 volume data found for mip " << level;
        continue;
      }

      std::shared_ptr<std::deque<om::coords::Chunk>> coordsPtr =
          vol.GetMipChunkCoords(level);

      FOR_EACH(iter, *coordsPtr) {
        ++counter;
        log_info("copying chunk %d of %d...", counter, numChunks);
        fflush(stdout);

        copyChunk(vol, *iter);
      }
    }
  }

  template <typename T>
  void putChunk(OmMipVolume& vol, const om::coords::Chunk& coord,
                std::shared_ptr<T> data) {
    vol.ChunkDS().Put(coord,
                      std::make_shared<om::chunk::ChunkVar>(
                          om::chunk::Chunk<T>(coord, vol.Coords(), data)));
  }

  void copyChunk(OmMipVolume& vol, const om::coords::Chunk& coord) {
    OmDataWrapperPtr hdf5 = OmHdf5ChunkUtils::ReadChunkData(&vol, coord);
    switch (vol.getVolDataType().index()) {
      case om::common::DataType::INT8:
        putChunk(vol, coord, om::ptrs::UnWrap<int8_t>(hdf5));
      case om::common::DataType::UINT8:
        putChunk(vol, coord, om::ptrs::UnWrap<uint8_t>(hdf5));
      case om::common::DataType::INT32:
        putChunk(vol, coord, om::ptrs::UnWrap<int>(hdf5));
      case om::common::DataType::UINT32:
        putChunk(vol, coord, om::ptrs::UnWrap<uint32_t>(hdf5));
      case om::common::DataType::FLOAT:
        putChunk(vol, coord, om::ptrs::UnWrap<float>(hdf5));
      default:
        throw om::ArgException("Invalid HDF5 Chunk Data.");
    }
  }
};
