#ifndef OM_SIMPLE_CHUNK_H
#define OM_SIMPLE_CHUNK_H

/**
 *	MipChunks are designed to associated to volume data set of static dimensions.  While the data can change,
 *	the size cannot.  This simplifies the interface so as to make it easier to add more power to opening
 *	and closing the volume.  Specifically a MipChunk is associated to a MipChunkCache that keeps track of
 *	the memory usage of a MipChunk.
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "omMipChunkCoord.h"
#include "system/omCacheableBase.h"
#include "omMipChunk.h"
#include <QMutex>

class OmSimpleChunkThreadedCache;
class OmMipVolume;
class OmVolumeCuller;

class OmSimpleChunk : public OmCacheableBase {

public:
	OmSimpleChunk(const OmMipChunkCoord &rMipCoord, OmMipVolume *pMipVolume);
	~OmSimpleChunk();
		
	//overridden datavolume methods so as to notify cache
	void Open();
	void OpenForWrite();
	void Flush();
	void Close();
	bool IsOpen();

	//properties
	const DataBbox& GetExtent();
	int GetSize();
	
	//dirty accessors
	bool IsDirty();
	bool IsVolumeDataDirty();
	bool IsMetaDataDirty();
	
	
	//data accessors
	quint32 GetVoxelValue(const DataCoord &vox);
	void SetVoxelValue(const DataCoord &vox, quint32 value);
	void SetImageData(void *imageData);

	
	//meta data io
	void ReadVolumeData();
	void WriteVolumeData();
	void ReadMetaData();
	void WriteMetaData();
	
	
	//meta data accessors
	const SegmentDataSet& GetModifiedVoxelValues();
	void ClearModifiedVoxelValues();
	
	
	//mipchunk data accessors
	const SegmentDataSet& GetDirectDataValues();
	const SegmentDataSet& GetIndirectDataValues();
	void RefreshDirectDataValues();
	void RefreshIndirectDataValues();
	

	//chunk extent
	const NormBbox& GetNormExtent();
	const NormBbox& GetClippedNormExtent();
	
	
	//mip properties
	int GetLevel();
	bool IsRoot();
	bool IsLeaf();
	const OmMipChunkCoord& GetCoordinate();
	const OmMipChunkCoord& GetParentCoordinate();
	const set<OmMipChunkCoord>& GetChildrenCoordinates();
	
	//slice
	AxisAlignedBoundingBox<int> ExtractSliceExtent(OmDataVolumePlane plane, int coord);
	void* ExtractDataSlice(OmDataVolumePlane plane, int offset, Vector2<int> &sliceDims, bool fast = false);
	
	//meshing
        void* GetMeshImageData();
	
	
	//drawing
	bool DrawCheck( OmVolumeCuller & );
	void DrawClippedExtent();
	
	int GetBytesPerSample();
	bool ContainsVoxel(const DataCoord &vox);
	const Vector3<int> GetDimensions();

private:
	bool mIsOpen;
	void SetOpen(bool);

	QMutex * mLock;
	int mEstMemBytes;
	void InitChunk(const OmMipChunkCoord &rMipCoord);
	
	//mip volume this chunk belongs to
	OmMipVolume * const mpMipVolume;
	
	//image data of chunk
	void *mpImageData;	
		
	//cache direct and indirectly contained values for drawing tree
	bool containedValuesDataLoaded;
	void loadMetadataIfPresent();
	SegmentDataSet mDirectlyContainedValues;	
	SegmentDataSet mIndirectlyContainedValues;
	SegmentDataSet & GetDirectDataValuesInternal();
	SegmentDataSet & GetIndirectDataValuesInternal();	

	//keep track what needs to be written out
	bool mVolumeDataDirty;
	bool mMetaDataDirty;
	
	//octree properties
	DataBbox mDataExtent;
	NormBbox mNormExtent;			//extent of chunk in norm space
	NormBbox mClippedNormExtent;	//extent of contained data in norm space
	OmMipChunkCoord mCoordinate;
	OmMipChunkCoord mParentCoord;
	set<OmMipChunkCoord> mChildrenCoordinates;
		
	//chunk properties
	string mFileName;
	string mDirectoryPath;
	
	//voxel management
	SegmentDataSet mModifiedVoxelValues;

	friend QDataStream &operator<<(QDataStream & out, const OmSimpleChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmSimpleChunk & chunk );
};

#endif
