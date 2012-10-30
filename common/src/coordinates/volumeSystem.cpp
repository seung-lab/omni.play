#include "common/common.h"
#include "zi/concurrency.hpp"
#include "coordinates/volumeSystem.h"

namespace om {
namespace coords {

volumeSystem::volumeSystem()
    : dataToGlobal_(Matrix4f::IDENTITY)
    , globalToData_(Matrix4f::IDENTITY)
    , normToGlobal_(Matrix4f::IDENTITY)
    , globalToNorm_(Matrix4f::IDENTITY)
    , chunkDim_(DefaultChunkDim)
    , mMipRootLevel(0)
{
    SetDataDimensions(Vector3i(DefaultChunkDim,
                                DefaultChunkDim,
                                DefaultChunkDim));

    UpdateRootLevel();
}

void volumeSystem::UpdateRootLevel()
{
    //determine max level
    Vector3i source_dims = GetDataDimensions();
    int max_source_dim = source_dims.getMaxComponent();
    int mipchunk_dim = GetChunkDimension();

    if (max_source_dim <= mipchunk_dim) {
        mMipRootLevel = 0;
    } else {
        mMipRootLevel = ceil(log((float) (max_source_dim) / mipchunk_dim) / log(2.0f));
    }
}

GlobalBbox volumeSystem::GetDataExtent() const
{
    Vector3f abs = GetAbsOffset();
    return GlobalBbox(abs, abs - Vector3f::ONE + dataDimensions_);
}

bool volumeSystem::ContainsMipChunk(const Chunk & rMipCoord) const
{
    //if level is greater than root level
    if(rMipCoord.mipLevel() < 0 ||
        rMipCoord.mipLevel() > GetRootMipLevel()){
        return false;
    }

    //convert to data box in leaf (MIP 0)
    GlobalBbox bbox = rMipCoord.BoundingBox(this).toGlobalBbox();

    bbox.intersect(GetDataExtent());
    if(bbox.isEmpty()){
        return false;
    }

    //else valid mip coord
    return true;
}

Chunk volumeSystem::RootMipChunkCoordinate() const {
    return Chunk(mMipRootLevel, Vector3i::ZERO);
}

boost::shared_ptr<std::deque<coords::Chunk> >
volumeSystem::GetMipChunkCoords() const
{
    std::deque<coords::Chunk>* coords = new std::deque<coords::Chunk>();

    for(int level = 0; level <= mMipRootLevel; ++level) {
        addChunkCoordsForLevel(level, coords);
    }

    return boost::shared_ptr<std::deque<coords::Chunk> >(coords);
}

boost::shared_ptr<std::deque<coords::Chunk> >
volumeSystem::GetMipChunkCoords(const int mipLevel) const
{
    std::deque<coords::Chunk>* coords = new std::deque<coords::Chunk>();

    addChunkCoordsForLevel(mipLevel, coords);

    return boost::shared_ptr<std::deque<coords::Chunk> >(coords);
}

void volumeSystem::addChunkCoordsForLevel(const int mipLevel, std::deque<coords::Chunk>* coords) const
{
    const Vector3i dims = MipLevelDimensionsInMipChunks(mipLevel);
    for (int z = 0; z < dims.z; ++z){
        for (int y = 0; y < dims.y; ++y){
            for (int x = 0; x < dims.x; ++x){
                coords->push_back(coords::Chunk(mipLevel, x, y, z));
            }
        }
    }
}

} // namespace coords
} // namespace om
