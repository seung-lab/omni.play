#include "common.h"
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
    , mMipLeafDim(0)
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

globalBbox volumeSystem::GetDataExtent() const
{
    Vector3f abs = GetAbsOffset();
    return globalBbox(abs, abs - Vector3f::ONE + dataDimensions_);
}

bool volumeSystem::ContainsMipChunkCoord(const chunkCoord & rMipCoord) const
{
    //if level is greater than root level
    if(rMipCoord.Level < 0 ||
        rMipCoord.Level > GetRootMipLevel()){
        return false;
    }
    
    //convert to data box in leaf (MIP 0)
    globalBbox bbox = rMipCoord.chunkBoundingBox(this).toGlobalBbox();
    
    bbox.intersect(GetDataExtent());
    if(bbox.isEmpty()){
        return false;
    }
    
    //else valid mip coord
    return true;
}

chunkCoord volumeSystem::RootMipChunkCoordinate() const {
    return chunkCoord(mMipRootLevel, Vector3i::ZERO);
}

} // namespace coords
} // namespace om