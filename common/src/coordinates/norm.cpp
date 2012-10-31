#include "common/common.h"

namespace om {
namespace coords {

Global Norm::ToGlobal() const
{
    const vmml::Vector4f Norm(x, y, z, 1);
    vmml::Vector3f global = vol_->NormToGlobalMat() * Norm;
    return global;
}

data Norm::ToData(int mipLevel) const
{
    return ToGlobal().ToData(vol_, mipLevel);
}

NormBbox::NormBbox(Norm min, Norm max)
    : base_t(min, max)
    , vol_(min.volume())
{
    if(min.volume() != max.volume()) {
        throw argException("min and max coords come from different volumes");
    }
}

} // namespace coords
} // namespace om