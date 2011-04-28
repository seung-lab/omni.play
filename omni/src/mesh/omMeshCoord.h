#ifndef OM_MIP_MESH_COORD_H
#define OM_MIP_MESH_COORD_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "common/omStd.h"
#include "chunks/omChunkCoord.h"

class OmMeshCoord {
 public:
	OmMeshCoord();
	OmMeshCoord( const OmChunkCoord&, OmSegID );

	void operator=( const OmMeshCoord& rhs );
	bool operator==( const OmMeshCoord& rhs ) const;
	bool operator!=( const OmMeshCoord& rhs ) const;
	bool operator<( const OmMeshCoord& rhs ) const;

	OmChunkCoord MipChunkCoord;
	OmSegID DataValue;

	const OmChunkCoord& Coord() const {
		return MipChunkCoord;
	}

	OmSegID SegID() const {
		return DataValue;
	}

	friend std::ostream& operator<<(std::ostream &out, const OmMeshCoord &in);
};

#endif
