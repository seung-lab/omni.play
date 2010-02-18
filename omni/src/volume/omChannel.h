#ifndef OM_CHANNEL_H
#define OM_CHANNEL_H

/*
 *	OmChannel is the MIP data structure for a channel of raw data in a volume. 
 *
 *	Brett Warne - bwarne@mit.edu - 2/6/09
 */


#include "omMipVolume.h"

#include "system/omManageableObject.h"
#include "system/omSystemTypes.h"
#include "volume/omFilter2d.h"
#include "volume/omFilter2dManager.h"
#include "common/omStd.h"


class OmChannel : public OmMipVolume, public OmManageableObject {

public:
	OmChannel() { }
	OmChannel(OmId id);
	
	//properties
	void SetHue(const Vector3f &);
	const Vector3f& GetHue();
	
	//accessor
	bool IsVolumeDataBuilt();
	void BuildVolumeData();
	
	void Print();
	OmFilter2d& AddFilter();
	OmFilter2d& GetFilter(OmId id);
	const set<OmId>& GetValidFilterIds();
	bool IsFilterEnabled(OmId id);
	bool IsFilterValid(const OmId id);


	
protected:
	//protected copy constructor and assignment operator to prevent copy
	OmChannel(const OmChannel&);
	OmChannel& operator= (const OmChannel&);
	
private:
	Vector3f mHueColor;
	OmFilter2dManager mFilter2dManager;
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};







#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmChannel, 0)

template<class Archive>
void 
OmChannel::serialize(Archive & ar, const unsigned int file_version) {
	ar & boost::serialization::base_object<OmMipVolume>(*this);
	ar & boost::serialization::base_object<OmManageableObject>(*this);
	ar & mFilter2dManager;
}


#endif
