#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */


#include "omMipVolume.h"

#include "mesh/omMipMeshManager.h"
#include "voxel/omMipVoxelationManager.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "mesh/meshingManager.h"
#include "system/omGenericManager.h"
#include "system/omManageableObject.h"
#include "system/events/omSystemModeEvent.h"
#include "volume/omSegmentationChunkCoord.h"
#include "common/omStd.h"

class OmVolumeCuller;
class OmSegment;

class OmSegmentation 
: public OmMipVolume, 
	public OmManageableObject, 
	public OmSystemModeEventListener 
{

public:
	OmSegmentation();
	OmSegmentation(OmId id);
	~OmSegmentation();
	
	//data mapping
	OmId GetSegmentIdMappedToValue( SEGMENT_DATA_TYPE value );
	SEGMENT_DATA_TYPE GetValueMappedToSegmentId( OmId );
	
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
	void RebuildChunk(const OmMipChunkCoord &mipCoord, const SegmentDataSet &rEditedVals);
	
	//export
	void ExportDataFilter(vtkImageData *);
	
	//events
	void SystemModeChangeEvent();
							
	//segment management
	OmSegment* GetSegment(OmId id);
	OmSegment* GetSegmentFromValue(SEGMENT_DATA_TYPE id);
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

	OmId GetNumSegments();
	OmId GetNumTopSegments();
	
	bool AreSegmentsSelected();
	
	//drawing
	void Draw(OmVolumeCuller &);
	void DrawChunkRecursive(const OmMipChunkCoord &, 
				OmSegmentIterator iter,
				bool testVis, 
				OmVolumeCuller &);
	void DrawChunk(const OmMipChunkCoord &, QList< OmSegment* > segmentsToDraw, OmVolumeCuller &rCuller);
	void DrawChunkVoxels( const OmMipChunkCoord &, const SegmentDataSet &, const OmBitfield & );
	
	OmMipMeshManager mMipMeshManager;

	void FlushDirtySegments();
	void FlushDend();
	void ReloadDendrogram( const float threshold);

	void ColorTile( SEGMENT_DATA_TYPE * imageData, const int size,
			const bool isSegmentation, unsigned char * data );

protected:
	//protected copy constructor and assignment operator to prevent copy
	OmSegmentation(const OmSegmentation&);
	OmSegmentation& operator= (const OmSegmentation&);
	
	
private:
	void KillCacheThreads();

	MeshingManager * mMeshingMan;
	
	//managers
	OmMipVoxelationManager mMipVoxelationManager;
	OmSegmentCache mSegmentCache;

        quint32 * mDend;
	int mDendSize;
        float * mDendValues;
	int mDendValuesSize;
	int mDendCount;
	float mDendThreshold;

	friend class OmBuildSegmentation;
	friend class OmSegmentCacheImpl;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentation & seg );
};

#endif
