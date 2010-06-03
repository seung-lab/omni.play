#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "voxel/omMipVoxelationManager.h"
#include "system/omManageableObject.h"
#include "system/events/omSystemModeEvent.h"
#include "mesh/omMipMeshManager.h"
#include "system/omGroups.h"
#include "volume/omMipVolume.h"
#include "datalayer/omDataWrapper.h"

class MeshingManager;
class OmSegment;
class OmSegmentCache;
class OmSegmentIterator;
class OmSegmentationChunkCoord;
class OmViewGroupState;
class OmVolumeCuller;

class OmSegmentation 
: public OmMipVolume, 
	public OmManageableObject, 
	public OmSystemModeEventListener
{

public:
	OmSegmentation();
	OmSegmentation(OmId id);
	~OmSegmentation();
	
	//data accessor
	void SetVoxelValue(const DataCoord &, uint32_t);
	OmId GetVoxelSegmentId(const DataCoord &vox);
	void SetVoxelSegmentId(const DataCoord &vox, OmId omId);

	//build methods
	bool IsVolumeDataBuilt();
	void BuildVolumeData();
	
	void BuildMeshData();
	void BuildMeshDataPlan(const QString &);
	void BuildMeshChunk(int level, int x, int y, int z, int numThreads = 0);
	void BuildMeshDataInternal();	
	void QueueUpMeshChunk(OmSegmentationChunkCoord chunk_coord );
	void RunMeshQueue();

	void BuildChunk( const OmMipChunkCoord &mipCoord);
	void RebuildChunk(const OmMipChunkCoord &mipCoord, const OmSegIDsSet &rEditedVals);
	
	//export
	void ExportDataFilter(vtkImageData *);
	
	//events
	void SystemModeChangeEvent();
							
	//segment management
	OmSegment* GetSegment(OmId id);
	OmSegment* GetSegmentFromValue(OmSegID id);
	OmSegment* AddSegment();
	bool IsSegmentValid(OmId id);
	bool IsSegmentEnabled(OmId id);
	void SetSegmentEnabled(OmId id, bool enable);
	void SetAllSegmentsEnabled(bool selected);
	const OmIDsSet & GetEnabledSegmentIds();
	bool IsSegmentSelected(OmId id);
	void SetSegmentSelected(OmId id, bool selected);
	void SetAllSegmentsSelected(bool selected);
	const OmIDsSet& GetSelectedSegmentIds();
	void JoinTheseSegments( const OmIDsSet & segmentIds);
	void UnJoinTheseSegments( const OmIDsSet & segmentIds);
	void UpdateSegmentSelection( const OmSegIDsSet & idsToSelect);

	OmId GetNumSegments();
	OmId GetNumTopSegments();
	
	bool AreSegmentsSelected();
	
	//group management
	OmId AddGroup(OmSegID = 0);
	void DeleteGroup(OmSegID = 0);


	//drawing
	void DrawChunkVoxels( const OmMipChunkCoord &, const OmSegIDsSet &, const OmBitfield & );
	
	OmMipMeshManager mMipMeshManager;

	void FlushDirtySegments();
	void FlushDend();
	void SetDendThreshold( float t );
	void SetDendThresholdAndReload( const float t );
	float GetDendThreshold(){ return mDendThreshold; }
	void ReloadDendrogram();

	OmSegmentCache * GetSegmentCache(){ return mSegmentCache; }
	
private:
	void KillCacheThreads();

	MeshingManager * mMeshingMan;
	
	//managers
	OmMipVoxelationManager mMipVoxelationManager;
	OmSegmentCache * mSegmentCache;

	OmGroups mGroups;

        OmDataWrapperPtr mDend;
        OmDataWrapperPtr mDendValues;
	int mDendSize;
	int mDendValuesSize;
	int mDendCount;
	float mDendThreshold;

	friend class OmBuildSegmentation;
	friend class OmSegmentCacheImpl;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentation & seg );
};

#endif
