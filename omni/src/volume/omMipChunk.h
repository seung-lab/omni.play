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

class OmVolumeCuller;
class OmSegmentCache;
class OmChunkData;
class OmMipVolume;

class OmMipChunk : public OmCacheableBase {

public:
	OmMipChunk(const OmMipChunkCoord &rMipCoord, OmMipVolume *pMipVolume);
	virtual ~OmMipChunk();

	int GetNumberOfVoxelsInChunk() const;

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
		mHDF5data = OmDataWrapper<T>::produceNoFree(data);
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
	void copyDataFromHDF5toMemMap(OmDataWrapperPtr hdf5);

	OmImage<uint32_t, 3> getOmImage32Chunk();

	//data accessors
	virtual quint32 GetVoxelValue(const DataCoord &vox);
	virtual void SetVoxelValue(const DataCoord &vox, quint32 value);
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
	void RefreshDirectDataValues(const bool,
				     boost::shared_ptr<OmSegmentCache>);

	//chunk extent
	const NormBbox& GetNormExtent();
	const NormBbox& GetClippedNormExtent();

	//mip properties
	int GetLevel();
	bool IsRoot();
	bool IsLeaf();
	const OmMipChunkCoord& GetCoordinate();
	const OmMipChunkCoord& GetParentCoordinate();
	const std::set<OmMipChunkCoord>& GetChildrenCoordinates();

	//slice
        boost::shared_ptr<uint8_t>  ExtractDataSlice8bit(const ViewType, const int);
	boost::shared_ptr<uint32_t> ExtractDataSlice32bit(const ViewType, const int);

	//meshing
        OmImage<uint32_t, 3> GetMeshOmImageData();

	bool ContainsVoxel(const DataCoord &vox);
	const Vector3i GetDimensions();

	bool compare(OmMipChunkPtr other);

protected:
	OmMipVolume *const mpMipVolume;
	bool mIsOpen;

	bool containedValuesDataLoaded;
	bool mChunkVolumeDataDirty;
	bool mChunkMetaDataDirty;

	void SetOpen(bool);

	mutable QMutex mOpenLock;
	mutable QMutex mDirectDataValueLock;
	virtual void InitChunk(const OmMipChunkCoord &rMipCoord);

	//cache direct and indirectly contained values for drawing tree
	void loadMetadataIfPresent();
	OmSegIDsSet mDirectlyContainedValues;

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
	std::set<OmMipChunkCoord> mChildrenCoordinates;

	//chunk properties
	std::string mFileName;
	std::string mDirectoryPath;

	//voxel management
	OmSegIDsSet mModifiedVoxelValues;

 private:
	//FIXME: remove once downsmapling no longer using VTK stuff
	OmDataWrapperPtr mData;

	boost::shared_ptr<OmChunkData> mChunkData;

	void writeHDF5();

	friend class OmMipVolume;
	friend class OmChunkData;
	friend class ProcessChunkVoxel;
	friend QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
};

#endif
