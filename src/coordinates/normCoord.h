#pragma once

#include "vmmlib/vmmlib.h"

namespace om {
namespace coords {

class dataCoord;
class globalCoord;
class volumeSystem;
    
class normCoord : public vmml::Vector3f 
{
private:
    typedef vmml::Vector3f base_t;
    const volumeSystem * vol_;
    
public:
    normCoord(base_t v, const volumeSystem *vol) 
        : base_t(v) 
        , vol_(vol)
    { }
    
    normCoord(float x, float y, float z, const volumeSystem *vol) 
        : base_t(x, y, z) 
        , vol_(vol)
    { }
    
    globalCoord toGlobalCoord() const;
    dataCoord toDataCoord(int) const;
    
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
    normBbox(normCoord min, normCoord max);
    
    inline normCoord getMin() const {
        return normCoord(base_t::getMin(), vol_);
    }
    
    inline normCoord getMax() const {
        return normCoord(base_t::getMax(), vol_);
    }
    
    inline normCoord getDimensions() const {
        return normCoord(base_t::getDimensions(), vol_);
    }
    
    inline normCoord getUnitDimensions() const {
        return normCoord(base_t::getUnitDimensions(), vol_);
    }
    
    inline normCoord getCenter() const {
        return normCoord(base_t::getCenter(), vol_);
    }
};

} // namespace coords
} // namespace om