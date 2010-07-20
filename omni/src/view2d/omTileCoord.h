#ifndef OM_TILE_COORD_H
#define OM_TILE_COORD_H

/*
 *	An OmTileCoord represents a location in Spatial space given by a level (or mip resolution),
 *	and an x,y,z coordinate.  This is stored in a four element tuple as [level, (x,y,z)].
 
*	Rachel Shearer - rshearer@mit.edu
 */

#include "common/omCommon.h"

class OmTileCoord {
	
public:
	OmTileCoord();
	OmTileCoord( int, const SpaceCoord &, ObjectType, unsigned int);
	
	void operator=( const OmTileCoord& rhs );
	bool operator==( const OmTileCoord& rhs ) const;
	bool operator!=( const OmTileCoord& rhs ) const;
	bool operator<( const OmTileCoord& rhs ) const;
		
	int Level;
	SpaceCoord Coordinate;

	ObjectType mVolType;
	unsigned int mFreshness;
		
	friend QDataStream &operator<<(QDataStream & out, const OmTileCoord & c );
	friend QDataStream &operator>>(QDataStream & in, OmTileCoord & c );
};

#endif
