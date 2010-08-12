#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "datalayer/omMST.h"
#include "system/omManageableObject.h"
#include "mesh/omMipMeshManager.h"
#include "system/omGroups.h"
#include "volume/omMipVolume.h"
#include "datalayer/omDataWrapper.h"
#include "segment/omSegmentIterator.h"

class OmSegmentLists;
class OmSegment;
class OmSegmentCache;
class OmSegmentIterator;
class OmSegmentationChunkCoord;
class OmViewGroupState;
class OmVolumeCuller;

class OmSegmentation : public OmMipVolume, public OmManageableObject {
 public:
	OmSegmentation();
	OmSegmentation(OmId id);
	~OmSegmentation();

	void CloseDownThreads();

	//build methods
	void BuildVolumeData();

	bool BuildThreadedVolume();
	bool BuildThreadedSegmentation();

	void BuildMeshData();
	void BuildMeshDataPlan(const QString &);
	void BuildMeshChunk(int level, int x, int y, int z, int numThreads = 0);
	void BuildMeshDataInternal();
	void QueueUpMeshChunk(OmSegmentationChunkCoord chunk_coord );
	void RunMeshQueue();

	void BuildChunk( const OmMipChunkCoord &mipCoord, bool remesh = false);
	void RebuildChunk(const OmMipChunkCoord &mipCoord, const OmSegIDsSet &rEditedVals);

	//export
	void ExportDataFilter(OmDataWrapperPtr);

	//segment management
	boost::shared_ptr<OmSegmentCache> GetSegmentCache(){ return mSegmentCache; }
	boost::shared_ptr<OmSegmentLists> getSegmentLists(){ return mSegmentLists; }

	//group management
        OmGroups * GetGroups(){ return &mGroups; }
 	void SetGroup(const OmSegIDsSet & set, OmSegIDRootType type, OmGroupName name);
	void UnsetGroup(const OmSegIDsSet & set, OmSegIDRootType type, OmGroupName name);
	void DeleteGroup(OmSegID = 0);

	OmMipMeshManager mMipMeshManager;

	void FlushDirtySegments();
	void FlushDend();
	void FlushDendUserEdges();
	void SetDendThreshold( float t );
	void SetDendThresholdAndReload( const float t );
	float GetDendThreshold(){ return mst.mDendThreshold; }

	Vector3<int> FindCenterOfSelectedSegments();

	bool ImportSourceData(OmDataPath & dataset);

private:
	void KillCacheThreads();

	boost::shared_ptr<OmSegmentCache> mSegmentCache;
	boost::shared_ptr<OmSegmentLists> mSegmentLists;

	OmGroups mGroups;

	OmMST mst;

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
