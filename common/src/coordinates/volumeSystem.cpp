#include "common/common.h"
#include "zi/concurrency.hpp"
#include "coordinates/volumeSystem.h"

namespace om {
namespace coords {

VolumeSystem::VolumeSystem()
    : dataToGlobal_(Matrix4f::IDENTITY)
    , globalToData_(Matrix4f::IDENTITY)
    , normToGlobal_(Matrix4f::IDENTITY)
    , globalToNorm_(Matrix4f::IDENTITY)
    , chunkDimensions_(Vector3i(DefaultChunkDim))
    , rootMipLevel_(0)
{
    SetDataDimensions(Vector3i(DefaultChunkDim));

    UpdateRootLevel();
}

void VolumeSystem::UpdateRootLevel()
{
    float ratio = (Vector3f(DataDimensions()) / chunkDimensions_).getMaxComponent();
    if (ratio <= 1) {
        rootMipLevel_ = 0;
    } else {
        rootMipLevel_ = ceil(log(ratio) / log(2.0f));
    }
}

GlobalBbox VolumeSystem::Extent() const
{
    Vector3f abs = AbsOffset();
    return GlobalBbox(abs, abs - Vector3f::ONE + DataDimensions() * Resolution() );
}

bool VolumeSystem::ContainsMipChunk(const Chunk & rMipCoord) const
{
    //if level is greater than root level
    if(rMipCoord.mipLevel() < 0 ||
        rMipCoord.mipLevel() > RootMipLevel()){
        return false;
    }

    //convert to data box in leaf (MIP 0)
    GlobalBbox bbox = rMipCoord.BoundingBox(this).ToGlobalBbox();

    bbox.intersect(Extent());
    if(bbox.isEmpty()){
        return false;
    }

    //else valid mip coord
    return true;
}

Chunk VolumeSystem::RootMipChunkCoordinate() const {
    return Chunk(rootMipLevel_, Vector3i::ZERO);
}

boost::shared_ptr<std::vector<coords::Chunk> >
VolumeSystem::MipChunkCoords() const
{
    std::vector<coords::Chunk>* coords = new std::vector<coords::Chunk>();

    for(int level = 0; level <= rootMipLevel_; ++level) {
        addChunkCoordsForLevel(level, coords);
    }

    return boost::shared_ptr<std::vector<coords::Chunk> >(coords);
}

boost::shared_ptr<std::vector<coords::Chunk> >
VolumeSystem::MipChunkCoords(const int mipLevel) const
{
    std::vector<coords::Chunk>* coords = new std::vector<coords::Chunk>();

    addChunkCoordsForLevel(mipLevel, coords);

    return boost::shared_ptr<std::vector<coords::Chunk> >(coords);
}

void VolumeSystem::addChunkCoordsForLevel(const int mipLevel, std::vector<coords::Chunk>* coords) const
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
