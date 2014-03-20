#pragma once
#include "precomp.h"

/**
 *  OmChunk encapsulates a 3D-matrix of image data (typically 128^3) and
 *    its associated size/positioning metadata.
 *
 *  Brett Warne - bwarne@mit.edu - 2/24/09
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunkMipping.hpp"
#include "volume/omVolumeTypes.hpp"

class OmChannel;
class OmChannelImpl;
class OmSegmentation;
namespace om {
namespace chunk {
class dataInterface;
}
}

class OmChunk {
 public:
  OmChunk(OmMipVolume& vol, const om::coords::Chunk& coord);

  virtual ~OmChunk();

  bool ContainsVoxel(const om::coords::Data& vox) const {
    return &vox.volume() == &vol_.Coords() && vox.mipLevel() == GetLevel() &&
           mipping_.Extent().contains(vox);
  }

 public:
  const om::coords::Chunk& GetCoordinate() const { return coord_; }
  int GetLevel() const { return coord_.mipLevel(); }
  const om::coords::Data GetDimensions() const {
    return mipping_.Extent().getUnitDimensions();
  }

  om::chunk::dataInterface* Data() { return chunkData_.get(); }

  OmChunkMipping& Mipping() { return mipping_; }

 protected:
  const om::coords::Chunk coord_;
  const std::unique_ptr<om::chunk::dataInterface> chunkData_;
  const OmMipVolume& vol_;

  OmChunkMipping mipping_;
};
