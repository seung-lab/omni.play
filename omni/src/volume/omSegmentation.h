#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "voxel/omMipVoxelationManager.h"
#include "system/omManageableObject.h"
#include "mesh/omMipMeshManager.h"
#include "system/omGroups.h"
#include "volume/omMipVolume.h"
#include "datalayer/omDataWrapper.h"
#include "segment/omSegmentIterator.h"

class MeshingManager;
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
							
	//segment management
	OmSegmentCache * GetSegmentCache(){ return mSegmentCache; }
	
	//group management
 	void SetGroup(const OmSegIDsSet & set, OmSegIDRootType type, OmGroupName name);
	void UnsetGroup(const OmSegIDsSet & set, OmSegIDRootType type, OmGroupName name);
	void DeleteGroup(OmSegID = 0);

	//drawing
	void DrawChunkVoxels( const OmMipChunkCoord &, const OmSegIDsSet &, const OmBitfield & );
	
	OmMipMeshManager mMipMeshManager;

	void FlushDirtySegments();
	void FlushDend();
	void FlushDendUserEdges();
	void SetDendThreshold( float t );
	void SetDendThresholdAndReload( const float t );
	float GetDendThreshold(){ return mDendThreshold; }
	
private:
	void KillCacheThreads();

	MeshingManager * mMeshingMan;
	
	//managers
	OmMipVoxelationManager mMipVoxelationManager;
	OmSegmentCache * mSegmentCache;

	OmGroups mGroups;

        OmDataWrapperPtr mDend;
        OmDataWrapperPtr mDendValues;
	OmDataWrapperPtr mEdgeDisabledByUser;
	OmDataWrapperPtr mEdgeWasJoined;
	OmDataWrapperPtr mEdgeForceJoin;
	int mDendSize;
	int mDendValuesSize;
	int mDendCount;
	float mDendThreshold;

	friend class OmBuildSegmentation;
	friend class OmSegmentCacheImpl;
	friend class OmSegmentCacheImplLowLevel;
	friend class OmSegmentIterator;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentation & seg );
};

#endif
