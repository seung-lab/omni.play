#pragma once

namespace om {
namespace mesher {

class setup {
private:
    std::map<OmChunkCoord, std::vector<MeshCollector*> >& occurances_;
    std::map<OmChunkCoord, MeshCollector*>& chunkCollectors_;
    std::deque<OmChunkCoord>& levelZeroChunks_;

public:
    setup(std::map<OmChunkCoord, std::vector<MeshCollector*> >& occurances,
          std::map<OmChunkCoord, MeshCollector*>& chunkCollectors,
          std::deque<OmChunkCoord>& levelZeroChunks)
        : occurances_(occurances)
        , chunkCollectors_(chunkCollectors)
        , levelZeroChunks_(levelZeroChunks)
    {}

};

} // namespace mesher
} // namespace om
