#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/container.hpp"
#include "chunks/omChunkItemContainerMatrix.hpp"

template <typename VOL, typename T>
class OmChunkItemContainer {
 private:
  VOL* const vol_;
  ;

  zi::spinlock lock_;

  typedef OmChunkItemContainerMatrix<VOL, T> matrix_t;
  std::vector<matrix_t*> mips_;

  void setup() {
    const int rootMipLevel = vol_->Coords().RootMipLevel();

    std::vector<matrix_t*> newMips;
    newMips.resize(rootMipLevel + 1);

    for (int mipLevel = 0; mipLevel <= rootMipLevel; ++mipLevel) {
      const Vector3i dims =
          vol_->Coords().MipLevelDimensionsInMipChunks(mipLevel);
      newMips[mipLevel] = new matrix_t(vol_, dims.x, dims.y, dims.z);
    }

    {
      zi::guard g(lock_);
      mips_.swap(newMips);
    }

    om::container::clearPtrVec(newMips);
  }

 public:
  OmChunkItemContainer(VOL* vol) : vol_(vol) { setup(); }

  ~OmChunkItemContainer() { om::container::clearPtrVec(mips_); }

  void UpdateFromVolResize() { setup(); }

  void Clear() { setup(); }

  T* Get(const om::coords::Chunk& coord) {
    zi::guard g(lock_);

    const std::size_t mipLevel = coord.mipLevel();

    if (mipLevel >= mips_.size()) {
      throw om::ArgException("invalid mip level");
    }

    return mips_[mipLevel]->Get(coord);
  }
};
