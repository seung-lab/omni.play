#include "common/common.h"
#include "common/exception.h"

using namespace om::common;

namespace om {
namespace coords {

Data::Data(Data::base_t v, const VolumeSystem* volume, int mipLevel)
    : base_t(v)
    , volume_(volume)
    , mipLevel_(mipLevel)
{
    if (mipLevel > volume_->RootMipLevel()) {
        throw argException("Invalid Mip level.");
    }
}

Data::Data(int x, int y, int z, const VolumeSystem* volume, int mipLevel)
    : base_t(x, y, z)
    , volume_(volume)
    , mipLevel_(mipLevel)
{
    if (mipLevel > volume_->RootMipLevel()) {
        throw argException("Invalid Mip level.");
    }
}


Global Data::ToGlobal() const
{
    const vmml::Vector4f data(x, y, z, 1);
    vmml::Vector3f global = volume_->DataToGlobalMat(mipLevel_) * data;
    return global;
}

Norm Data::ToNorm() const
{
    return ToGlobal().ToNorm(volume_);
}

Chunk Data::ToChunk() const
{
    return Chunk(mipLevel_, *this / volume_->ChunkDimensions());
}

Vector3i Data::ToChunkVec() const
{
    Vector3i dims = volume_->ChunkDimensions();
    return Vector3i(x % dims.x, y % dims.y, z % dims.z);
}

int Data::ToChunkOffset() const
{
    const Vector3i dims = volume_->ChunkDimensions();
    const Vector3i chunkVec = ToChunkVec();
    return dims.x * dims.y * chunkVec.z + dims.x * chunkVec.y + chunkVec.x;
}

int Data::ToTileOffset (viewType viewType) const
{
    const Vector3i dims = volume_->ChunkDimensions();
    const Vector3i chunkVec = ToChunkVec();

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

int Data::ToTileDepth (viewType viewType) const
{
    const Vector3i dims = volume_->ChunkDimensions();
    const Vector3i chunkVec = ToChunkVec();

    switch(viewType)
    {
    case XY_VIEW: return chunkVec.z;
    case XZ_VIEW: return chunkVec.y;
    case ZY_VIEW: return chunkVec.x;
    }

    return -1;
}

bool Data::IsInVolume() const {
	return volume_->DataExtent().contains(ToGlobal());
}

Data Data::AtDifferentLevel(int newLevel) const
{
    if (newLevel > volume_->RootMipLevel()) {
        throw argException("Invalid Mip level.");
    }

    if(newLevel == mipLevel_) {
        return *this;
    }

    Vector3f result = *this * om::math::pow2int(mipLevel_) / om::math::pow2int(newLevel);
    return Data(result, volume_, newLevel);
}

DataBbox::DataBbox(Data min, Data max)
    : base_t(min, max)
    , volume_(min.volume())
    , mipLevel_(min.mipLevel())
{
    if(min.volume() != max.volume()) {
        throw argException("min and max coords come from different volumes");
    }

    if(min.mipLevel() != max.mipLevel()) {
        throw argException("min and max coords come from different mip levels");
    }
}

DataBbox::DataBbox(const VolumeSystem* volume, int level)
    : base_t()
    , volume_(volume)
    , mipLevel_(level)
{ }

DataBbox::DataBbox(DataBbox::base_t bbox, const VolumeSystem* volume, int level)
    : base_t(bbox)
    , volume_(volume)
    , mipLevel_(level)
{ }


GlobalBbox DataBbox::ToGlobalBbox() const {
    return GlobalBbox(getMin().ToGlobal(), getMax().ToGlobal());
}

NormBbox DataBbox::ToNormBbox() const
{
    return NormBbox(getMin().ToNorm(), getMax().ToNorm());
}

DataBbox DataBbox::AtDifferentLevel(int newLevel) const
{
    return DataBbox(getMin().AtDifferentLevel(newLevel),
                    getMax().AtDifferentLevel(newLevel));
}

} // namespace coords
} // namespace om
