#pragma once

#include "vmmlib/vmmlib.h"
#include "common/macro.hpp"

namespace om {
namespace coords {

class Data;
class Global;
class volumeSystem;

class Norm : public vmml::Vector3f
{
private:
    typedef vmml::Vector3f base_t;

public:
    Norm(base_t v, const volumeSystem *volume)
        : base_t(v)
        , volume_(volume)
    { }

    Norm(float x, float y, float z, const volumeSystem *volume)
        : base_t(x, y, z)
        , volume_(volume)
    { }

    Global ToGlobal() const;
    Data ToData(int) const;

private:
	PROP_CONST_PTR(volumeSystem, volume);
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