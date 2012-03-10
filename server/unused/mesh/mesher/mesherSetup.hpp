#pragma once

#include "common/common.h"

namespace om {
namespace mesher {

class setup {
private:
    std::map<coords::chunkCoord, std::vector<MeshCollector*> >& occurances_;
    std::map<coords::chunkCoord, MeshCollector*>& chunkCollectors_;
    std::deque<coords::chunkCoord>& levelZeroChunks_;

public:
    setup(std::map<coords::chunkCoord, std::vector<MeshCollector*> >& occurances,
          std::map<coords::chunkCoord, MeshCollector*>& chunkCollectors,
          std::deque<coords::chunkCoord>& levelZeroChunks)
        : occurances_(occurances)
        , chunkCollectors_(chunkCollectors)
        , levelZeroChunks_(levelZeroChunks)
    {}

};

} // namespace mesher
} // namespace om
