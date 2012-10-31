#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

#include "common/macro.hpp"

class OmMipVolume;

namespace om {
namespace coords {

class Global;
class Norm;
class Chunk;
class GlobalBbox;
class NormBbox;
class volumeSystem;

class Data : public vmml::Vector3i
{
private:
    typedef Vector3i base_t;

public:
    Data(base_t v, const volumeSystem * vol, int mipLevel);
    Data(int x, int y, int z, const volumeSystem * vol, int mipLevel);

    Global ToGlobal() const;
    Norm ToNorm() const;
    Chunk ToChunk() const;
    Vector3i ToChunkVec() const;
    int ToChunkOffset() const;
    int ToTileOffset(common::viewType) const;
    int ToTileDepth(common::viewType) const;
    bool IsInVolume() const;
    Data AtDifferentLevel(int) const;

    inline Data operator+(const Vector3i b) const {
        return Data(base_t::operator+(b), volume_, mipLevel_);
    }

    inline Data operator*(const int scalar) const {
        return Data(base_t::operator*(scalar), volume_, mipLevel_);
    }

    inline Data operator/(const int scalar) const {
        return Data(base_t::operator/(scalar), volume_, mipLevel_);
    }

    inline Data operator/(const Vector3i b) const {
        return Data(base_t::operator/(b), volume_, mipLevel_);
    }

    inline bool operator==(const Data& other) const {
    	return base_t::operator==(other) && volume_ == other.volume_ && mipLevel_ == other.mipLevel_;
    }
private:
	PROP_CONST_PTR(volumeSystem, volume);
	PROP_CONST_REF(int, mipLevel);
};

class DataBbox : public vmml::AxisAlignedBoundingBox<int>
{
private:
    typedef AxisAlignedBoundingBox<int> base_t;

public:
    DataBbox(Data min, Data max);
    DataBbox(const volumeSystem * vol, int level);
    DataBbox(base_t bbox, const volumeSystem * vol, int level);

    inline Data getMin() const {
        return Data(base_t::getMin(), volume_, mipLevel_);
    }

    inline Data getMax() const {
        return Data(base_t::getMax(), volume_, mipLevel_);
    }

    inline Data getDimensions() const {
        return Data(base_t::getDimensions(), volume_, mipLevel_);
    }

    inline Data getUnitDimensions() const {
        return Data(base_t::getUnitDimensions(), volume_, mipLevel_);
    }

    inline Data getCenter() const {
        return Data(base_t::getCenter(), volume_, mipLevel_);
    }

    inline void intersect(const DataBbox& other) {
        base_t::intersect(other);
    }

    GlobalBbox ToGlobalBbox() const;
    NormBbox ToNormBbox() const;
    DataBbox AtDifferentLevel(int) const;

    inline DataBbox& operator=(const DataBbox& other) {
        base_t::operator=(other);
        return *this;
    }

    inline bool operator==(const DataBbox& other) const {
    	return base_t::operator==(other) && volume_ == other.volume_ && mipLevel_ == other.mipLevel_;
    }
private:
	PROP_CONST_PTR(volumeSystem, volume);
	PROP_CONST_REF(int, mipLevel);
};

} // namespace coords
} // namespace om
