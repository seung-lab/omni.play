#ifndef OM_MIP_MESH_COORD_H
#define OM_MIP_MESH_COORD_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "segment/omSegmentTypes.h"

#include "common/omStd.h"
#include "volume/omMipChunkCoord.h"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

class OmMipMeshCoord {
 public:
	OmMipMeshCoord();
	OmMipMeshCoord(const OmMipChunkCoord &, SEGMENT_DATA_TYPE);
	OmMipMeshCoord(const tuple < int, int, int, int >&, SEGMENT_DATA_TYPE);

	void operator=(const OmMipMeshCoord & rhs);
	bool operator==(const OmMipMeshCoord & rhs) const;
	bool operator!=(const OmMipMeshCoord & rhs) const;
	bool operator<(const OmMipMeshCoord & rhs) const;

	OmMipChunkCoord MipChunkCoord;
	SEGMENT_DATA_TYPE DataValue;

	friend ostream & operator<<(ostream & out, const OmMipMeshCoord & in);

	friend class boost::serialization::access;
	template < class Archive > void serialize(Archive & ar, const unsigned int file_version);
};

#pragma mark
#pragma mark Serialization
/////////////////////////////////
///////          Serialization

BOOST_CLASS_VERSION(OmMipMeshCoord, 0)

template < class Archive > void OmMipMeshCoord::serialize(Archive & ar, const unsigned int file_version)
{

	ar & MipChunkCoord;
	ar & DataValue;
}

#endif
