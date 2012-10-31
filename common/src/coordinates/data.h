#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

class OmMipVolume;

namespace om {
namespace coords {

class Global;
class Norm;
class Chunk;
class GlobalBbox;
class NormBbox;
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

    Global toGlobal() const;
    Norm toNorm() const;
    Chunk toChunk() const;
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

    inline data operator+(const Vector3i b) const {
        return data(base_t::operator+(b), vol_, mipLevel_);
    }

    inline data operator*(const int scalar) const {
        return data(base_t::operator*(scalar), vol_, mipLevel_);
    }

    inline data operator/(const int scalar) const {
        return data(base_t::operator/(scalar), vol_, mipLevel_);
    }

    inline data operator/(const Vector3i b) const {
        return data(base_t::operator/(b), vol_, mipLevel_);
    }

    inline bool operator==(const data& other) const {
    	return base_t::operator==(other) && vol_ == other.vol_ && mipLevel_ == other.mipLevel_;
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

    GlobalBbox toGlobalBbox() const;
    NormBbox toNormBbox() const;

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

    inline bool operator==(const dataBbox& other) const {
    	return base_t::operator==(other) && vol_ == other.vol_ && mipLevel_ == other.mipLevel_;
    }
};

} // namespace coords
} // namespace om
