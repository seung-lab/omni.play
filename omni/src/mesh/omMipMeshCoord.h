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
	OmMipMeshCoord( const OmMipChunkCoord&, SEGMENT_DATA_TYPE );
	
	void operator=( const OmMipMeshCoord& rhs );
	bool operator==( const OmMipMeshCoord& rhs ) const;
	bool operator!=( const OmMipMeshCoord& rhs ) const;
	bool operator<( const OmMipMeshCoord& rhs ) const;
		
	OmMipChunkCoord MipChunkCoord;
	SEGMENT_DATA_TYPE DataValue;
	
	friend ostream& operator<<(ostream &out, const OmMipMeshCoord &in);
	
	friend QDataStream &operator<<(QDataStream & out, const OmMipMeshCoord & c );
	friend QDataStream &operator>>(QDataStream & in, OmMipMeshCoord & c );
};

uint qHash(const OmMipMeshCoord & c);

#endif
