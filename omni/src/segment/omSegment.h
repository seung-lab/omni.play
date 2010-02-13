#ifndef OM_SEGMENT_H
#define OM_SEGMENT_H

/*
 *	Segment Object
 *	
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "omSegmentTypes.h"
#include "system/omSystemTypes.h"
#include "system/omManageableObject.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;


class OmSegment : public OmManageableObject {

public:
	OmSegment();
	OmSegment(OmId id);
	
	//accessors
	const Vector3<float>& GetColor() const;
	void SetColor(const Vector3<float> &);
	
	//drawing
	void ApplyColor(const OmBitfield &drawOps);
	void Join (OmId segid);

	void set_original_mapped_data_value(const SEGMENT_DATA_TYPE value );
	SEGMENT_DATA_TYPE get_original_mapped_data_value();
	
private:
	//data members
	Vector3<float> mColor;
	OmId mJoinId;
	SEGMENT_DATA_TYPE original_mapped_data_value;
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};



#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmSegment, 2)

template<class Archive>
void 
OmSegment::serialize(Archive & ar, const unsigned int file_version) {
	ar & boost::serialization::base_object<OmManageableObject>(*this);
	
	if (file_version > 0) {
		ar & mJoinId;
	}
	if (file_version > 1) {
		ar & original_mapped_data_value;
	}
	ar & mColor;
}



#endif
