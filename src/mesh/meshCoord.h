#pragma once

/*
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "common/common.h"

class meshCoord {
 public:
	meshCoord();
    meshCoord( const coords::chunkCoord&, segId );

	void operator=( const meshCoord& rhs );
	bool operator==( const meshCoord& rhs ) const;
	bool operator!=( const meshCoord& rhs ) const;
	bool operator<( const meshCoord& rhs ) const;

    coords::chunkCoord MipChunkCoord;
	segId DataValue;

    const coords::chunkCoord& Coord() const {
		return MipChunkCoord;
	}

	segId SegID() const {
		return DataValue;
	}

	friend std::ostream& operator<<(std::ostream &out, const meshCoord &in);
};

