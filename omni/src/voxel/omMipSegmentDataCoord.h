#ifndef OM_MIP_SEGMENT_DATA_COORD_H
#define OM_MIP_SEGMENT_DATA_COORD_H

/*
 *	Comparable coordinate for data values within a specific MipCoordinate.  
 *	This MipCoordinate and data value pair can represent the region of a segment
 *	contained within a MipChunk.
 *
 *	Brett Warne - bwarne@mit.edu - 5/28/09
 */

#include "segment/omSegmentTypes.h"

#include "common/omStd.h"
#include "volume/omMipChunkCoord.h"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

class OmMipSegmentDataCoord {
 public:
	OmMipSegmentDataCoord() {
		MipChunkCoord = OmMipChunkCoord();
		DataValue = -1;
	} OmMipSegmentDataCoord(const OmMipChunkCoord & rMipChunkCoord, SEGMENT_DATA_TYPE dataValue)
	:MipChunkCoord(rMipChunkCoord), DataValue(dataValue) {
	}

	//operators
	void operator=(const OmMipSegmentDataCoord & rhs);
	bool operator==(const OmMipSegmentDataCoord & rhs) const;
	bool operator!=(const OmMipSegmentDataCoord & rhs) const;
	bool operator<(const OmMipSegmentDataCoord & rhs) const;

	//data members
	OmMipChunkCoord MipChunkCoord;
	SEGMENT_DATA_TYPE DataValue;

	//stream
	friend ostream & operator<<(ostream & out, const OmMipSegmentDataCoord & in);

	//serialization
	friend class boost::serialization::access;
	template < class Archive > void serialize(Archive & ar, const unsigned int file_version);
};

#pragma mark
#pragma mark Operators
/////////////////////////////////
///////          Operators

inline void
 OmMipSegmentDataCoord::operator=(const OmMipSegmentDataCoord & rhs)
{
	MipChunkCoord = rhs.MipChunkCoord;
	DataValue = rhs.DataValue;
}

inline bool OmMipSegmentDataCoord::operator==(const OmMipSegmentDataCoord & rhs) const const
{
	return MipChunkCoord == rhs.MipChunkCoord && DataValue == rhs.DataValue;
}

inline bool OmMipSegmentDataCoord::operator!=(const OmMipSegmentDataCoord & rhs) constconst
{
	return MipChunkCoord != rhs.MipChunkCoord || DataValue != rhs.DataValue;
}

/* comparitor for key usage */
inline bool OmMipSegmentDataCoord::operator<(const OmMipSegmentDataCoord & rhs) constconst
{
	if (MipChunkCoord != rhs.MipChunkCoord)
		return (MipChunkCoord < rhs.MipChunkCoord);
	return (DataValue < rhs.DataValue);
}

#pragma mark
#pragma mark stream
/////////////////////////////////
///////          stream

inline ostream & operator<<(ostream & out, const OmMipSegmentDataCoord & in)
{
	out << in.MipChunkCoord << " { " << (int)in.DataValue << " } ";
}

#pragma mark
#pragma mark Serialization
/////////////////////////////////
///////          Serialization

BOOST_CLASS_VERSION(OmMipSegmentDataCoord, 0)

template < class Archive > void OmMipSegmentDataCoord::serialize(Archive & ar, const unsigned int file_version)
{

	ar & MipChunkCoord;
	ar & DataValue;
}

#endif
