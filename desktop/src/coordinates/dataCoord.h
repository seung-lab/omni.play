#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

class OmMipVolume;



namespace om {

class globalCoord;
class normCoord;
class chunkCoord;
class globalBbox;
class normBbox;

class dataCoord : public vmml::Vector3i
{
private:
    typedef Vector3i base_t;
    const OmMipVolume *vol_;
    int mipLevel_;

public:
    dataCoord(base_t v, const OmMipVolume * vol, int mipLevel);
    dataCoord(int x, int y, int z, const OmMipVolume * vol, int mipLevel);

    globalCoord toGlobalCoord() const;
    normCoord toNormCoord() const;
    chunkCoord toChunkCoord() const;
    Vector3i toChunkVec() const;
    int toChunkOffset() const;
    int toTileOffset(ViewType) const;
    int toTileDepth(ViewType) const;

    inline int level() const {
        return mipLevel_;
    }

    dataCoord atDifferentLevel(int) const;

    inline OmMipVolume const * volume() const {
        return vol_;
    }

    dataCoord operator+(const Vector3i b) const {
        return dataCoord(base_t::operator+(b), vol_, mipLevel_);
    }

    dataCoord operator*(const int scalar) const {
        return dataCoord(base_t::operator*(scalar), vol_, mipLevel_);
    }

    dataCoord operator/(const int scalar) const {
        return dataCoord(base_t::operator/(scalar), vol_, mipLevel_);
    }

    dataCoord operator/(const Vector3i b) const {
        return dataCoord(base_t::operator/(b), vol_, mipLevel_);
    }
};

class dataBbox : public vmml::AxisAlignedBoundingBox<int>
{
private:
    typedef AxisAlignedBoundingBox<int> base_t;
    const OmMipVolume * vol_;
    const int mipLevel_;

public:
    dataBbox(dataCoord min, dataCoord max);
    dataBbox(const OmMipVolume * vol, int level);
    dataBbox(base_t bbox, const OmMipVolume * vol, int level);

    inline dataCoord getMin() const {
        return dataCoord(base_t::getMin(), vol_, mipLevel_);
    }

    inline dataCoord getMax() const {
        return dataCoord(base_t::getMax(), vol_, mipLevel_);
    }

    inline dataCoord getDimensions() const {
        return dataCoord(base_t::getDimensions(), vol_, mipLevel_);
    }

    inline dataCoord getUnitDimensions() const {
        return dataCoord(base_t::getUnitDimensions(), vol_, mipLevel_);
    }

    inline dataCoord getCenter() const {
        return dataCoord(base_t::getCenter(), vol_, mipLevel_);
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

    inline OmMipVolume const * volume() const {
        return vol_;
    }

    inline dataBbox& operator=(const dataBbox& other) {
        base_t::operator=(other);
        return *this;
    }
};

} // namespace om