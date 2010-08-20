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

#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "system/cache/omCacheableBase.h"
#include "utility/image/omImage.hpp"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"

#include <QMutex>

class vtkImageData;
class OmVolumeCuller;
class OmSegmentCache;
class OmChunkData;
class OmMipVolume;

class OmMipChunk : public OmCacheableBase {

public:
	OmMipChunk(const OmMipChunkCoord &rMipCoord, OmMipVolume *pMipVolume);
	virtual ~OmMipChunk();

	void Open();
	void OpenForWrite();
	void Flush();
	void Close();
	bool IsOpen();

	//properties
	const DataBbox& GetExtent();

	//dirty accessors
	bool IsDirty();
	bool IsVolumeDataDirty();
	bool IsMetaDataDirty();


	template <typename T> void RawWriteChunkData(T* data){
		QMutexLocker locker(&mOpenLock);
		mHDF5data = OmDataWrapper<T>::producenofree(data);
		writeHDF5();
	}
	OmDataWrapperPtr RawReadChunkDataHDF5();
	bool mIsRawChunkOpen;
	OmDataWrapperPtr mHDF5data;
	void copyInTile(const int sliceOffset, uchar* bits);
	void dealWithCrazyNewStuff();
	bool mIsRawMappedChunkOpen;
	OmDataWrapperPtr mRawMappedChunk;
	void copyChunkFromMemMapToHDF5();
	void copyDataFromHDF5toMemMap();

	OmImage<uint32_t, 3> getOmImage32Chunk();

	//data accessors
	virtual quint32 GetVoxelValue(const DataCoord &vox);
	virtual void SetVoxelValue(const DataCoord &vox, quint32 value);
	vtkImageData* GetImageData();
	OmDataWrapperPtr GetImageDataWrapper();
	void GetBounds(float & maxout, float & minout);
	void SetImageData(OmDataWrapperPtr data);


	//meta data io
	virtual void ReadVolumeData();
	virtual void WriteVolumeData();
	void ReadMetaData();
	void WriteMetaData();


	//meta data accessors
	const OmSegIDsSet & GetModifiedVoxelValues();
	void ClearModifiedVoxelValues();


	//mipchunk data accessors
	const OmSegIDsSet & GetDirectDataValues();
	OmSegSizeMapPtr RefreshDirectDataValues(const bool computeSizes);

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
        Vector2i GetSliceDims();
        void * ExtractDataSlice(const ViewType, int offset);

	//meshing
        OmImage<uint32_t, 3> GetMeshOmImageData();

	bool ContainsVoxel(const DataCoord &vox);
	const Vector3<int> GetDimensions();

	boost::unordered_map< OmSegID, DataBbox> & GetDirectDataBounds() { return mBounds; }

protected:
	bool mIsOpen;
	void SetOpen(bool);

	mutable QMutex mOpenLock;
	mutable QMutex mDirectDataValueLock;
	int mEstMemBytes;
	virtual void InitChunk(const OmMipChunkCoord &rMipCoord);

	//mip volume this chunk belongs to
	OmMipVolume * const mpMipVolume;

	//cache direct and indirectly contained values for drawing tree
	bool containedValuesDataLoaded;
	void loadMetadataIfPresent();
	OmSegIDsSet mDirectlyContainedValues;

	//keep track what needs to be written out
	bool mChunkVolumeDataDirty;
	bool mChunkMetaDataDirty;

	void setVolDataDirty();
	void setMetaDataDirty();
	void setVolDataClean();
	void setMetaDataClean();

	//octree properties
	DataBbox mDataExtent;
	NormBbox mNormExtent;		// extent of chunk in norm space
	NormBbox mClippedNormExtent;	// extent of contained data in norm space
	OmMipChunkCoord mCoordinate;
	OmMipChunkCoord mParentCoord;
	set<OmMipChunkCoord> mChildrenCoordinates;

	//chunk properties
	string mFileName;
	string mDirectoryPath;

	//voxel management
	OmSegIDsSet mModifiedVoxelValues;

 private:
	//FIXME: remove once downsmapling no longer using VTK stuff
	OmDataWrapperPtr mData;

	boost::shared_ptr<OmChunkData> mChunkData;

        OmSegBounds mBounds;
	void addVoxelToBounds(const OmSegID val, const Vector3i & voxelPos);
	void writeHDF5();

	friend class OmMipVolume;
	friend class OmChunkData;
	friend class RefreshDirectDataValuesVisitor;
	friend QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
};

#endif
