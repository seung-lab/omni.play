#pragma once

#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

namespace om {
namespace chunk {
class CachedUniqueValuesDataSource{
private:
    OmSegmentation* vol_;

public:
    CachedUniqueValuesDataSource(OmSegmentation* vol)
        : vol_(vol)
    {}

    std::shared_ptr<ChunkUniqueValues> Get(const om::chunkCoord& coord){
        auto v = vol_->ChunkUniqueValues()->Values(coord, 1);
        return std::make_shared<ChunkUniqueValues>(v);
    }
};
}}
