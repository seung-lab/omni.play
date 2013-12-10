#pragma once

/**
 * OmSegChunk extends OmChunk with extra funtionality specific to
 *   manipulating segmentation data.
 *
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunk.h"
#include "zi/omMutex.h"

class OmSegmentation;
namespace om {
namespace segchunk {
class dataInterface;
}
}

class OmSegChunk : public OmChunk {
 private:
  OmSegmentation* const vol_;
  const std::unique_ptr<om::segchunk::dataInterface> segChunkData_;

  std::set<om::common::SegID> modifiedSegIDs_;
  zi::spinlock modifiedSegIDsLock_;

 public:
  OmSegChunk(OmSegmentation* vol, const om::coords::Chunk& coord);
  virtual ~OmSegChunk();

  uint32_t GetVoxelValue(const om::coords::Data& vox);
  void SetVoxelValue(const om::coords::Data& vox, const uint32_t value);

  om::segchunk::dataInterface* SegData() { return segChunkData_.get(); }
};
