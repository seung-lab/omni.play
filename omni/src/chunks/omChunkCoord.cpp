#include "chunks/omChunkCoord.h"
#include "common/omDebug.h"

const OmChunkCoord OmChunkCoord::NULL_COORD(-1, -1, -1, -1);

/////////////////////////////////
///////          OmChunkCoord

OmChunkCoord::OmChunkCoord()
	: Level(-1)
	, Coordinate(DataCoord(-1, -1, -1))
{
}

OmChunkCoord::OmChunkCoord(int Level, const DataCoord & rDataCoord)
	: Level(Level)
	, Coordinate(rDataCoord)
{
}

OmChunkCoord::OmChunkCoord(int level, int x, int y, int z)
	: Level(level)
	, Coordinate(DataCoord(x, y, z))
{
}

QString OmChunkCoord::getCoordsAsString() const
{
	return QString("%1:%2,%3,%4")
		.arg(Level)
		.arg(Coordinate.x)
		.arg(Coordinate.y)
		.arg(Coordinate.z);
}

/////////////////////////////////
///////          Property

bool OmChunkCoord::IsLeaf() const
{
	return 0 == Level;
}

/////////////////////////////////
///////          Family Coordinates

/* Parent coordiante containing this coordinate as a child */
OmChunkCoord OmChunkCoord::ParentCoord() const
{
	//get primary coordinate of octal containing given coordinate
	OmChunkCoord primary_coord = PrimarySiblingCoord();

	int x = primary_coord.Coordinate.x;
	int y = primary_coord.Coordinate.y;
	int z = primary_coord.Coordinate.z;

	//return parent (next level, half coordinates)
	return OmChunkCoord(Level + 1, x / 2, y / 2, z / 2);

}

/* Primary coordinate in the sibling octal */
OmChunkCoord OmChunkCoord::PrimarySiblingCoord() const
{
	int prim_x = ROUNDDOWN(Coordinate.x, 2);
	int prim_y = ROUNDDOWN(Coordinate.y, 2);
	int prim_z = ROUNDDOWN(Coordinate.z, 2);

	return OmChunkCoord(Level, prim_x, prim_y, prim_z);

}

/* Array of sibling coordinates in octal */
void OmChunkCoord::SiblingCoords(OmChunkCoord * pSiblings) const
{
	//return null coordinates if coord is null
	if (Level < 0) {
		//set each child to null
		for (int i = 0; i < 8; i++)
			pSiblings[i] = OmChunkCoord::NULL_COORD;

		return;
	}

	//primary child is in first position
	OmChunkCoord primary_coord = PrimarySiblingCoord();

	int x = primary_coord.Coordinate.x;
	int y = primary_coord.Coordinate.y;
	int z = primary_coord.Coordinate.z;

	//This should be removed if this function is ever going to be called
	//in a way other than how it already is
	assert(Coordinate.x == primary_coord.Coordinate.x);
	assert(Coordinate.y == primary_coord.Coordinate.y);
	assert(Coordinate.z == primary_coord.Coordinate.z);

	pSiblings[0] = primary_coord;
	pSiblings[1] = OmChunkCoord(Level, x + 1, y, z);
	pSiblings[2] = OmChunkCoord(Level, x + 1, y + 1, z);
	pSiblings[3] = OmChunkCoord(Level, x, y + 1, z);
	pSiblings[4] = OmChunkCoord(Level, x, y, z + 1);
	pSiblings[5] = OmChunkCoord(Level, x + 1, y, z + 1);
	pSiblings[6] = OmChunkCoord(Level, x + 1, y + 1, z + 1);
	pSiblings[7] = OmChunkCoord(Level, x, y + 1, z + 1);
}

/* Primary child coordinate of octal children */
OmChunkCoord OmChunkCoord::PrimaryChildCoord() const
{
	//return primary child (prev level, double coordinates)
	return OmChunkCoord(Level - 1, Coordinate.x * 2, Coordinate.y * 2, Coordinate.z * 2);
}

void OmChunkCoord::ChildrenCoords(OmChunkCoord * pChildren) const
{
	//get primary child
	OmChunkCoord primary_child = PrimaryChildCoord();
	primary_child.SiblingCoords(pChildren);

}

/////////////////////////////////
///////          Operators

void OmChunkCoord::operator=(const OmChunkCoord & rhs)
{
	Level = rhs.Level;
	Coordinate = rhs.Coordinate;
}

bool OmChunkCoord::operator==(const OmChunkCoord & rhs) const
{
	return (Level == rhs.Level && Coordinate == rhs.Coordinate);
}

bool OmChunkCoord::operator!=(const OmChunkCoord & rhs) const
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
bool OmChunkCoord::operator<(const OmChunkCoord & rhs) const
{
	if (Level != rhs.Level) {
		return (Level < rhs.Level);
	}

	return (Coordinate < rhs.Coordinate);
}

std::ostream& operator<<(std::ostream &out, const OmChunkCoord &c) {
	out << "[" << c.Level;
	out << " (" << c.Coordinate.x << ", " << c.Coordinate.y << ", " << c.Coordinate.z << ")]";
	return out;
}
