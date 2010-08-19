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

	std::string GetName();
	std::string GetDirectoryPath();
	void loadVolData();

	//properties
	void SetHue(const Vector3f &);
	const Vector3f& GetHue();

	void CloseDownThreads();

	//accessor
	bool IsVolumeDataBuilt();
	void BuildVolumeData();
	bool BuildThreadedVolume();
	bool BuildThreadedChannel();
	void BuildChunk(const OmMipChunkCoord&);

	void Print();
	OmFilter2d& AddFilter();
	OmFilter2d& GetFilter(OmId id);
	const OmIDsSet & GetValidFilterIds();
	bool IsFilterEnabled(OmId id);
	bool IsFilterValid(const OmId id);

	bool GetBounds(float & mx, float & mn) { if(mWasBounded) {mx = mMaxVal; mn = mMinVal;} return mWasBounded;}

	bool ImportSourceData(OmDataPath & dataset);

protected:
	//protected copy constructor and assignment operator to prevent copy
	OmChannel(const OmChannel&);
	OmChannel& operator= (const OmChannel&);

private:
	Vector3f mHueColor;
	OmFilter2dManager mFilter2dManager;

	float mMaxVal;
	float mMinVal;
	bool mWasBounded;

	friend class OmBuildChannel;
	template <class T> friend class OmVolumeImporter;

	friend QDataStream &operator<<(QDataStream & out, const OmChannel & );
	friend QDataStream &operator>>(QDataStream & in, OmChannel & );
};

#endif
