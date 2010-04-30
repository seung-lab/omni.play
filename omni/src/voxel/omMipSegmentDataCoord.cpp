#include "omMipSegmentDataCoord.h"

/////////////////////////////////
///////		 Operators

void 
OmMipSegmentDataCoord::operator=( const OmMipSegmentDataCoord& rhs ) {
	MipChunkCoord = rhs.MipChunkCoord;
	DataValue = rhs.DataValue;
}

bool 
OmMipSegmentDataCoord::operator==( const OmMipSegmentDataCoord& rhs ) const {
	return MipChunkCoord == rhs.MipChunkCoord && DataValue == rhs.DataValue;
}

bool 
OmMipSegmentDataCoord::operator!=( const OmMipSegmentDataCoord& rhs ) const {
	return MipChunkCoord != rhs.MipChunkCoord || DataValue != rhs.DataValue;
}

/* comparitor for key usage */
bool 
OmMipSegmentDataCoord::operator<( const OmMipSegmentDataCoord& rhs ) const 
{ 
	if(MipChunkCoord != rhs.MipChunkCoord) return (MipChunkCoord <  rhs.MipChunkCoord);
	return (DataValue < rhs.DataValue);
}	

/////////////////////////////////
///////		 stream

ostream& 
operator<<(ostream &out, const OmMipSegmentDataCoord &in) {
	out << in.MipChunkCoord << " { " << (int) in.DataValue << " } ";
	return out;
}

uint qHash(const OmMipSegmentDataCoord& c)
{
	return qHash( QString("%1_%2")
		      .arg(c.DataValue)
		      .arg(qHash(c.MipChunkCoord)));
}
