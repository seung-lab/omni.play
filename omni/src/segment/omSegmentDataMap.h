#ifndef OM_SEGMENT_DATA_MAP_H
#define OM_SEGMENT_DATA_MAP_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 4/18/09
 */


#include "omSegmentTypes.h"

#include "system/omSystemTypes.h"
#include "common/omSerialization.h"
#include "common/omStd.h"



class OmSegmentDataMap {

public:
	bool IsValueMappedToSegmentId( SEGMENT_DATA_TYPE value );
	SEGMENT_DATA_TYPE GetUnMappedValue();
	
	
	OmId GetSegmentIdMappedToValue( SEGMENT_DATA_TYPE value );
	const SegmentDataSet& GetValuesMappedToSegmentId( OmId );
	
	
	void MapValueToSegmentId( OmId , SEGMENT_DATA_TYPE );
	void MapValuesToSegmentId( OmId, const SegmentDataSet & );
	void UnMapValuesToSegmentId( OmId, const SegmentDataSet & );
	
	
	void UnMapSegmentId( OmId );
	void UnMapValues( const SegmentDataSet & );
	
	
	
private:
	map< SEGMENT_DATA_TYPE, OmId > mDataValueToId;
	map< OmId, SegmentDataSet > mIdToDataValues;
	
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
	
};








/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmSegmentDataMap, 0)

template<class Archive>
void 
OmSegmentDataMap::serialize(Archive & ar, const unsigned int file_version) {
	ar & mDataValueToId;
	ar & mIdToDataValues;
}



#endif
