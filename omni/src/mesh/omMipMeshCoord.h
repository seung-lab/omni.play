#ifndef OM_MIP_MESH_COORD_H
#define OM_MIP_MESH_COORD_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "common/omStd.h"
#include "volume/omMipChunkCoord.h"

class OmMipMeshCoord {
 public:
	OmMipMeshCoord();
	OmMipMeshCoord( const OmMipChunkCoord&, OmSegID );

	void operator=( const OmMipMeshCoord& rhs );
	bool operator==( const OmMipMeshCoord& rhs ) const;
	bool operator!=( const OmMipMeshCoord& rhs ) const;
	bool operator<( const OmMipMeshCoord& rhs ) const;

	OmMipChunkCoord MipChunkCoord;
	OmSegID DataValue;

	const OmMipChunkCoord& Coord() const {
		return MipChunkCoord;
	}

	OmSegID SegID() const {
		return DataValue;
	}

	friend std::ostream& operator<<(std::ostream &out, const OmMipMeshCoord &in);
};

#endif
