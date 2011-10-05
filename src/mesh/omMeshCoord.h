#pragma once

/*
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "common/common.h"

class OmMeshCoord {
 public:
	OmMeshCoord();
    OmMeshCoord( const om::chunkCoord&, OmSegID );

	void operator=( const OmMeshCoord& rhs );
	bool operator==( const OmMeshCoord& rhs ) const;
	bool operator!=( const OmMeshCoord& rhs ) const;
	bool operator<( const OmMeshCoord& rhs ) const;

    om::chunkCoord MipChunkCoord;
	OmSegID DataValue;

    const om::chunkCoord& Coord() const {
		return MipChunkCoord;
	}

	OmSegID SegID() const {
		return DataValue;
	}

	friend std::ostream& operator<<(std::ostream &out, const OmMeshCoord &in);
};

