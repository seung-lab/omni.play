#include "common/common.h"

namespace om {
namespace coords {

Global Norm::ToGlobal() const
{
    const vmml::Vector4f Norm(x, y, z, 1);
    vmml::Vector3f global = volume_->NormToGlobalMat() * Norm;
    return global;
}

Data Norm::ToData(int mipLevel) const
{
    return ToGlobal().ToData(volume_, mipLevel);
}

NormBbox::NormBbox(Norm min, Norm max)
    : base_t(min, max)
    , vol_(min.volume())
{
    if(min.volume() != max.volume()) {
        throw ArgException("min and max coords come from different volumes");
    }
}

} // namespace coords
} // namespace om