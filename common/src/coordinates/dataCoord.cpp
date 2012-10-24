#include "common/common.h"
#include "common/exception.h"

using namespace om::common;

namespace om {
namespace coords {

data::data(data::base_t v, const volumeSystem* vol, int mipLevel)
    : base_t(v)
    , vol_(vol)
    , mipLevel_(mipLevel)
{
    if (mipLevel > vol_->GetRootMipLevel()) {
        throw argException("Invalid Mip level.");
    }
}

data::data(int x, int y, int z, const volumeSystem* vol, int mipLevel)
    : base_t(x, y, z)
    , vol_(vol)
    , mipLevel_(mipLevel)
{
    if (mipLevel > vol_->GetRootMipLevel()) {
        throw argException("Invalid Mip level.");
    }
}


global data::toGlobal() const
{
    const vmml::Vector4f data(x, y, z, 1);
    vmml::Vector3f global = vol_->DataToGlobalMat(mipLevel_) * data;
    return global;
}

norm data::toNorm() const
{
    return toGlobal().toNorm(vol_);
}

chunk data::toChunk() const
{
    return chunk(mipLevel_, *this / vol_->GetChunkDimensions());
}

Vector3i data::toChunkVec() const
{
    Vector3i dims = vol_->GetChunkDimensions();
    return Vector3i(x % dims.x, y % dims.y, z % dims.z);
}

int data::toChunkOffset() const
{
    const Vector3i dims = vol_->GetChunkDimensions();
    const Vector3i chunkVec = toChunkVec();
    return dims.x * dims.y * chunkVec.z + dims.x * chunkVec.y + chunkVec.x;
}

int data::toTileOffset (viewType viewType) const
{
    const Vector3i dims = vol_->GetChunkDimensions();
    const Vector3i chunkVec = toChunkVec();

    switch(viewType)
    {
    case XY_VIEW:
        return dims.x * chunkVec.y + chunkVec.x;
    case XZ_VIEW:
        return dims.x * chunkVec.z + chunkVec.x;
    case ZY_VIEW:
        return dims.z * chunkVec.y + chunkVec.z;
    }

    return -1;
}

int data::toTileDepth (viewType viewType) const
{
    const Vector3i dims = vol_->GetChunkDimensions();
    const Vector3i chunkVec = toChunkVec();

    switch(viewType)
    {
    case XY_VIEW: return chunkVec.z;
    case XZ_VIEW: return chunkVec.y;
    case ZY_VIEW: return chunkVec.x;
    }

    return -1;
}

bool data::isInVolume() const {
	return vol_->GetDataExtent().contains(toGlobal());
}

data data::atDifferentLevel(int newLevel) const
{
    if (newLevel > vol_->GetRootMipLevel()) {
        throw argException("Invalid Mip level.");
    }

    if(newLevel == mipLevel_) {
        return *this;
    }

    Vector3f result = *this * om::math::pow2int(mipLevel_) / om::math::pow2int(newLevel);
    return data(result, vol_, newLevel);
}

dataBbox::dataBbox(data min, data max)
    : base_t(min, max)
    , vol_(min.volume())
    , mipLevel_(min.level())
{
    if(min.volume() != max.volume()) {
        throw argException("min and max coords come from different volumes");
    }

    if(min.level() != max.level()) {
        throw argException("min and max coords come from different mip levels");
    }
}

dataBbox::dataBbox(const volumeSystem* vol, int level)
    : base_t()
    , vol_(vol)
    , mipLevel_(level)
{ }

dataBbox::dataBbox(dataBbox::base_t bbox, const volumeSystem* vol, int level)
    : base_t(bbox)
    , vol_(vol)
    , mipLevel_(level)
{ }


globalBbox dataBbox::toGlobalBbox() const {
    return globalBbox(getMin().toGlobal(), getMax().toGlobal());
}

normBbox dataBbox::toNormBbox() const
{
    return normBbox(getMin().toNorm(), getMax().toNorm());
}

dataBbox dataBbox::atDifferentLevel(int newLevel) const
{
    return dataBbox(getMin().atDifferentLevel(newLevel),
                    getMax().atDifferentLevel(newLevel));
}

} // namespace coords
} // namespace om
