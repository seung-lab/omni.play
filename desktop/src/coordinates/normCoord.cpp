#include "common/common.h"
#include "volume/omMipVolume.h"
#include "globalCoord.h"
#include "normCoord.h"

namespace om {

globalCoord normCoord::toGlobalCoord() const {
  const vmml::Vector4f norm(x, y, z, 1);
  vmml::Vector3f global = vol_->Coords().NormToGlobalMat() * norm;
  return global;
}

dataCoord normCoord::toDataCoord(int mipLevel) const {
  return toGlobalCoord().toDataCoord(vol_, mipLevel);
}

normBbox::normBbox(normCoord min, normCoord max)
    : base_t(min, max), vol_(min.volume()) {
  if (min.volume() != max.volume()) {
    throw om::ArgException("min and max coords come from different volumes");
  }
}

}  // namespace om
