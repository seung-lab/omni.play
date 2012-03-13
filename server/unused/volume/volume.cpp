#include "volume/volume.h"
#include "volume/io/volumeData.h"

using namespace om::coords;

namespace om {
namespace volume {

volume::volume()
    : mVolDataType(dataType::UNKNOWN)
    , coords_()
    , mBuildState(MIPVOL_UNBUILT)
{}

bool volume::ContainsVoxel(const globalCoord & vox){
    return coords_.GetDataExtent().contains(vox);
}


} // namespace volume
} // namespace om
