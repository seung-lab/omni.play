#ifndef OM_CHANNEL_H
#define OM_CHANNEL_H

/*
 *	OmChannel is the MIP data structure for a channel of raw data in a volume. 
 *
 *	Brett Warne - bwarne@mit.edu - 2/6/09
 */


#include "volume/omVolume.h"
#include "volume/omMipVolume.h"
#include "system/omManageableObject.h"
#include "volume/omFilter2d.h"
#include "volume/omFilter2dManager.h"
#include "common/omStd.h"


class OmChannel : public OmMipVolume, public OmManageableObject {

public:
        OmChannel();
	OmChannel(OmId id);
	
	//properties
	void SetHue(const vmml::Vector3< float > &);
	const Vector3f& GetHue();
	
	//accessor
	bool IsVolumeDataBuilt();
	void BuildVolumeData();
	
	void Print();
	OmFilter2d& AddFilter();
	OmFilter2d& GetFilter(OmId id);
	const OmIDsSet & GetValidFilterIds();
	bool IsFilterEnabled(OmId id);
	bool IsFilterValid(const OmId id);
	
protected:
	//protected copy constructor and assignment operator to prevent copy
	OmChannel(const OmChannel&);
	OmChannel& operator= (const OmChannel&);
	
private:
	Vector3f mHueColor;
	OmFilter2dManager mFilter2dManager;

	friend class OmBuildChannel;

	friend QDataStream &operator<<(QDataStream & out, const OmChannel & );
	friend QDataStream &operator>>(QDataStream & in, OmChannel & );
};

#endif
