#ifndef OM_MIP_CHUNK_H
#define OM_MIP_CHUNK_H

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

#include <QMutex>

enum OmDataVolumePlane { VOL_XY_PLANE, VOL_XZ_PLANE, VOL_YZ_PLANE };

class vtkImageData;
class OmMipVolume;
class OmVolumeCuller;
class OmSegmentCache;

class OmMipChunk : public OmCacheableBase {

public:
	OmMipChunk(const OmMipChunkCoord &rMipCoord, OmMipVolume *pMipVolume);
	~OmMipChunk();
		
	//overridden datavolume methods so as to notify cache
	void Open();
	virtual void OpenForWrite();
	void Flush();
	virtual void Close();
	bool IsOpen();

	//properties
	const DataBbox& GetExtent();
	
	//dirty accessors
	bool IsDirty();
	bool IsVolumeDataDirty();
	bool IsMetaDataDirty();
	
	
	//data accessors
	virtual quint32 GetVoxelValue(const DataCoord &vox);
	virtual void SetVoxelValue(const DataCoord &vox, quint32 value);
	void SetImageData(vtkImageData *imageData);

	
	//meta data io
	virtual void ReadVolumeData();
	virtual void WriteVolumeData();
	void ReadMetaData();
	void WriteMetaData();
	
	
	//meta data accessors
	const OmSegIDs & GetModifiedVoxelValues();
	void ClearModifiedVoxelValues();
	
	
	//mipchunk data accessors
	const OmSegIDs & GetDirectDataValues();
	virtual void RefreshDirectDataValues( OmSegmentCache *);
	

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
	virtual void * ExtractDataSlice(OmDataVolumePlane plane, int offset, Vector2<int> &sliceDims, bool fast = false);
	
	//meshing
	vtkImageData* GetMeshImageData();
	
	
	//drawing
	bool DrawCheck( OmVolumeCuller & );
	void DrawClippedExtent();
	
	int GetBytesPerSample();
	bool ContainsVoxel(const DataCoord &vox);
	const Vector3<int> GetDimensions();

protected:
	bool mIsOpen;
	void SetOpen(bool);

	QMutex * mOpenLock;
	int mEstMemBytes;
	void InitChunk(const OmMipChunkCoord &rMipCoord);
	
	//mip volume this chunk belongs to
	OmMipVolume * const mpMipVolume;
	
	//cache direct and indirectly contained values for drawing tree
	bool containedValuesDataLoaded;
	void loadMetadataIfPresent();
	OmSegIDs mDirectlyContainedValues;

	//keep track what needs to be written out
	bool mChunkVolumeDataDirty;
	bool mChunkMetaDataDirty;

	void setVolDataDirty();
	void setMetaDataDirty();
	void setVolDataClean();
	void setMetaDataClean();

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
	OmSegIDs mModifiedVoxelValues;

 private:

	//image data of chunk
	vtkImageData *mpImageData;	

	friend QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
};

#endif
