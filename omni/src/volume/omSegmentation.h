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
	public OmSystemModeEventListener,
	boost::noncopyable
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
	void RebuildChunk(const OmMipChunkCoord &mipCoord, const OmSegIDs &rEditedVals);
	
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
	const OmIds & GetEnabledSegmentIds();
	bool IsSegmentSelected(OmId id);
	void SetSegmentSelected(OmId id, bool selected);
	void SetAllSegmentsSelected(bool selected);
	const OmIds& GetSelectedSegmentIds();
	void JoinAllSegmentsInSelectedList();

	OmId GetNumSegments();
	OmId GetNumTopSegments();
	
	bool AreSegmentsSelected();
	
	//group management
	OmId AddGroup();


	//drawing
	void Draw(OmVolumeCuller &, OmViewGroupState * vgs);
	void DrawChunkRecursive(const OmMipChunkCoord &, 
				OmSegmentIterator iter,
				bool testVis, 
				OmVolumeCuller &);
	void DrawChunk(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
		       const OmMipChunkCoord & chunkCoord,
		       const OmSegPtrs & segmentsToDraw, 
		       OmVolumeCuller &rCuller);
	void DrawChunkVoxels( const OmMipChunkCoord &, const OmSegIDs &, const OmBitfield & );
	
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
	OmViewGroupState * mViewGroupState;

        quint32 * mDend;
        float * mDendValues;
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
