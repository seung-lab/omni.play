#ifndef OM_CHANNEL_H
#define OM_CHANNEL_H

/*
 *	OmChannel is the MIP data structure for a raw data volume
 *
 *	Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "volume/omVolume.h"
#include "volume/omMipVolume.h"
#include "system/omManageableObject.h"
#include "volume/omFilter2d.h"
#include "volume/omFilter2dManager.h"
#include "common/omStd.h"

class OmVolumeData;

class OmChannel : public OmMipVolume, public OmManageableObject {

public:
	OmChannel();
	OmChannel(OmId id);
	~OmChannel();

	boost::shared_ptr<OmVolumeData> getVolData();

	std::string GetName();
	std::string GetDirectoryPath();
	void loadVolData();
	ObjectType getVolumeType(){ return CHANNEL; }
	OmId getID(){ return GetID(); }
	OmMipVolumeCache* getDataCache(){ return mDataCache; }

	void CloseDownThreads();

	void BuildVolumeData();

	OmFilter2d& AddFilter();
	OmFilter2d& GetFilter(OmId id);
	const OmIDsSet & GetValidFilterIds();
	bool IsFilterEnabled(OmId id);
	bool IsFilterValid(const OmId id);

	bool ImportSourceData(const OmDataPath& dataset);

protected:
	//protected copy constructor and assignment operator to prevent copy
	OmChannel(const OmChannel&);
	OmChannel& operator= (const OmChannel&);

	OmDataWrapperPtr doExportChunk(const OmMipChunkCoord&);

	virtual void doBuildThreadedVolume();

private:
	OmMipVolumeCache *const mDataCache;

	boost::shared_ptr<OmVolumeData> mVolData;

	OmFilter2dManager mFilter2dManager;

	friend class OmBuildChannel;
	friend class OmChannelChunkBuildTask;
	template <class T> friend class OmVolumeImporter;

	friend QDataStream &operator<<(QDataStream & out, const OmChannel & );
	friend QDataStream &operator>>(QDataStream & in, OmChannel & );
};

#endif
