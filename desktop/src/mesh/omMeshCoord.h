#pragma once

#include "common/common.h"

class OmMeshCoord {
 public:
	OmMeshCoord();
    OmMeshCoord( const om::coords::Chunk&, OmSegID );

	void operator=( const OmMeshCoord& rhs );
	bool operator==( const OmMeshCoord& rhs ) const;
	bool operator!=( const OmMeshCoord& rhs ) const;
	bool operator<( const OmMeshCoord& rhs ) const;

    om::coords::Chunk MipChunkCoord;
	OmSegID DataValue;

    const om::coords::Chunk& Coord() const {
		return MipChunkCoord;
	}

	OmSegID SegID() const {
		return DataValue;
	}

	friend std::ostream& operator<<(std::ostream &out, const OmMeshCoord &in);
};

