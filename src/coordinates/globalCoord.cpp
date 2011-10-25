#include "common/common.h"

namespace om {
namespace coords {

screenCoord globalCoord::toScreenCoord(screenSystem *state) const
{
    const vmml::Vector4f global(x, y, z, 1);
    vmml::Vector3f screen = state->GlobalToScreenMat() * global;
    return screenCoord(vmml::Vector2i(screen.x, screen.y), state);
}

dataCoord globalCoord::toDataCoord(const volumeSystem *vol, const int mipLevel) const
{
    const vmml::Vector4f global(x, y, z, 1);
    vmml::Vector3f data = vol->GlobalToDataMat(mipLevel) * global;
    return dataCoord(data, vol, mipLevel);
}

chunkCoord globalCoord::toChunkCoord(const volumeSystem *vol, const int mipLevel) const
{
    return toDataCoord(vol, mipLevel).toChunkCoord();
}


normCoord globalCoord::toNormCoord(const volumeSystem *vol) const
{
    const vmml::Vector4f global(x, y, z, 1);
    vmml::Vector3f norm = vol->GlobalToNormMat() * global;
    return normCoord(norm, vol);
}

globalCoord globalCoord::fromOffsetCoords(Vector3i vec, const volumeSystem * vol)
{
    return globalCoord(vec - vol->GetAbsOffset());
}

Vector3i globalCoord::withAbsOffset(const volumeSystem * vol) const
{
    return *this + vol->GetAbsOffset();
}


normBbox globalBbox::toNormBbox(const volumeSystem *vol) const
{
    globalCoord min = _min;
    globalCoord max = _max;

    return normBbox(min.toNormCoord(vol), max.toNormCoord(vol));
}

dataBbox globalBbox::toDataBbox(const volumeSystem *vol, int mipLevel) const
{
    globalCoord min = _min;
    globalCoord max = _max;

    return dataBbox(min.toDataCoord(vol, mipLevel), max.toDataCoord(vol, mipLevel));
}

} // namespace coords
} // namespace om
