#include "common/common.h"
#include "common/omDebug.h"
#include "volume/omMipVolume.h"

namespace om {

//const chunkCoord chunkCoord::NULL_COORD(-1, -1, -1, -1);

/////////////////////////////////
///////          chunkCoord

chunkCoord::chunkCoord()
    : Level(-1)
    , Coordinate(Vector3i(-1, -1, -1))
{
}

chunkCoord::chunkCoord(int Level, const Vector3i & coord)
    : Level(Level)
    , Coordinate(coord)
{
    if (coord.x < 0 || coord.y < 0 || coord.z < 0) {
        throw OmArgException("Bad Chunk Coord");
    }
}

chunkCoord::chunkCoord(int level, int x, int y, int z)
    : Level(level)
    , Coordinate(Vector3i(x, y, z))
{
    if (x < 0 || y < 0 || z < 0) {
        throw OmArgException("Bad Chunk Coord");
    }
}

QString chunkCoord::getCoordsAsString() const
{
    return QString("%1:%2,%3,%4")
        .arg(Level)
        .arg(Coordinate.x)
        .arg(Coordinate.y)
        .arg(Coordinate.z);
}

/////////////////////////////////
///////          Family Coordinates

// Parent coordiante containing this coordinate as a child
chunkCoord chunkCoord::ParentCoord() const
{
    //get primary coordinate of octal containing given coordinate
    chunkCoord primary_coord = PrimarySiblingCoord();

    int x = primary_coord.Coordinate.x;
    int y = primary_coord.Coordinate.y;
    int z = primary_coord.Coordinate.z;

    //return parent (next level, half coordinates)
    return chunkCoord(Level + 1, x / 2, y / 2, z / 2);
}

// Primary coordinate in the sibling octal
chunkCoord chunkCoord::PrimarySiblingCoord() const
{
    int prim_x = om::math::roundDown(Coordinate.x, 2);
    int prim_y = om::math::roundDown(Coordinate.y, 2);
    int prim_z = om::math::roundDown(Coordinate.z, 2);

    return chunkCoord(Level, prim_x, prim_y, prim_z);
}

// Array of sibling coordinates in octal
void chunkCoord::SiblingCoords(chunkCoord * pSiblings) const
{
    //return null coordinates if coord is null
    if (Level < 0) {
        //set each child to null
        for (int i = 0; i < 8; i++) {
            //pSiblings[i] = chunkCoord::NULL_COORD; // TODO: Fix for mesher
        }
        return;
    }

    //primary child is in first position
    chunkCoord primary_coord = PrimarySiblingCoord();

    int x = primary_coord.Coordinate.x;
    int y = primary_coord.Coordinate.y;
    int z = primary_coord.Coordinate.z;

    //This should be removed if this function is ever going to be called
    //in a way other than how it already is
    assert(Coordinate.x == primary_coord.Coordinate.x);
    assert(Coordinate.y == primary_coord.Coordinate.y);
    assert(Coordinate.z == primary_coord.Coordinate.z);

    pSiblings[0] = primary_coord;
    pSiblings[1] = chunkCoord(Level, x + 1, y, z);
    pSiblings[2] = chunkCoord(Level, x + 1, y + 1, z);
    pSiblings[3] = chunkCoord(Level, x, y + 1, z);
    pSiblings[4] = chunkCoord(Level, x, y, z + 1);
    pSiblings[5] = chunkCoord(Level, x + 1, y, z + 1);
    pSiblings[6] = chunkCoord(Level, x + 1, y + 1, z + 1);
    pSiblings[7] = chunkCoord(Level, x, y + 1, z + 1);
}

// Primary child coordinate of octal children
chunkCoord chunkCoord::PrimaryChildCoord() const
{
    //return primary child (prev level, double coordinates)
    return chunkCoord(Level - 1, Coordinate.x * 2, Coordinate.y * 2, Coordinate.z * 2);
}

void chunkCoord::ChildrenCoords(chunkCoord * pChildren) const
{
    //get primary child
    chunkCoord primary_child = PrimaryChildCoord();
    primary_child.SiblingCoords(pChildren);

}

dataCoord chunkCoord::toDataCoord(const OmMipVolume *vol) const {
    return dataCoord(Coordinate * vol->Coords().GetChunkDimensions(), vol, Level);
}

dataBbox chunkCoord::chunkBoundingBox(const OmMipVolume *vol) const
{
    const dataCoord min = toDataCoord(vol);
    const dataCoord max = min + vol->Coords().GetChunkDimensions();
    return dataBbox(min, max);
}

/////////////////////////////////
///////          Operators

void chunkCoord::operator=(const chunkCoord & rhs)
{
    Level = rhs.Level;
    Coordinate = rhs.Coordinate;
}

bool chunkCoord::operator==(const chunkCoord & rhs) const
{
    return (Level == rhs.Level && Coordinate == rhs.Coordinate);
}

bool chunkCoord::operator!=(const chunkCoord & rhs) const
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
bool chunkCoord::operator<(const chunkCoord & rhs) const
{
    if (Level != rhs.Level) {
        return (Level < rhs.Level);
    }

    return (Coordinate < rhs.Coordinate);
}

std::ostream& operator<<(std::ostream &out, const chunkCoord &c) {
    out << "[" << c.Level;
    out << " (" << c.Coordinate.x << ", " << c.Coordinate.y << ", " << c.Coordinate.z << ")]";
    return out;
}

} // namespace om