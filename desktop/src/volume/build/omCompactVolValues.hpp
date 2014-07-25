#pragma once
#include "precomp.h"

#include "volume/omSegmentation.h"

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
    for (auto& iter : vol_->Iterate<om::common::SegID>()) {
      if (iter.value()) {
        values.insert(iter.value());
      }
    }
  }

  void doRewriteVol(const std::unordered_map<uint32_t, uint32_t>& compact) {
    std::shared_ptr<std::deque<om::coords::Chunk>> coordsPtr =
        vol_->GetMipChunkCoords(0);
    const uint32_t numChunks = coordsPtr->size();

    int counter = 0;

    for (auto& cc : *coordsPtr) {
      ++counter;
      log_info("rewriting chunk %d of %d...", counter, numChunks);

      auto chunk = vol_->GetChunk(cc);
      auto typedChunk = boost::get<om::chunk::Chunk<uint32_t>>(chunk.get());
      if (!typedChunk) {
        throw om::InvalidOperationException(
            "Unable to load chunk for rewrite.");
      }
      for (int i = 0; i < vol_->Coords().GetNumberOfVoxelsPerChunk(); ++i) {
        auto& val = (*typedChunk)[i];
        val = compact.at(val);
      }
      vol_->ChunkDS().Put(cc, chunk);
    }
  }
};
