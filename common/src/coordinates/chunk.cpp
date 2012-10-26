#include "common/common.h"
#include "common/debug.h"

#include <sstream>

namespace om {
namespace coords {

Chunk::Chunk()
    : mipLevel_(-1)
    , coord_(Vector3i(-1, -1, -1))
{
}

Chunk::Chunk(int level, const Vector3i & coord)
    : mipLevel_(level)
    , coord_(coord)
{
    if (coord.x < 0 || coord.y < 0 || coord.z < 0) {
        throw argException("Bad Chunk Coord");
    }
}

Chunk::Chunk(int level, int x, int y, int z)
    : mipLevel_(level)
    , coord_(Vector3i(x, y, z))
{
    if (x < 0 || y < 0 || z < 0) {
        throw argException("Bad Chunk Coord");
    }
}

std::string Chunk::GetCoordsAsString() const
{
    std::stringstream ss;
    ss << mipLevel_ <<":"
       << coord_.x <<","
       << coord_.y <<","
       << coord_.z;
   return ss.str();
}

// Parent coordiante containing this coordinate as a child
Chunk Chunk::ParentCoord() const
{
    Chunk primary_coord = PrimarySiblingCoord();

    return Chunk(mipLevel_ + 1, primary_coord.coord_ / 2);
}

// Primary coordinate in the sibling octal
Chunk Chunk::PrimarySiblingCoord() const
{
    int prim_x = om::math::roundDown(coord_.x, 2);
    int prim_y = om::math::roundDown(coord_.y, 2);
    int prim_z = om::math::roundDown(coord_.z, 2);

    return Chunk(mipLevel_, prim_x, prim_y, prim_z);
}

std::vector<Chunk> Chunk::SiblingCoords() const
{
	std::vector<Chunk> sibs;
    //return null coordinates if coord is null
    if (mipLevel_ < 0) {
        return sibs;
    }

    Chunk primary_coord = PrimarySiblingCoord();

    int x = primary_coord.coord_.x;
    int y = primary_coord.coord_.y;
    int z = primary_coord.coord_.z;

    sibs.push_back(primary_coord);
    sibs.push_back(Chunk(mipLevel_, x + 1, y, z));
    sibs.push_back(Chunk(mipLevel_, x + 1, y + 1, z));
    sibs.push_back(Chunk(mipLevel_, x, y + 1, z));
    sibs.push_back(Chunk(mipLevel_, x, y, z + 1));
    sibs.push_back(Chunk(mipLevel_, x + 1, y, z + 1));
    sibs.push_back(Chunk(mipLevel_, x + 1, y + 1, z + 1));
    sibs.push_back(Chunk(mipLevel_, x, y + 1, z + 1));

    return sibs;
}

// Primary child coordinate of octal children
Chunk Chunk::PrimaryChildCoord() const
{
    //return primary child (prev level, double coordinates)
    return Chunk(mipLevel_ - 1, coord_.x * 2, coord_.y * 2, coord_.z * 2);
}

std::vector<Chunk> Chunk::ChildrenCoords() const
{
    Chunk primary_child = PrimaryChildCoord();
    return primary_child.SiblingCoords();
}

data Chunk::ToData(const volumeSystem *vol) const {
    return data(coord_ * vol->GetChunkDimensions(), vol, mipLevel_);
}

dataBbox Chunk::BoundingBox(const volumeSystem *vol) const
{
    const data min = ToData(vol);
    const data max = min + vol->GetChunkDimensions();
    return dataBbox(min, max);
}

uint64_t Chunk::PtrOffset(const volumeSystem* vol, int64_t bytesPerVoxel) const
{
    const Vector3<int64_t> volDims = vol->getDimsRoundedToNearestChunk(mipLevel_);
    const Vector3<int64_t> chunkDims = vol->GetChunkDimensions();

    const int64_t slabSize  = volDims.x   * volDims.y   * chunkDims.z * bytesPerVoxel;
    const int64_t rowSize   = volDims.x   * chunkDims.y * chunkDims.z * bytesPerVoxel;
    const int64_t chunkSize = chunkDims.x * chunkDims.y * chunkDims.z * bytesPerVoxel;

    const Vector3<int64_t> chunkPos = coord_; // bottom left corner
    const int64_t offset = slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;

    ZiLOG(DEBUG, io) << "offset is: " << offset
                     << " (" << volDims << ") for "
                     << coord_ << "\n";
    return offset;
}

int Chunk::SliceDepth(const volumeSystem* vol, global c, common::viewType view) const
{
    const data d = c.toData(vol, mipLevel_);
    const dataBbox bounds = BoundingBox(vol);
    if(!bounds.contains(d)) {
        throw argException("Coord outside of chunk.");
    }

    switch(view)
    {
    case common::XY_VIEW: return d.z - bounds.getMin().z;
    case common::XZ_VIEW: return d.y - bounds.getMin().y;
    case common::ZY_VIEW: return d.x - bounds.getMin().x;
    }

    throw argException("Bad viewType");
}

void Chunk::operator=(const Chunk & rhs)
{
    mipLevel_ = rhs.mipLevel_;
    coord_ = rhs.coord_;
}

bool Chunk::operator==(const Chunk & rhs) const
{
    return (mipLevel_ == rhs.mipLevel_ && coord_ == rhs.coord_);
}

bool Chunk::operator!=(const Chunk & rhs) const
{
    if( mipLevel_ != rhs.mipLevel_ ){
        return true;
    }

    if(coord_ != rhs.coord_) {
        return true;
    }

    return false;
}

// comparitor for stl key usage
bool Chunk::operator<(const Chunk & rhs) const
{
    if (mipLevel_ != rhs.mipLevel_) {
        return (mipLevel_ < rhs.mipLevel_);
    }

    return (coord_ < rhs.coord_);
}

std::ostream& operator<<(std::ostream &out, const Chunk &c) {
    out << "[" << c.mipLevel_;
    out << " (" << c.coord_.x << ", " << c.coord_.y << ", " << c.coord_.z << ")]";
    return out;
}

} // namespace coords
} // namespace om
