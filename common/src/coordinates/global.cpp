#include "common/common.h"
#include "server_types.h"

namespace om {
namespace coords {

global::global(server::vector3d v) {
  x = v.x;
  y = v.y;
  z = v.z;
}

screen global::toScreen(screenSystem *state) const {
  const vmml::Vector4f global(x, y, z, 1);
  vmml::Vector3f screenC = state->GlobalToScreenMat() * global;
  return screen(vmml::Vector2i(screenC.x, screenC.y), state);
}

data global::toData(const volumeSystem *vol, const int mipLevel) const {
  const vmml::Vector4f global(x, y, z, 1);
  vmml::Vector3f dataC = vol->GlobalToDataMat(mipLevel) * global;
  return data(dataC, vol, mipLevel);
}

chunk global::toChunk(const volumeSystem *vol, const int mipLevel) const {
  return toData(vol, mipLevel).toChunk();
}

norm global::toNorm(const volumeSystem *vol) const {
  const vmml::Vector4f global(x, y, z, 1);
  vmml::Vector3f normC = vol->GlobalToNormMat() * global;
  return norm(normC, vol);
}

global global::fromOffsetCoords(Vector3i vec, const volumeSystem *vol) {
  return global(vec - vol->GetAbsOffset());
}

Vector3i global::withAbsOffset(const volumeSystem *vol) const {
  return *this + vol->GetAbsOffset();
}

global::operator server::vector3d() const {
  server::vector3d out;
  out.x = x;
  out.y = y;
  out.z = z;
  return out;
}

normBbox globalBbox::toNormBbox(const volumeSystem *vol) const {
  global min = _min;
  global max = _max;

  return normBbox(min.toNorm(vol), max.toNorm(vol));
}

dataBbox globalBbox::toDataBbox(const volumeSystem *vol, int mipLevel) const {
  global min = _min;
  global max = _max;

  return dataBbox(min.toData(vol, mipLevel), max.toData(vol, mipLevel));
}

globalBbox::operator server::bbox() const {
  server::bbox out;

  out.min = global(_min);
  out.max = global(_max);
  return out;
}

}  // namespace coords
}  // namespace om
