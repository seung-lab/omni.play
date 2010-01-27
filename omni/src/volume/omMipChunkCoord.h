#ifndef OM_MIP_CHUNK_COORD_H
#define OM_MIP_CHUNK_COORD_H

/*
 *	OmMipChunkCoord represents a location in Mip Space given by a level (or mip resolution),
 *	and an x,y,z coordinate.  This is stored in a four element tuple as [level, (x,y,z)].
 *
 *	The boost tuple comparision header allows tuples to be used within STL containers.
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "omVolumeTypes.h"

#include "common/omStd.h"
#include "common/omSerialization.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

class OmMipChunkCoord {

 public:
	OmMipChunkCoord();
	OmMipChunkCoord(int, const DataCoord &);
	 OmMipChunkCoord(int level, int, int, int);

	//property
	bool IsLeaf() const;

	//family coordinate methods
	OmMipChunkCoord ParentCoord() const;
	OmMipChunkCoord PrimarySiblingCoord() const;
	void SiblingCoords(OmMipChunkCoord * pSiblings) const;
	OmMipChunkCoord PrimaryChildCoord() const;
	void ChildrenCoords(OmMipChunkCoord * pChildren) const;

	//access
	 template < size_t M > int get() const;

	//operators
	void operator=(const OmMipChunkCoord & rhs);
	bool operator==(const OmMipChunkCoord & rhs) const;
	bool operator!=(const OmMipChunkCoord & rhs) const;
	bool operator<(const OmMipChunkCoord & rhs) const;

	int Level;
	DataCoord Coordinate;

	friend ostream & operator<<(ostream & out, const OmMipChunkCoord & in);

	friend class boost::serialization::access;
	 template < class Archive > void serialize(Archive & ar, const unsigned int file_version);

	static const OmMipChunkCoord NULL_COORD;
};

/* backwards support for previous Om Mip Coordinate type */
template < size_t M > int
 OmMipChunkCoord::get() const const
{

	switch (M) {
	case 0:
		return Level;
	case 1:
		return Coordinate.x;
	case 2:
		return Coordinate.y;
	case 3:
		return Coordinate.z;
	}

}

#pragma mark
#pragma mark Serialization
/////////////////////////////////
///////          Serialization

BOOST_CLASS_VERSION(OmMipChunkCoord, 0)

template < class Archive > void OmMipChunkCoord::serialize(Archive & ar, const unsigned int file_version)
{
	ar & Level;
	ar & Coordinate;
}

#endif
