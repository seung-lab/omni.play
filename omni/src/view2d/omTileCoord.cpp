
#include "omTileCoord.h"




OmTileCoord::OmTileCoord() {
	Level = -1;
	Coordinate = SpaceCoord();
}


OmTileCoord::OmTileCoord(int mLevel, const SpaceCoord &pSpaceCoord)
: Level(mLevel), Coordinate(pSpaceCoord) {
		
}








void 
OmTileCoord::operator=( const OmTileCoord& rhs ) {
	Level = rhs.Level;
	Coordinate = rhs.Coordinate;
}

bool 
OmTileCoord::operator==( const OmTileCoord& rhs ) const {
	return Level == rhs.Level && Coordinate == rhs.Coordinate;
}

bool 
OmTileCoord::operator!=( const OmTileCoord& rhs ) const {
	return Level != rhs.Level || Coordinate != rhs.Coordinate;
}

/* comparitor for key usage */
bool 
OmTileCoord::operator<( const OmTileCoord& rhs ) const 
{ 
	if(Level != rhs.Level) return (Level <  rhs.Level);
	return (Coordinate < rhs.Coordinate);
}	



ostream& 
operator<<(ostream &out, const OmTileCoord &in) {
	out << "[ " << in.Level;
	out << " ( " << in.Coordinate.x << " " << in.Coordinate.y << " " << in.Coordinate.z << " ) ]";
}