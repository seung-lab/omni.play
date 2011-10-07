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


} // namespace volume
} // namespace om