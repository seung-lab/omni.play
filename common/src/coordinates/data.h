#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

class OmMipVolume;

namespace om {
namespace coords {

class global;
class norm;
class chunk;
class globalBbox;
class normBbox;
class volumeSystem;

class data : public vmml::Vector3i
{
private:
    typedef Vector3i base_t;
    const volumeSystem *vol_;
    int mipLevel_;

public:
    data(base_t v, const volumeSystem * vol, int mipLevel);
    data(int x, int y, int z, const volumeSystem * vol, int mipLevel);

    global toGlobal() const;
    norm toNorm() const;
    chunk toChunk() const;
    Vector3i toChunkVec() const;
    int toChunkOffset() const;
    int toTileOffset(om::common::viewType) const;
    int toTileDepth(om::common::viewType) const;
    bool isInVolume() const;

    inline int level() const {
        return mipLevel_;
    }

    data atDifferentLevel(int) const;

    inline volumeSystem const * volume() const {
        return vol_;
    }

    data operator+(const Vector3i b) const {
        return data(base_t::operator+(b), vol_, mipLevel_);
    }

    data operator*(const int scalar) const {
        return data(base_t::operator*(scalar), vol_, mipLevel_);
    }

    data operator/(const int scalar) const {
        return data(base_t::operator/(scalar), vol_, mipLevel_);
    }

    data operator/(const Vector3i b) const {
        return data(base_t::operator/(b), vol_, mipLevel_);
    }
};

class dataBbox : public vmml::AxisAlignedBoundingBox<int>
{
private:
    typedef AxisAlignedBoundingBox<int> base_t;
    const volumeSystem * vol_;
    const int mipLevel_;

public:
    dataBbox(data min, data max);
    dataBbox(const volumeSystem * vol, int level);
    dataBbox(base_t bbox, const volumeSystem * vol, int level);

    inline data getMin() const {
        return data(base_t::getMin(), vol_, mipLevel_);
    }

    inline data getMax() const {
        return data(base_t::getMax(), vol_, mipLevel_);
    }

    inline data getDimensions() const {
        return data(base_t::getDimensions(), vol_, mipLevel_);
    }

    inline data getUnitDimensions() const {
        return data(base_t::getUnitDimensions(), vol_, mipLevel_);
    }

    inline data getCenter() const {
        return data(base_t::getCenter(), vol_, mipLevel_);
    }

    inline void intersect(const dataBbox& other) {
        base_t::intersect(other);
    }

    globalBbox toGlobalBbox() const;
    normBbox toNormBbox() const;

    inline int level() {
        return mipLevel_;
    }

    dataBbox atDifferentLevel(int) const;

    inline volumeSystem const * volume() const {
        return vol_;
    }

    inline dataBbox& operator=(const dataBbox& other) {
        base_t::operator=(other);
        return *this;
    }
};

} // namespace coords
} // namespace om
