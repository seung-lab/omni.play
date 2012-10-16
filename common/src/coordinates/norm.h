#pragma once

#include "vmmlib/vmmlib.h"

namespace om {
namespace coords {

class data;
class global;
class volumeSystem;

class norm : public vmml::Vector3f
{
private:
    typedef vmml::Vector3f base_t;
    const volumeSystem * vol_;

public:
    norm(base_t v, const volumeSystem *vol)
        : base_t(v)
        , vol_(vol)
    { }

    norm(float x, float y, float z, const volumeSystem *vol)
        : base_t(x, y, z)
        , vol_(vol)
    { }

    global toGlobal() const;
    data toData(int) const;

    inline const volumeSystem* volume() const {
        return vol_;
    }
};

class normBbox : public vmml::AxisAlignedBoundingBox<float>
{
private:
    typedef AxisAlignedBoundingBox<float> base_t;
    volumeSystem const * const vol_;

public:
    normBbox(norm min, norm max);

    inline norm getMin() const {
        return norm(base_t::getMin(), vol_);
    }

    inline norm getMax() const {
        return norm(base_t::getMax(), vol_);
    }

    inline norm getDimensions() const {
        return norm(base_t::getDimensions(), vol_);
    }

    inline norm getUnitDimensions() const {
        return norm(base_t::getUnitDimensions(), vol_);
    }

    inline norm getCenter() const {
        return norm(base_t::getCenter(), vol_);
    }
};

} // namespace coords
} // namespace om