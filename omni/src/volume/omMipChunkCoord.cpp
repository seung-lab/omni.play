
#include "omMipChunkCoord.h"
#include "system/omDebug.h"

const OmMipChunkCoord OmMipChunkCoord::NULL_COORD(-1, -1, -1, -1);

#pragma mark -
#pragma mark OmMipChunkCoord
/////////////////////////////////
///////          OmMipChunkCoord

OmMipChunkCoord::OmMipChunkCoord()
 : Level(-1), Coordinate(DataCoord(-1, -1, -1))
{
}

OmMipChunkCoord::OmMipChunkCoord(int Level, const DataCoord & rDataCoord)
:Level(Level), Coordinate(rDataCoord)
{
}

OmMipChunkCoord::OmMipChunkCoord(int level, int x, int y, int z)
:Level(level), Coordinate(DataCoord(x, y, z))
{

}

#pragma mark -
#pragma mark Property
/////////////////////////////////
///////          Property

bool OmMipChunkCoord::IsLeaf() constconst
{
	return 0 == Level;
}

#pragma mark -
#pragma mark Family Coordinates
/////////////////////////////////
///////          Family Coordinates

/* Parent coordiante containing this coordinate as a child */
OmMipChunkCoord OmMipChunkCoord::ParentCoord() constconst
{
	//get primary coordinate of octal containing given coordinate
	OmMipChunkCoord primary_coord = PrimarySiblingCoord();

	//return parent (next level, half coordinates)
	return OmMipChunkCoord(Level + 1, Coordinate.x / 2, Coordinate.y / 2, Coordinate.z / 2);

}

/* Primary coordinate in the sibling octal */
OmMipChunkCoord OmMipChunkCoord::PrimarySiblingCoord() constconst
{

	int prim_x = ROUNDDOWN(Coordinate.x, 2);
	int prim_y = ROUNDDOWN(Coordinate.y, 2);
	int prim_z = ROUNDDOWN(Coordinate.z, 2);

	return OmMipChunkCoord(Level, prim_x, prim_y, prim_z);

}

/* Array of sibling coordinates in octal */
void OmMipChunkCoord::SiblingCoords(OmMipChunkCoord * pSiblings) const const
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

	int x = Coordinate.x;
	int y = Coordinate.y;
	int z = Coordinate.z;

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
OmMipChunkCoord OmMipChunkCoord::PrimaryChildCoord() constconst
{

	//return primary child (prev level, double coordinates)
	return OmMipChunkCoord(Level - 1, Coordinate.x * 2, Coordinate.y * 2, Coordinate.z * 2);
}

void OmMipChunkCoord::ChildrenCoords(OmMipChunkCoord * pChildren) const const
{
	//get primary child
	OmMipChunkCoord primary_child = PrimaryChildCoord();
	primary_child.SiblingCoords(pChildren);

}

#pragma mark -
#pragma mark Operators
/////////////////////////////////
///////          Operators

void OmMipChunkCoord::operator=(const OmMipChunkCoord & rhs)
{
	Level = rhs.Level;
	Coordinate = rhs.Coordinate;
}

bool OmMipChunkCoord::operator==(const OmMipChunkCoord & rhs) const const
{
	return (Level == rhs.Level && Coordinate == rhs.Coordinate);
}

bool OmMipChunkCoord::operator!=(const OmMipChunkCoord & rhs) constconst
{
	//cout << Level << endl;
	//cout << rhs.Level << endl;
	//cout << Coordinate << endl;
	//cout << rhs.Coordinate << endl;

	//cout << "+" << Level;
	//cout << "-" << rhs.Level;
	/*
	   if( Level != rhs.Level )
	   return true;

	   if(Coordinate != rhs.Coordinate)
	   return true;

	   return false;
	 */
	return (Level != rhs.Level || Coordinate != rhs.Coordinate);
}

/* comparitor for stl key usage */
bool OmMipChunkCoord::operator<(const OmMipChunkCoord & rhs) constconst
{
	if (Level != rhs.Level)
		return (Level < rhs.Level);
	return (Coordinate < rhs.Coordinate);
}

ostream & operator<<(ostream & out, const OmMipChunkCoord & in)
{
	out << "[ " << in.Level;
	out << " ( " << in.Coordinate.x << " " << in.Coordinate.y << " " << in.Coordinate.z << " ) ]";
}
