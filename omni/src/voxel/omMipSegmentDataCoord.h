#ifndef OM_MIP_SEGMENT_DATA_COORD_H
#define OM_MIP_SEGMENT_DATA_COORD_H

/*
 *	Comparable coordinate for data values within a specific MipCoordinate.  
 *	This MipCoordinate and data value pair can represent the region of a segment
 *	contained within a MipChunk.
 *
 *	Brett Warne - bwarne@mit.edu - 5/28/09
 */


#include "common/omStd.h"
#include "volume/omMipChunkCoord.h"

class OmMipSegmentDataCoord {

 public:
	OmMipSegmentDataCoord() {
		MipChunkCoord = OmMipChunkCoord();
		DataValue = 0;
	}
	
	OmMipSegmentDataCoord( const OmMipChunkCoord& rMipChunkCoord, SEGMENT_DATA_TYPE dataValue)
	: MipChunkCoord(rMipChunkCoord), DataValue(dataValue) { }
	
	
	//operators
	void operator=( const OmMipSegmentDataCoord& rhs );
	bool operator==( const OmMipSegmentDataCoord& rhs ) const;
	bool operator!=( const OmMipSegmentDataCoord& rhs ) const;
	bool operator<( const OmMipSegmentDataCoord& rhs ) const;

	
	//data members
	OmMipChunkCoord MipChunkCoord;
	SEGMENT_DATA_TYPE DataValue;
		
	//stream
	friend ostream& operator<<(ostream &out, const OmMipSegmentDataCoord &in);
	
	friend QDataStream &operator<<(QDataStream & out, const OmMipSegmentDataCoord & c );
	friend QDataStream &operator>>(QDataStream & in, OmMipSegmentDataCoord & c );

};

#endif
