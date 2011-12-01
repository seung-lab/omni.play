#include "common/common.h"
#include "common/debug.h"

#include <sstream>

namespace om {
namespace coords {

//const chunk chunk::NULL_COORD(-1, -1, -1, -1);

/////////////////////////////////
///////          chunk

chunk::chunk()
    : Level(-1)
    , Coordinate(Vector3i(-1, -1, -1))
{
}

chunk::chunk(int Level, const Vector3i & coord)
    : Level(Level)
    , Coordinate(coord)
{
    if (coord.x < 0 || coord.y < 0 || coord.z < 0) {
        throw common::argException("Bad Chunk Coord");
    }
}

chunk::chunk(int level, int x, int y, int z)
    : Level(level)
    , Coordinate(Vector3i(x, y, z))
{
    if (x < 0 || y < 0 || z < 0) {
        throw common::argException("Bad Chunk Coord");
    }
}

std::string chunk::getCoordsAsString() const
{
    std::stringstream ss;
    ss << Level <<":"
       << Coordinate.x <<","
       << Coordinate.y <<","
       << Coordinate.z;
   return ss.str();
}

/////////////////////////////////
///////          Family Coordinates

// Parent coordiante containing this coordinate as a child
chunk chunk::ParentCoord() const
{
    //get primary coordinate of octal containing given coordinate
    chunk primary_coord = PrimarySiblingCoord();

    int x = primary_coord.Coordinate.x;
    int y = primary_coord.Coordinate.y;
    int z = primary_coord.Coordinate.z;

    //return parent (next level, half coordinates)
    return chunk(Level + 1, x / 2, y / 2, z / 2);
}

// Primary coordinate in the sibling octal
chunk chunk::PrimarySiblingCoord() const
{
    int prim_x = om::math::roundDown(Coordinate.x, 2);
    int prim_y = om::math::roundDown(Coordinate.y, 2);
    int prim_z = om::math::roundDown(Coordinate.z, 2);

    return chunk(Level, prim_x, prim_y, prim_z);
}

// Array of sibling coordinates in octal
void chunk::SiblingCoords(chunk * pSiblings) const
{
    //return null coordinates if coord is null
    if (Level < 0) {
        //set each child to null
        for (int i = 0; i < 8; i++) {
            //pSiblings[i] = chunk::NULL_COORD; // TODO: Fix for mesher
        }
        return;
    }

    //primary child is in first position
    chunk primary_coord = PrimarySiblingCoord();

    int x = primary_coord.Coordinate.x;
    int y = primary_coord.Coordinate.y;
    int z = primary_coord.Coordinate.z;

    //This should be removed if this function is ever going to be called
    //in a way other than how it already is
    assert(Coordinate.x == primary_coord.Coordinate.x);
    assert(Coordinate.y == primary_coord.Coordinate.y);
    assert(Coordinate.z == primary_coord.Coordinate.z);

    pSiblings[0] = primary_coord;
    pSiblings[1] = chunk(Level, x + 1, y, z);
    pSiblings[2] = chunk(Level, x + 1, y + 1, z);
    pSiblings[3] = chunk(Level, x, y + 1, z);
    pSiblings[4] = chunk(Level, x, y, z + 1);
    pSiblings[5] = chunk(Level, x + 1, y, z + 1);
    pSiblings[6] = chunk(Level, x + 1, y + 1, z + 1);
    pSiblings[7] = chunk(Level, x, y + 1, z + 1);
}

// Primary child coordinate of octal children
chunk chunk::PrimaryChildCoord() const
{
    //return primary child (prev level, double coordinates)
    return chunk(Level - 1, Coordinate.x * 2, Coordinate.y * 2, Coordinate.z * 2);
}

void chunk::ChildrenCoords(chunk * pChildren) const
{
    //get primary child
    chunk primary_child = PrimaryChildCoord();
    primary_child.SiblingCoords(pChildren);

}

data chunk::toData(const volumeSystem *vol) const {
    return data(Coordinate * vol->GetChunkDimensions(), vol, Level);
}

dataBbox chunk::chunkBoundingBox(const volumeSystem *vol) const
{
    const data min = toData(vol);
    const data max = min + vol->GetChunkDimensions();
    return dataBbox(min, max);
}

uint64_t chunk::chunkPtrOffset(const volumeSystem* vol, int64_t bytesPerVoxel) const
{
    const Vector3<int64_t> volDims = vol->getDimsRoundedToNearestChunk(Level);
    const Vector3<int64_t> chunkDims = vol->GetChunkDimensions();

    const int64_t slabSize  = volDims.x   * volDims.y   * chunkDims.z * bytesPerVoxel;
    const int64_t rowSize   = volDims.x   * chunkDims.y * chunkDims.z * bytesPerVoxel;
    const int64_t chunkSize = chunkDims.x * chunkDims.y * chunkDims.z * bytesPerVoxel;

    const Vector3<int64_t> chunkPos = Coordinate; // bottom left corner
    const int64_t offset = slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;

    ZiLOG(DEBUG, io) << "offset is: " << offset
                     << " (" << volDims << ") for "
                     << Coordinate << "\n";
    return offset;
}

int chunk::sliceDepth(const volumeSystem* vol, global c, common::viewType view) const
{
    const data d = c.toData(vol, Level);
    const dataBbox bounds = chunkBoundingBox(vol);
    if(!bounds.contains(d)) {
        throw common::argException("Coordinate outside of chunk.");
    }

    switch(view)
    {
    case common::XY_VIEW: return d.z - bounds.getMin().z;
    case common::XZ_VIEW: return d.y - bounds.getMin().y;
    case common::ZY_VIEW: return d.x - bounds.getMin().x;
    }

    throw common::argException("Bad viewType");
}

/////////////////////////////////
///////          Operators

void chunk::operator=(const chunk & rhs)
{
    Level = rhs.Level;
    Coordinate = rhs.Coordinate;
}

bool chunk::operator==(const chunk & rhs) const
{
    return (Level == rhs.Level && Coordinate == rhs.Coordinate);
}

bool chunk::operator!=(const chunk & rhs) const
{
    if( Level != rhs.Level ){
        return true;
    }

    if(Coordinate != rhs.Coordinate) {
        return true;
    }

    return false;
}

// comparitor for stl key usage
bool chunk::operator<(const chunk & rhs) const
{
    if (Level != rhs.Level) {
        return (Level < rhs.Level);
    }

    return (Coordinate < rhs.Coordinate);
}

std::ostream& operator<<(std::ostream &out, const chunk &c) {
    out << "[" << c.Level;
    out << " (" << c.Coordinate.x << ", " << c.Coordinate.y << ", " << c.Coordinate.z << ")]";
    return out;
}

} // namespace coords
} // namespace om
