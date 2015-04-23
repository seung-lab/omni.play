#pragma once
#include "precomp.h"

#include "threads/taskManager.hpp"
#include "utility/omTimer.hpp"
#include "volume/build/omDownsamplerTypes.hpp"
#include "volume/build/omDownsamplerVoxelTask.hpp"
#include "volume/omMipVolume.h"

template <typename T>
class OmDownsampler {
 private:
  OmMipVolume* const vol_;
  OmMemMappedVolumeImpl<T>* const files_;

  std::vector<MipLevelInfo> mips_;
  MippingInfo mippingInfo_;

 public:
  OmDownsampler(OmMipVolume* vol, OmMemMappedVolumeImpl<T>* files)
      : vol_(vol), files_(files) {
    mippingInfo_.maxMipLevel = vol_->Coords().RootMipLevel();

    files_->Load();
    const Vector3<uint64_t> chunkDims = vol_->Coords().ChunkDimensions();

    mippingInfo_.chunkDims = chunkDims;
    mippingInfo_.chunkSize = chunkDims.x * chunkDims.y * chunkDims.z;
    mippingInfo_.tileSize = chunkDims.x * chunkDims.y;

    mips_.resize(mippingInfo_.maxMipLevel + 1);

    for (int i = 0; i <= mippingInfo_.maxMipLevel; ++i) {
      mips_[i].factor = om::math::pow2int(i);

      const Vector3i dims = vol_->Coords().DimsRoundedToNearestChunk(i);
      mips_[i].volDims = dims;
      mips_[i].volSlabSize = dims.x * dims.y * chunkDims.z;
      mips_[i].volSliceSize = dims.x * dims.y;
      mips_[i].volRowSize = dims.x * chunkDims.y * chunkDims.z;
      mips_[i].totalVoxels = dims.x * dims.y * dims.z;
    }
  }

  void DownsampleChooseOneVoxelOfNine() {
    OmTimer timer;
    log_infos << "downsampling...";

    om::thread::ThreadPool threadPool;
    threadPool.start(3);

    std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
        vol_->GetMipChunkCoords(0);

    for(auto coord : *coordsPtr) {
      auto task = std::make_shared<DownsampleVoxelTask<T> >(vol_, mips_, mippingInfo_, coord, files_);
      threadPool.push_back(task);
    }

    threadPool.join();
    log_infos << "\t downsampling done in " << timer.s_elapsed() << " secs";
  }
};
