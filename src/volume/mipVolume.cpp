#include "volume/mipVolume.h"
#include "volume/io/volumeData.h"

using namespace om::coords;

namespace om {
namespace volume {

volume::volume()
    : mVolDataType(volDataType::UNKNOWN)
    , coords_(this)
    , mBuildState(MIPVOL_UNBUILT)
{}

bool volume::ContainsVoxel(const globalCoord & vox){
    return coords_.GetDataExtent().contains(vox);
}

void volume::addChunkCoordsForLevel(const int mipLevel,
                                         std::deque<chunkCoord>* coords) const
{
    const Vector3i dims = coords_.MipLevelDataDimensions(mipLevel);
    
    for (int z = 0; z < dims.z; ++z){
        for (int y = 0; y < dims.y; ++y){
            for (int x = 0; x < dims.x; ++x){
                coords->push_back(chunkCoord(0, Vector3i(x, y, z)));
            }
        }
    }
}

boost::shared_ptr<std::deque<chunkCoord> >
volume::GetMipChunkCoords() const
{
    std::deque<chunkCoord>* coords = new std::deque<chunkCoord>();

    for(int level = 0; level <= coords_.GetRootMipLevel(); ++level) {
        addChunkCoordsForLevel(level, coords);
    }

    return coords::shared_ptr<std::deque<chunkCoord> >(coords);
}

boost::shared_ptr<std::deque<coords::chunkCoord> >
volume::GetMipChunkCoords(const int mipLevel) const
{
    std::deque<chunkCoord>* coords = new std::deque<chunkCoord>();

    addChunkCoordsForLevel(mipLevel, coords);

    return shared_ptr<std::deque<coords::chunkCoord> >(coords);
}


} // namespace volume
} // namespace om