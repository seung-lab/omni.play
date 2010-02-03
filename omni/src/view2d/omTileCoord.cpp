
#include "omTileCoord.h"
#include "system/omDebug.h"

OmTileCoord::OmTileCoord()
{
	Level = -1;
	Coordinate = SpaceCoord();
}

OmTileCoord::OmTileCoord(int mLevel, const SpaceCoord & pSpaceCoord, int volType)
:Level(mLevel), Coordinate(pSpaceCoord), mVolType (volType)
{

}

void OmTileCoord::operator=(const OmTileCoord & rhs)
{
	Level = rhs.Level;
	Coordinate = rhs.Coordinate;
	mVolType = mVolType;
}

bool OmTileCoord::operator==(const OmTileCoord & rhs) const
{
	return Level == rhs.Level && Coordinate == rhs.Coordinate && mVolType == rhs.mVolType;
}

bool OmTileCoord::operator!=(const OmTileCoord & rhs) const
{
	return Level != rhs.Level || Coordinate != rhs.Coordinate || mVolType != rhs.mVolType;
}

/* comparitor for key usage */
bool OmTileCoord::operator<(const OmTileCoord & rhs) const
{
	if (Level != rhs.Level)
		return (Level < rhs.Level);
	if (mVolType != rhs.mVolType)
		return (mVolType < rhs.mVolType);
	return (Coordinate < rhs.Coordinate);
}

ostream & operator<<(ostream & out, const OmTileCoord & in)
{
	out << "[ " << in.Level;
	out << " ( " << in.Coordinate.x << " " << in.Coordinate.y << " " << in.Coordinate.z << " ) ]";
}
