#include "common.h"

namespace om {
namespace coords {
    
globalCoord normCoord::toGlobalCoord() const
{
    const vmml::Vector4f norm(x, y, z, 1);
    vmml::Vector3f global = vol_->NormToGlobalMat() * norm;
    return global;
}

dataCoord normCoord::toDataCoord(int mipLevel) const
{
    return toGlobalCoord().toDataCoord(vol_, mipLevel);
}

normBbox::normBbox(normCoord min, normCoord max)
    : base_t(min, max)
    , vol_(min.volume())
{ 
    if(min.volume() != max.volume()) {
        throw common::argException("min and max coords come from different volumes");
    }
}

} // namespace coords
} // namespace om