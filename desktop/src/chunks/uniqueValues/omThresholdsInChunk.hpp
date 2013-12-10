#pragma once

#include "chunks/uniqueValues/omChunkUniqueValuesPerThreshold.hpp"
#include "utility/fuzzyStdObjs.hpp"

class OmThresholdsInChunk {
 private:
  OmSegmentation* const segmentation_;
  const om::coords::Chunk coord_;

  typedef DoubleFuzzyStdMap<OmChunkUniqueValuesPerThreshold*> map_t;
  typedef map_t::iterator iterator;
  typedef map_t::value_type value_t;

  map_t valByThres_;
  zi::mutex lock_;

 public:
  OmThresholdsInChunk(OmSegmentation* segmentation,
                      const om::coords::Chunk& coord)
      : segmentation_(segmentation), coord_(coord) {}

  ~OmThresholdsInChunk() {
    zi::guard g(lock_);
    FOR_EACH(iter, valByThres_) { delete iter->second; }
  }

  OmChunkUniqueValuesPerThreshold* Get(const double threshold) {
    zi::guard g(lock_);

    iterator iter = valByThres_.lower_bound(threshold);

    if (iter != valByThres_.end() &&
        !(valByThres_.key_comp()(threshold, iter->first))) {
      return iter->second;
    }

    iterator p = valByThres_.insert(
        iter, value_t(threshold, new OmChunkUniqueValuesPerThreshold(
                                     segmentation_, coord_, threshold)));
    return p->second;
  }
};
