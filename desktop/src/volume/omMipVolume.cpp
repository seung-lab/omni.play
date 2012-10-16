#include "volume/omMipVolume.h"
#include "volume/io/omVolumeData.h"

OmMipVolume::OmMipVolume()
    : mVolDataType(OmVolDataType::UNKNOWN)
    , coords_(this)
    , mBuildState(MIPVOL_UNBUILT)
{}

bool OmMipVolume::ContainsVoxel(const om::globalCoord & vox){
    return coords_.GetExtent().contains(vox);
}

void OmMipVolume::addChunkCoordsForLevel(const int mipLevel,
                                         std::deque<om::chunkCoord>* coords) const
{
    const Vector3i dims = coords_.MipLevelDimensionsInMipChunks(mipLevel);

    for (int z = 0; z < dims.z; ++z){
        for (int y = 0; y < dims.y; ++y){
            for (int x = 0; x < dims.x; ++x){
                coords->push_back(om::chunkCoord(mipLevel, Vector3i(x, y, z)));
            }
        }
    }
}

om::shared_ptr<std::deque<om::chunkCoord> >
OmMipVolume::GetMipChunkCoords() const
{
    std::deque<om::chunkCoord>* coords = new std::deque<om::chunkCoord>();

    for(int level = 0; level <= coords_.GetRootMipLevel(); ++level) {
        addChunkCoordsForLevel(level, coords);
    }

    return om::shared_ptr<std::deque<om::chunkCoord> >(coords);
}

om::shared_ptr<std::deque<om::chunkCoord> >
OmMipVolume::GetMipChunkCoords(const int mipLevel) const
{
    std::deque<om::chunkCoord>* coords = new std::deque<om::chunkCoord>();

    addChunkCoordsForLevel(mipLevel, coords);

    return om::shared_ptr<std::deque<om::chunkCoord> >(coords);
}


