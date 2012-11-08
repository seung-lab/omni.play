#include "common/common.h"
#include "common/debug.h"

#include <sstream>

namespace om {
namespace coords {

Chunk::Chunk(int level, const Vector3i & coord)
    : base_t(coord)
    , mipLevel_(level)
{
    if (coord.x < 0 || coord.y < 0 || coord.z < 0) {
        throw ArgException("Bad Chunk Coord");
    }
}

Chunk::Chunk(int level, int x, int y, int z)
    : base_t(Vector3i(x, y, z))
    , mipLevel_(level)
{
    if (x < 0 || y < 0 || z < 0) {
        throw ArgException("Bad Chunk Coord");
    }
}

std::string Chunk::GetCoordsAsString() const
{
    std::stringstream ss;
    ss << mipLevel_ << ":" << x << "," << y << "," << z;
   return ss.str();
}

// Parent coordiante containing this coordinate as a child
Chunk Chunk::ParentCoord() const
{
    Chunk primary_coord = PrimarySiblingCoord();

    return Chunk(mipLevel_ + 1, primary_coord / 2);
}

// Primary coordinate in the sibling octal
Chunk Chunk::PrimarySiblingCoord() const
{
    int prim_x = om::math::roundDown(x, 2);
    int prim_y = om::math::roundDown(y, 2);
    int prim_z = om::math::roundDown(z, 2);

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

    int x = primary_coord.x;
    int y = primary_coord.y;
    int z = primary_coord.z;

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
    return Chunk(mipLevel_ - 1, x * 2, y * 2, z * 2);
}

std::vector<Chunk> Chunk::ChildrenCoords() const
{
    Chunk primary_child = PrimaryChildCoord();
    return primary_child.SiblingCoords();
}

Data Chunk::ToData(const VolumeSystem *vol) const {
    return Data(*this * vol->ChunkDimensions(), vol, mipLevel_);
}

DataBbox Chunk::BoundingBox(const VolumeSystem *vol) const
{
    const Data min = ToData(vol);
    const Data max = min + vol->ChunkDimensions();
    return DataBbox(min, max);
}

uint64_t Chunk::PtrOffset(const VolumeSystem* vol, int64_t bytesPerVoxel) const
{
    const Vector3<int64_t> volDims = vol->DimsRoundedToNearestChunk(mipLevel_);
    const Vector3<int64_t> chunkDims = vol->ChunkDimensions();

    const int64_t slabSize  = volDims.x   * volDims.y   * chunkDims.z * bytesPerVoxel;
    const int64_t rowSize   = volDims.x   * chunkDims.y * chunkDims.z * bytesPerVoxel;
    const int64_t chunkSize = chunkDims.x * chunkDims.y * chunkDims.z * bytesPerVoxel;

    const Vector3<int64_t> chunkPos = *this; // bottom left corner
    const int64_t offset = slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;

    ZiLOG(DEBUG, io) << "offset is: " << offset
                     << " (" << volDims << ") for "
                     << *this << "\n";
    return offset;
}

int Chunk::SliceDepth(const VolumeSystem* vol, Global c, om::common::ViewType view) const
{
    const Data d = c.ToData(vol, mipLevel_);
    const DataBbox bounds = BoundingBox(vol);
    if(!bounds.contains(d)) {
        throw ArgException("Coord outside of chunk.");
    }

    switch(view)
    {
    case common::XY_VIEW: return d.z - bounds.getMin().z;
    case common::XZ_VIEW: return d.y - bounds.getMin().y;
    case common::ZY_VIEW: return d.x - bounds.getMin().x;
    }

    throw ArgException("Bad viewType");
}

void Chunk::operator=(const Chunk & rhs)
{
    mipLevel_ = rhs.mipLevel_;
    x = rhs.x; y = rhs.y; z = rhs.z;
}

bool Chunk::operator==(const Chunk & rhs) const
{
    return (mipLevel_ == rhs.mipLevel_ && base_t::operator==(rhs));
}

bool Chunk::operator!=(const Chunk & rhs) const
{
    return !(*this == rhs);
}

// comparitor for stl key usage
bool Chunk::operator<(const Chunk & rhs) const
{
    if (mipLevel_ != rhs.mipLevel_) {
        return (mipLevel_ < rhs.mipLevel_);
    }

    return base_t::operator<(rhs);
}

std::ostream& operator<<(std::ostream &out, const Chunk &c) {
    out << "[" << c.mipLevel_;
    out << " (" << c.x << ", " << c.y << ", " << c.z << ")]";
    return out;
}

} // namespace coords
} // namespace om
