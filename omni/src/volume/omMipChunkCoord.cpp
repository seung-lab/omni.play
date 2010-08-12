#include "volume/omMipChunkCoord.h"
#include "common/omDebug.h"

const OmMipChunkCoord OmMipChunkCoord::NULL_COORD(-1, -1, -1, -1);

/////////////////////////////////
///////          OmMipChunkCoord

OmMipChunkCoord::OmMipChunkCoord()
	: Level(-1)
	, Coordinate(DataCoord(-1, -1, -1))
{
}

OmMipChunkCoord::OmMipChunkCoord(int Level, const DataCoord & rDataCoord)
	: Level(Level)
	, Coordinate(rDataCoord)
{
}

OmMipChunkCoord::OmMipChunkCoord(int level, int x, int y, int z)
	: Level(level)
	, Coordinate(DataCoord(x, y, z))
{
}

QString OmMipChunkCoord::getCoordsAsString() const
{
	QString ret = QString("%1:%2,%3,%4").arg(Level).arg(Coordinate.x).arg(Coordinate.y).arg(Coordinate.z);
	return ret;
}

/////////////////////////////////
///////          Property

bool OmMipChunkCoord::IsLeaf() const
{
	return 0 == Level;
}

/////////////////////////////////
///////          Family Coordinates

/* Parent coordiante containing this coordinate as a child */
OmMipChunkCoord OmMipChunkCoord::ParentCoord() const
{
	//get primary coordinate of octal containing given coordinate
	OmMipChunkCoord primary_coord = PrimarySiblingCoord();

	int x = primary_coord.Coordinate.x;
	int y = primary_coord.Coordinate.y;
	int z = primary_coord.Coordinate.z;

	//return parent (next level, half coordinates)
	return OmMipChunkCoord(Level + 1, x / 2, y / 2, z / 2);

}

/* Primary coordinate in the sibling octal */
OmMipChunkCoord OmMipChunkCoord::PrimarySiblingCoord() const
{
	int prim_x = ROUNDDOWN(Coordinate.x, 2);
	int prim_y = ROUNDDOWN(Coordinate.y, 2);
	int prim_z = ROUNDDOWN(Coordinate.z, 2);

	return OmMipChunkCoord(Level, prim_x, prim_y, prim_z);

}

/* Array of sibling coordinates in octal */
void OmMipChunkCoord::SiblingCoords(OmMipChunkCoord * pSiblings) const
{
	//return null coordinates if coord is null
	if (Level < 0) {
		//set each child to null
		for (int i = 0; i < 8; i++)
			pSiblings[i] = OmMipChunkCoord::NULL_COORD;

		return;
	}

	//primary child is in first position
	OmMipChunkCoord primary_coord = PrimarySiblingCoord();

	int x = primary_coord.Coordinate.x;
	int y = primary_coord.Coordinate.y;
	int z = primary_coord.Coordinate.z;

	//This should be removed if this function is ever going to be called
	//in a way other than how it already is
	assert(Coordinate.x == primary_coord.Coordinate.x);
	assert(Coordinate.y == primary_coord.Coordinate.y);
	assert(Coordinate.z == primary_coord.Coordinate.z);

	pSiblings[0] = primary_coord;
	pSiblings[1] = OmMipChunkCoord(Level, x + 1, y, z);
	pSiblings[2] = OmMipChunkCoord(Level, x + 1, y + 1, z);
	pSiblings[3] = OmMipChunkCoord(Level, x, y + 1, z);
	pSiblings[4] = OmMipChunkCoord(Level, x, y, z + 1);
	pSiblings[5] = OmMipChunkCoord(Level, x + 1, y, z + 1);
	pSiblings[6] = OmMipChunkCoord(Level, x + 1, y + 1, z + 1);
	pSiblings[7] = OmMipChunkCoord(Level, x, y + 1, z + 1);
}

/* Primary child coordinate of octal children */
OmMipChunkCoord OmMipChunkCoord::PrimaryChildCoord() const
{
	//return primary child (prev level, double coordinates)
	return OmMipChunkCoord(Level - 1, Coordinate.x * 2, Coordinate.y * 2, Coordinate.z * 2);
}

void OmMipChunkCoord::ChildrenCoords(OmMipChunkCoord * pChildren) const
{
	//get primary child
	OmMipChunkCoord primary_child = PrimaryChildCoord();
	primary_child.SiblingCoords(pChildren);

}

/////////////////////////////////
///////          Operators

void OmMipChunkCoord::operator=(const OmMipChunkCoord & rhs)
{
	Level = rhs.Level;
	Coordinate = rhs.Coordinate;
}

bool OmMipChunkCoord::operator==(const OmMipChunkCoord & rhs) const
{
	return (Level == rhs.Level && Coordinate == rhs.Coordinate);
}

bool OmMipChunkCoord::operator!=(const OmMipChunkCoord & rhs) const
{
	if( Level != rhs.Level ){
		return true;
	}

	if(Coordinate != rhs.Coordinate) {
		return true;
	}

	return false;
}

/* comparitor for stl key usage */
bool OmMipChunkCoord::operator<(const OmMipChunkCoord & rhs) const
{
	if (Level != rhs.Level) {
		return (Level < rhs.Level);
	}

	return (Coordinate < rhs.Coordinate);
}

std::ostream& operator<<(std::ostream &out, const OmMipChunkCoord &c) {
	out << "[" << c.Level;
	out << " (" << c.Coordinate.x << ", " << c.Coordinate.y << ", " << c.Coordinate.z << ")]";
	return out;
}
