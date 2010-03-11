
#include "omTileCoord.h"
#include "common/omDebug.h"

OmTileCoord::OmTileCoord()
{
	Level = -1;
	Coordinate = SpaceCoord();
}

OmTileCoord::OmTileCoord(int mLevel, const SpaceCoord & pSpaceCoord, int volType, unsigned int freshness)
:Level(mLevel), Coordinate(pSpaceCoord), mVolType (volType), mFreshness (freshness)
{
}

void OmTileCoord::operator=(const OmTileCoord & rhs)
{
	Level = rhs.Level;
	Coordinate = rhs.Coordinate;
	mVolType = rhs.mVolType;
	mFreshness = rhs.mFreshness;
}

bool OmTileCoord::operator==(const OmTileCoord & rhs) const
{
	return Level == rhs.Level && Coordinate == rhs.Coordinate && mVolType == rhs.mVolType && mFreshness == rhs.mFreshness;
}

bool OmTileCoord::operator!=(const OmTileCoord & rhs) const
{
	return Level != rhs.Level || Coordinate != rhs.Coordinate || mVolType != rhs.mVolType || mFreshness != rhs.mFreshness;
}

/* comparitor for key usage */
bool OmTileCoord::operator<(const OmTileCoord & rhs) const
{
	if (Level != rhs.Level)
		return (Level < rhs.Level);
	if (mVolType != rhs.mVolType)
		return (mVolType < rhs.mVolType);
	if (mFreshness != rhs.mFreshness)
		return (mFreshness < rhs.mFreshness);
	return (Coordinate < rhs.Coordinate);
}

ostream & operator<<(ostream & out, const OmTileCoord & in)
{
	out << "[ " << in.Level;
	out << " ( " << in.Coordinate.x << " " << in.Coordinate.y << " " << in.Coordinate.z << " " << 
		        in.mVolType << " " << in.mFreshness << " ) ]";
	return out;
}
