#ifndef OM_TILE_COORD_H
#define OM_TILE_COORD_H

/*
 *	An OmTileCoord represents a location in Spatial space given by a level (or mip resolution),
 *	and an x,y,z coordinate.  This is stored in a four element tuple as [level, (x,y,z)].
 
*	Rachel Shearer - rshearer@mit.edu
 */

#include "common/omStd.h"

#include "common/omStd.h"
#include "common/omSerialization.h"

#include "system/omSystemTypes.h"
#include "volume/omVolumeTypes.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

class OmTileCoord {

 public:
	OmTileCoord();
	OmTileCoord(int, const SpaceCoord &);

	void operator=(const OmTileCoord & rhs);
	bool operator==(const OmTileCoord & rhs) const;
	bool operator!=(const OmTileCoord & rhs) const;
	bool operator<(const OmTileCoord & rhs) const;

	int Level;
	SpaceCoord Coordinate;

	friend ostream & operator<<(ostream & out, const OmTileCoord & in);

	friend class boost::serialization::access;
	template < class Archive > void serialize(Archive & ar, const unsigned int file_version);
};

#pragma mark
#pragma mark Serialization
/////////////////////////////////
///////          Serialization

BOOST_CLASS_VERSION(OmTileCoord, 0)

template < class Archive > void OmTileCoord::serialize(Archive & ar, const unsigned int file_version)
{

	ar & Level;
	ar & Coordinate;
}

#endif
