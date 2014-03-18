#pragma once
#include "precomp.h"

#include "common/common.h"

namespace om {
namespace mesher {

class setup {
 private:
  std::map<om::coords::Chunk, std::vector<MeshCollector*> >& occurances_;
  std::map<om::coords::Chunk, MeshCollector*>& chunkCollectors_;
  std::deque<om::coords::Chunk>& levelZeroChunks_;

 public:
  setup(std::map<om::coords::Chunk, std::vector<MeshCollector*> >& occurances,
        std::map<om::coords::Chunk, MeshCollector*>& chunkCollectors,
        std::deque<om::coords::Chunk>& levelZeroChunks)
      : occurances_(occurances),
        chunkCollectors_(chunkCollectors),
        levelZeroChunks_(levelZeroChunks) {}
};

}  // namespace mesher
}  // namespace om
