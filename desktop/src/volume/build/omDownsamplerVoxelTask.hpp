#pragma once
#include "precomp.h"

#include "volume/io/omMemMappedVolumeImpl.hpp"
#include "volume/build/omDownsamplerTypes.hpp"

template <typename T>
class DownsampleVoxelTask : public zi::runnable {
 private:
  OmMipVolume* const vol_;
  const std::vector<MipLevelInfo>& mips_;
  const MippingInfo& mippingInfo_;
  const om::coords::Chunk coord_;
  const Vector3i srcChunkStartPos_;

  OmMemMappedVolumeImpl<T>* const files_;
  std::vector<T*> rawChunks_;

 public:
  DownsampleVoxelTask(OmMipVolume* vol, const std::vector<MipLevelInfo>& mips,
                      const MippingInfo& mippingInfo,
                      const om::coords::Chunk& coord,
                      OmMemMappedVolumeImpl<T>* files)
      : vol_(vol),
        mips_(mips),
        mippingInfo_(mippingInfo),
        coord_(coord),
        srcChunkStartPos_(coord_.BoundingBox(vol_->Coords()).getMin()),
        files_(files) {
    rawChunks_.resize(mippingInfo.maxMipLevel + 1);

    for (int i = 1; i <= mippingInfo_.maxMipLevel; ++i) {
      const Vector3i dstCoord = coord_ / mips_[i].factor;
      const om::coords::Chunk coord(i, dstCoord);

      rawChunks_[i] = files_->GetChunkPtr(coord);
    }
  }

  void run() {
    auto chunk = vol_->ChunkDS().Get(coord_);
    auto typedChunk = boost::get<om::chunk::Chunk<T>>(chunk.get());
    if (!chunk) {
      log_errors << "Unable to downsample chunk " << coord_;
      return;
    }

    const int sliceSize = 128 * 128;

    for (uint64_t sz = 0, si = 0; sz < 128; sz += 2, si += sliceSize) {
      for (uint64_t sy = 0; sy < 128; sy += 2, si += 128) {
        for (uint64_t sx = 0; sx < 128; sx += 2, si += 2) {

          const Vector3i srcVoxelPosInChunk(sx, sy, sz);

          const T srcVoxel = (*typedChunk)[si];

          pushVoxelIntoMips(srcChunkStartPos_ + srcVoxelPosInChunk, srcVoxel);
        }
      }
    }
  }

  inline uint64_t computeVoxelOffsetIntoChunk(const Vector3i& offsetVec) {
    return (mippingInfo_.tileSize * offsetVec.z +
            mippingInfo_.chunkDims.x * offsetVec.y + offsetVec.x);
  }

  inline void pushVoxelIntoMips(const Vector3i& srcCoord, const T srcVoxel) {
    for (int i = 1; i <= mippingInfo_.maxMipLevel; ++i) {
      if (0 != srcCoord.z % mips_[i].factor ||
          0 != srcCoord.y % mips_[i].factor ||
          0 != srcCoord.x % mips_[i].factor) {
        return;
      }

      const Vector3i dstLocation = srcCoord / mips_[i].factor;
      const Vector3i dstCoord(dstLocation.x % 128, dstLocation.y % 128,
                              dstLocation.z % 128);
      const uint64_t offset = computeVoxelOffsetIntoChunk(dstCoord);
      rawChunks_[i][offset] = srcVoxel;
    }
  }
};
