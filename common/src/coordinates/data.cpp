#include "common/common.h"
#include "common/exception.h"

using namespace om::common;

namespace om {
namespace coords {

Data::Data(Data::base_t v, const volumeSystem* vol, int mipLevel)
    : base_t(v)
    , vol_(vol)
    , mipLevel_(mipLevel)
{
    if (mipLevel > vol_->GetRootMipLevel()) {
        throw argException("Invalid Mip level.");
    }
}

Data::Data(int x, int y, int z, const volumeSystem* vol, int mipLevel)
    : base_t(x, y, z)
    , vol_(vol)
    , mipLevel_(mipLevel)
{
    if (mipLevel > vol_->GetRootMipLevel()) {
        throw argException("Invalid Mip level.");
    }
}


Global Data::ToGlobal() const
{
    const vmml::Vector4f data(x, y, z, 1);
    vmml::Vector3f global = vol_->DataToGlobalMat(mipLevel_) * data;
    return global;
}

Norm Data::ToNorm() const
{
    return ToGlobal().ToNorm(vol_);
}

Chunk Data::ToChunk() const
{
    return Chunk(mipLevel_, *this / vol_->GetChunkDimensions());
}

Vector3i Data::ToChunkVec() const
{
    VecTor3i dims = vol_->GetChunkDimensions();
    return Vector3i(x % dims.x, y % dims.y, z % dims.z);
}

int Data::ToChunkOffset() const
{
    const Vector3i dims = vol_->GetChunkDimensions();
    const Vector3i chunkVec = toChunkVec();
    return dims.x * dims.y * chunkVec.z + dims.x * chunkVec.y + chunkVec.x;
}

int Data::ToTileOffset (viewType viewType) const
{
    const Vector3i dims = vol_->GetChunkDimensions();
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
    const Vector3i dims = vol_->GetChunkDimensions();
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
	return vol_->GetDataExtent().contains(ToGlobal());
}

Data Data::AtDifferentLevel(int newLevel) const
{
    if (newLevel > vol_->GetRootMipLevel()) {
        throw argException("Invalid Mip level.");
    }

    if(newLevel == mipLevel_) {
        return *this;
    }

    Vector3f result = *this * om::math::pow2int(mipLevel_) / om::math::pow2int(newLevel);
    return Data(result, vol_, newLevel);
}

DataBbox::DataBbox(Data min, Data max)
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

DataBbox::DataBbox(const volumeSystem* vol, int level)
    : base_t()
    , vol_(vol)
    , mipLevel_(level)
{ }

DataBbox::DataBbox(DataBbox::base_t bbox, const volumeSystem* vol, int level)
    : base_t(bbox)
    , vol_(vol)
    , mipLevel_(level)
{ }


GlobalBbox DataBbox::ToGlobalBbox() const {
    return GlobalBbox(getMin().ToGlobal(), getMax().ToGlobal());
}

NormBbox DataBbox::ToNormBbox() const
{
    return NormBbox(getMin().ToNorm(), getMax().toNorm());
}

DataBbox DataBbox::AtDifferentLevel(int newLevel) const
{
    return DataBbox(getMin().atDifferentLevel(newLevel),
                    getMax().atDifferentLevel(newLevel));
}

} // namespace coords
} // namespace om
