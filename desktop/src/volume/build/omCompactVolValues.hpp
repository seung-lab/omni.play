#pragma once
#include "precomp.h"

#include "chunks/omSegChunk.h"
#include "volume/omSegmentation.h"
#include "chunks/omSegChunkDataInterface.hpp"

/**
 * Rewrite MIP 0 in segmentation so that all segment values are
 *  contiguous from [1...n]
 */
class OmCompactVolValues {
 private:
  OmSegmentation* const vol_;

 public:
  OmCompactVolValues(OmSegmentation* vol) : vol_(vol) {}

  void Rewrite() {
    std::unordered_set<uint32_t> values;
    findUniqueValues(values);
    values.erase(0);

    std::unordered_map<uint32_t, uint32_t> compactedValues;
    compactedValues[0] = 0;

    int newValue = 1;
    FOR_EACH(iter, values) { compactedValues[*iter] = newValue++; }

    doRewriteVol(compactedValues);
  }

 private:
  void findUniqueValues(std::unordered_set<uint32_t>& values) {
    std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
        vol_->GetMipChunkCoords(0);
    const uint32_t numChunks = coordsPtr->size();

    int counter = 0;

    FOR_EACH(iter, *coordsPtr) {
      const om::coords::Chunk& coord = *iter;

      ++counter;
      log_info("\rreading chunk %d of %d...", counter, numChunks);
      fflush(stdout);

      OmSegChunk* chunk = vol_->GetChunk(coord);

      std::shared_ptr<uint32_t> dataPtr =
          chunk->SegData()->GetCopyOfChunkDataAsUint32();
      uint32_t const* const data = dataPtr.get();

      for (uint32_t v = 0; v < chunk->Mipping().NumVoxels(); ++v) {
        values.insert(data[v]);
      }
    }
  }

  void doRewriteVol(const std::unordered_map<uint32_t, uint32_t>& compact) {
    std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
        vol_->GetMipChunkCoords(0);
    const uint32_t numChunks = coordsPtr->size();

    int counter = 0;

    FOR_EACH(iter, *coordsPtr) {
      const om::coords::Chunk& coord = *iter;

      ++counter;
      log_info("rewriting chunk %d of %d...", counter, numChunks);

      OmSegChunk* chunk = vol_->GetChunk(coord);

      chunk->SegData()->RewriteChunk(compact);
    }
  }
};
