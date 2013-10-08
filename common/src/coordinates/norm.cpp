#include "common/common.h"

namespace om {
namespace coords {

global norm::toGlobal() const {
  const vmml::Vector4f norm(x, y, z, 1);
  vmml::Vector3f global = vol_->NormToGlobalMat() * norm;
  return global;
}

data norm::toData(int mipLevel) const {
  return toGlobal().toData(vol_, mipLevel);
}

normBbox::normBbox(norm min, norm max) : base_t(min, max), vol_(min.volume()) {
  if (min.volume() != max.volume()) {
    throw argException("min and max coords come from different volumes");
  }
}

}  // namespace coords
}  // namespace om