#include "common/common.h"
#include "server_types.h"

namespace om {
namespace coords {

Global::Global(server::vector3d v) {
    x = v.x;
    y = v.y;
    z = v.z;
}

screen Global::ToScreen(screenSystem *state) const
{
    const vmml::Vector4f Global(x, y, z, 1);
    vmml::Vector3f screenC = state->GlobalToScreenMat() * Global;
    return screen(vmml::Vector2i(screenC.x, screenC.y), state);
}

data Global::ToData(const volumeSystem *vol, const int mipLevel) const
{
    const vmml::Vector4f Global(x, y, z, 1);
    vmml::Vector3f dataC = vol->GlobalToDataMat(mipLevel) * Global;
    return data(dataC, vol, mipLevel);
}

Chunk Global::ToChunk(const volumeSystem *vol, const int mipLevel) const
{
    return ToData(vol, mipLevel).toChunk();
}

Norm Global::ToNorm(const volumeSystem *vol) const
{
    const vmml::Vector4f Global(x, y, z, 1);
    vmml::Vector3f normC = vol->GlobalToNormMat() * Global;
    return Norm(normC, vol);
}

Global Global::FromOffsetCoords(Vector3i vec, const volumeSystem * vol)
{
    return Global(vec - vol->GetAbsOffset());
}

Vector3i Global::WithAbsOffset(const volumeSystem * vol) const
{
    return *this + vol->GetAbsOffset();
}

Global::operator server::vector3d () const {
    server::vector3d out;
    out.x = x;
    out.y = y;
    out.z = z;
    return out;
}

NormBbox GlobalBbox::ToNormBbox(const volumeSystem *vol) const
{
    Global min = _min;
    Global max = _max;

    return NormBbox(min.ToNorm(vol), max.ToNorm(vol));
}

dataBbox GlobalBbox::ToDataBbox(const volumeSystem *vol, int mipLevel) const
{
    Global min = _min;
    Global max = _max;

    return dataBbox(min.ToData(vol, mipLevel), max.ToData(vol, mipLevel));
}

GlobalBbox::operator server::bbox() const{
    server::bbox out;

    out.min = Global(_min);
    out.max = Global(_max);
    return out;
}

} // namespace coords
} // namespace om
