#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "system/omManageableObject.h"
#include "mesh/omMipMeshManager.h"
#include "volume/omMipVolume.h"
#include "datalayer/omDataWrapper.h"
#include "segment/omSegmentIterator.h"

class OmGroups;
class OmMST;
class OmSegmentLists;
class OmSegment;
class OmSegmentCache;
class OmSegmentIterator;
class OmSegmentationChunkCoord;
class OmViewGroupState;
class OmVolumeCuller;
class OmVolumeData;

class OmSegmentation : public OmMipVolume, public OmManageableObject {
public:
	OmSegmentation();
	OmSegmentation(OmId id);
	~OmSegmentation();

	boost::shared_ptr<OmVolumeData> getVolData();

	std::string GetName();
	std::string GetDirectoryPath();
	void loadVolData();
	ObjectType getVolumeType(){ return SEGMENTATION; }
	OmId getID(){ return GetId(); }
	OmMipVolumeCache* getDataCache(){ return mDataCache; }

	void CloseDownThreads();

	void BuildVolumeData();
	void Mesh();

	void RebuildChunk(const OmMipChunkCoord &, const OmSegIDsSet &);

	//segment management
	boost::shared_ptr<OmSegmentCache> GetSegmentCache(){ return mSegmentCache; }
	boost::shared_ptr<OmSegmentLists> GetSegmentLists(){ return mSegmentLists; }

	//group management
	boost::shared_ptr<OmGroups> GetGroups(){ return mGroups; }
 	void SetGroup(const OmSegIDsSet&, OmSegIDRootType, OmGroupName);
	void UnsetGroup(const OmSegIDsSet&, OmSegIDRootType, OmGroupName);
	void DeleteGroup(OmSegID = 0);

	OmMipMeshManager mMipMeshManager;

	void FlushDirtySegments();
	void FlushDend();
	void FlushDendUserEdges();
	void SetDendThreshold( float t );
	void SetDendThresholdAndReload( const float t );
	float GetDendThreshold();
	boost::shared_ptr<OmMST> getMST();

	Vector3i FindCenterOfSelectedSegments();

	bool ImportSourceData(OmDataPath & dataset);

protected:
	virtual void doBuildThreadedVolume();

private:
	OmMipVolumeCache *const mDataCache;

	void KillCacheThreads();

	boost::shared_ptr<OmVolumeData> mVolData;
	boost::shared_ptr<OmSegmentCache> mSegmentCache;
	boost::shared_ptr<OmSegmentLists> mSegmentLists;

	boost::shared_ptr<OmGroups> mGroups;

	boost::shared_ptr<OmMST> mst_;

	OmDataWrapperPtr doExportChunk(const OmMipChunkCoord &);

	friend class OmBuildSegmentation;
	template <class T> friend class OmVolumeImporter;

	friend class OmSegmentCacheImpl;
	friend class OmSegmentCacheImplLowLevel;
	friend class OmSegmentIterator;
	friend class MstViewerImpl;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentation & seg );
};

#endif
