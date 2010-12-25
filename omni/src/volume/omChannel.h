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
#include "volume/omFilter2dManager.h"
#include "common/omStd.h"

class OmVolumeData;

class OmChannel : public OmMipVolume, public OmManageableObject {

public:
	OmChannel();
	OmChannel(OmID id);
	~OmChannel();

	boost::shared_ptr<OmVolumeData> getVolData();

	std::string GetName();
	std::string GetDirectoryPath();
	void loadVolData();
	ObjectType getVolumeType(){ return CHANNEL; }
	OmID getID(){ return GetID(); }
	OmMipVolumeCache* getDataCache(){ return mDataCache; }
	int GetBytesPerSample() const;

	void CloseDownThreads();

	OmFilter2d& AddFilter();
	OmFilter2d& GetFilter(OmID id);
	const OmIDsSet & GetValidFilterIds();
	bool IsFilterEnabled(OmID id);
	bool IsFilterValid(const OmID id);

	void SetVolDataType(const OmVolDataType);

	virtual void SetAffinity(const om::Affinity aff) { mAffinity = aff; }
	om::Affinity GetAffinity() { return mAffinity; }
protected:
	//protected copy constructor and assignment operator to prevent copy
	OmChannel(const OmChannel&);
	OmChannel& operator= (const OmChannel&);

	OmDataWrapperPtr doExportChunk(const OmMipChunkCoord&,
								   const bool rerootSegments);

	virtual void doBuildThreadedVolume();

private:
	OmMipVolumeCache *const mDataCache;

	boost::shared_ptr<OmVolumeData> mVolData;

	OmFilter2dManager mFilter2dManager;

	om::Affinity mAffinity;

	friend class OmBuildChannel;
	template <class T> friend class OmVolumeBuilder;
	friend class OmChannelChunkBuildTask;
	template <class T> friend class OmVolumeImporter;

	friend QDataStream &operator<<(QDataStream & out, const OmChannel & );
	friend QDataStream &operator>>(QDataStream & in, OmChannel & );
};

#endif
