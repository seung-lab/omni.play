#pragma once

#include "common/omCommon.h"

namespace om {
namespace mesher {

class setup {
 private:
  std::map<om::chunkCoord, std::vector<MeshCollector*> >& occurances_;
  std::map<om::chunkCoord, MeshCollector*>& chunkCollectors_;
  std::deque<om::chunkCoord>& levelZeroChunks_;

 public:
  setup(std::map<om::chunkCoord, std::vector<MeshCollector*> >& occurances,
        std::map<om::chunkCoord, MeshCollector*>& chunkCollectors,
        std::deque<om::chunkCoord>& levelZeroChunks)
      : occurances_(occurances),
        chunkCollectors_(chunkCollectors),
        levelZeroChunks_(levelZeroChunks) {}

};

}  // namespace mesher
}  // namespace om
