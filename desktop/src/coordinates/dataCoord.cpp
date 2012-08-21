#include "common/omCommon.h"
#include "volume/omMipVolume.h"
#include "common/omException.h"

namespace om {

dataCoord::dataCoord(dataCoord::base_t v, const OmMipVolume* vol, int mipLevel)
    : base_t(v)
    , vol_(vol)
    , mipLevel_(mipLevel)
{
    if (mipLevel > vol_->Coords().GetRootMipLevel()) {
        throw OmArgException("Invalid Mip level.");
    }
}

dataCoord::dataCoord(int x, int y, int z, const OmMipVolume* vol, int mipLevel)
    : base_t(x, y, z)
    , vol_(vol)
    , mipLevel_(mipLevel)
{
    if (mipLevel > vol_->Coords().GetRootMipLevel()) {
        throw OmArgException("Invalid Mip level.");
    }
}


globalCoord dataCoord::toGlobalCoord() const
{
    const vmml::Vector4f data(x, y, z, 1);
    vmml::Vector3f global = vol_->Coords().DataToGlobalMat(mipLevel_) * data;
    return global;
}

normCoord dataCoord::toNormCoord() const
{
    return toGlobalCoord().toNormCoord(vol_);
}

chunkCoord dataCoord::toChunkCoord() const
{
    return chunkCoord(mipLevel_, *this / vol_->Coords().GetChunkDimensions());
}

Vector3i dataCoord::toChunkVec() const
{
    Vector3i dims = vol_->Coords().GetChunkDimensions();
    return Vector3i(x % dims.x, y % dims.y, z % dims.z);
}

int dataCoord::toChunkOffset() const
{
    const Vector3i dims = vol_->Coords().GetChunkDimensions();
    const Vector3i chunkVec = toChunkVec();
    return dims.x * dims.y * chunkVec.z + dims.x * chunkVec.y + chunkVec.x;
}

int dataCoord::toTileOffset (ViewType viewType) const
{
    const Vector3i dims = vol_->Coords().GetChunkDimensions();
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

dataCoord dataCoord::atDifferentLevel(int newLevel) const
{
    if (newLevel > vol_->Coords().GetRootMipLevel()) {
        throw OmArgException("Invalid Mip level.");
    }

    if(newLevel == mipLevel_) {
        return *this;
    }

    Vector3f result = *this * om::math::pow2int(mipLevel_) / om::math::pow2int(newLevel);
    return dataCoord(result, vol_, newLevel);
}

dataBbox::dataBbox(dataCoord min, dataCoord max)
    : base_t(min, max)
    , vol_(min.volume())
    , mipLevel_(min.level())
{
    if(min.volume() != max.volume()) {
        throw OmArgException("min and max coords come from different volumes");
    }

    if(min.level() != max.level()) {
        throw OmArgException("min and max coords come from different mip levels");
    }
}

dataBbox::dataBbox(const OmMipVolume* vol, int level)
    : base_t()
    , vol_(vol)
    , mipLevel_(level)
{ }

dataBbox::dataBbox(dataBbox::base_t bbox, const OmMipVolume* vol, int level)
    : base_t(bbox)
    , vol_(vol)
    , mipLevel_(level)
{ }


globalBbox dataBbox::toGlobalBbox() const {
    return globalBbox(getMin().toGlobalCoord(), getMax().toGlobalCoord());
}

normBbox dataBbox::toNormBbox() const
{
    return normBbox(getMin().toNormCoord(), getMax().toNormCoord());
}


dataBbox dataBbox::atDifferentLevel(int newLevel) const
{
    return dataBbox(getMin().atDifferentLevel(newLevel),
                    getMax().atDifferentLevel(newLevel));
}

} // namespace om