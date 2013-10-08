#include "common/omCommon.h"
#include "view2d/omView2dState.hpp"
#include "volume/omMipVolume.h"

namespace om {

int round(float f) {
  return std::floor(f + 0.001f);  // Fix small rounding errors.
}

screenCoord globalCoord::toScreenCoord(OmView2dState *state) const {
  const vmml::Vector4f global(x, y, z, 1);
  vmml::Vector3f screen = state->Coords().GlobalToScreenMat() * global;
  return screenCoord(round(screen.x), round(screen.y), state);
}

dataCoord globalCoord::toDataCoord(const OmMipVolume *vol, int mipLevel) const {
  const vmml::Vector4f global(x, y, z, 1);
  vmml::Vector3f data = vol->Coords().GlobalToDataMat(mipLevel) * global;
  return dataCoord(round(data.x), round(data.y), round(data.z), vol, mipLevel);
}

chunkCoord globalCoord::toChunkCoord(const OmMipVolume *vol,
                                     int mipLevel) const {
  return toDataCoord(vol, mipLevel).toChunkCoord();
}

normCoord globalCoord::toNormCoord(const OmMipVolume *vol) const {
  const vmml::Vector4f global(x, y, z, 1);
  vmml::Vector3f norm = vol->Coords().GlobalToNormMat() * global;
  return normCoord(norm, vol);
}

globalCoord globalCoord::fromOffsetCoords(Vector3i vec,
                                          const OmMipVolume *vol) {
  return globalCoord(vec - vol->Coords().GetAbsOffset());
}

Vector3i globalCoord::withAbsOffset(const OmMipVolume *vol) const {
  return *this + vol->Coords().GetAbsOffset();
}

normBbox globalBbox::toNormBbox(const OmMipVolume *vol) const {
  globalCoord min = _min;
  globalCoord max = _max;

  return normBbox(min.toNormCoord(vol), max.toNormCoord(vol));
}

dataBbox globalBbox::toDataBbox(const OmMipVolume *vol, int mipLevel) const {
  globalCoord min = _min;
  globalCoord max = _max;

  return dataBbox(min.toDataCoord(vol, mipLevel),
                  max.toDataCoord(vol, mipLevel));
}

}  // namespace om