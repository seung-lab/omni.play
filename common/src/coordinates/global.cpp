#include "common/common.h"
#include "coordinates/coordinates.h"

namespace om {
namespace coords {

Screen Global::ToScreen(ScreenSystem &state) const {
  const vmml::Vector4f Global(x, y, z, 1);
  vmml::Vector3f screenC = state.globalToScreenMat() * Global;
  return Screen(vmml::Vector2i(screenC.x, screenC.y), state);
}

Data Global::ToData(const VolumeSystem &vol, const int mipLevel) const {
  const vmml::Vector4f Global(x, y, z, 1);
  vmml::Vector3f dataC = vol.GlobalToDataMat(mipLevel) * Global;
  return Data(dataC, vol, mipLevel);
}

Chunk Global::ToChunk(const VolumeSystem &vol, const int mipLevel) const {
  return ToData(vol, mipLevel).ToChunk();
}

Norm Global::ToNorm(const VolumeSystem &vol) const {
  const vmml::Vector4f Global(x, y, z, 1);
  vmml::Vector3f normC = vol.GlobalToNormMat() * Global;
  return Norm(normC, vol);
}

Global Global::FromOffsetCoords(Vector3i vec, const VolumeSystem &vol) {
  return Global(vec - vol.AbsOffset());
}

Vector3i Global::WithAbsOffset(const VolumeSystem &vol) const {
  return *this + vol.AbsOffset();
}

NormBbox GlobalBbox::ToNormBbox(const VolumeSystem &vol) const {
  Global min = _min;
  Global max = _max;

  return NormBbox(min.ToNorm(vol), max.ToNorm(vol));
}

DataBbox GlobalBbox::ToDataBbox(const VolumeSystem &vol, int mipLevel) const {
  Global min = _min;
  Global max = _max;

  return DataBbox(min.ToData(vol, mipLevel), max.ToData(vol, mipLevel));
}

}  // namespace coords
}  // namespace om
