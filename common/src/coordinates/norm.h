#pragma once

#include "vmmlib/vmmlib.h"

namespace om {
namespace coords {

class data;
class Global;
class volumeSystem;

class Norm : public vmml::Vector3f
{
private:
    typedef vmml::Vector3f base_t;
    const volumeSystem * vol_;

public:
    Norm(base_t v, const volumeSystem *vol)
        : base_t(v)
        , vol_(vol)
    { }

    Norm(float x, float y, float z, const volumeSystem *vol)
        : base_t(x, y, z)
        , vol_(vol)
    { }

    Global ToGlobal() const;
    data ToData(int) const;

    inline const volumeSystem* volume() const {
        return vol_;
    }
};

class NormBbox : public vmml::AxisAlignedBoundingBox<float>
{
private:
    typedef AxisAlignedBoundingBox<float> base_t;
    volumeSystem const * const vol_;

public:
    NormBbox(Norm min, Norm max);

    inline Norm getMin() const {
        return Norm(base_t::getMin(), vol_);
    }

    inline Norm getMax() const {
        return Norm(base_t::getMax(), vol_);
    }

    inline Norm getDimensions() const {
        return Norm(base_t::getDimensions(), vol_);
    }

    inline Norm getUnitDimensions() const {
        return Norm(base_t::getUnitDimensions(), vol_);
    }

    inline Norm getCenter() const {
        return Norm(base_t::getCenter(), vol_);
    }
};

} // namespace coords
} // namespace om