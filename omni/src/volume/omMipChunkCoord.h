#ifndef OM_MIP_CHUNK_COORD_H
#define OM_MIP_CHUNK_COORD_H

/*
 *	OmMipChunkCoord represents a location in Mip Space given by a level (or mip resolution),
 *	and an x,y,z coordinate.  This is stored in a four element tuple as [level, (x,y,z)].
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "common/omCommon.h"

class OmMipChunkCoord {

public:
	OmMipChunkCoord();
	OmMipChunkCoord( int, const DataCoord &);
	OmMipChunkCoord( int level, int, int, int );

	static const OmMipChunkCoord NULL_COORD;

	QString getCoordsAsString() const;

	//property
	bool IsLeaf() const;
	
	//family coordinate methods
	OmMipChunkCoord	ParentCoord() const;
	OmMipChunkCoord PrimarySiblingCoord() const;
	void SiblingCoords(OmMipChunkCoord *pSiblings) const;
	OmMipChunkCoord PrimaryChildCoord() const;
	void ChildrenCoords(OmMipChunkCoord *pChildren) const;
	
	//access
	const int & getLevel(){ return Level; }
	const int & getCoordinateX(){ return Coordinate.x; }
	const int & getCoordinateY(){ return Coordinate.y; }
	const int & getCoordinateZ(){ return Coordinate.z; }
	
	//operators
	void operator=( const OmMipChunkCoord& rhs );
	bool operator==( const OmMipChunkCoord& rhs ) const;
	bool operator!=( const OmMipChunkCoord& rhs ) const;
	bool operator<( const OmMipChunkCoord& rhs ) const;
	
	int Level;
	DataCoord Coordinate;
	
	friend QDataStream &operator<<(QDataStream & out, const OmMipChunkCoord & c );
	friend QDataStream &operator>>(QDataStream & in, OmMipChunkCoord & c );
};

#endif
