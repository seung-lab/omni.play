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
#include "datalayer/omIDataWriter.h"
#include "utility/image/omImage.hpp"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"

#include "zi/omMutex.h"

class OmMipVolumeCache;
class OmVolumeCuller;
class OmSegmentCache;
class OmChunkData;
class OmMipVolume;

class OmMipChunk {
public:
	OmMipChunk(const OmMipChunkCoord &coord, OmMipVolume* vol);
	virtual ~OmMipChunk(){}

	void Flush(){}

	bool ContainsVoxel(const DataCoord &vox);
	const Vector3i GetDimensions();
	int GetNumberOfVoxelsInChunk() const;
	const DataBbox& GetExtent();

	OmDataWrapperPtr RawReadChunkDataHDF5();
	void copyInTile(const int sliceOffset, uchar* bits);
	void copyDataFromHDF5toMemMap();
	void copyInChunkData(OmDataWrapperPtr hdf5);
	OmImage<uint32_t, 3> GetCopyOfChunkDataAsOmImage32();
	boost::shared_ptr<uint8_t>  ExtractDataSlice8bit(const ViewType, const int);
	boost::shared_ptr<uint32_t> ExtractDataSlice32bit(const ViewType, const int);
	OmImage<uint32_t, 3> GetMeshOmImageData();
	bool compare(OmMipChunkPtr other);

	virtual uint32_t GetVoxelValue(const DataCoord &vox);
	virtual void SetVoxelValue(const DataCoord &vox, uint32_t value);

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

protected:
	OmMipVolume *const vol_;

	void InitChunk(const OmMipChunkCoord &rMipCoord);

	zi::mutex mDirectDataValueLock;
	void loadMetadataIfPresent();
	OmSegIDsSet mDirectlyContainedValues;
	bool containedValuesDataLoaded;

	//octree properties
	DataBbox mDataExtent;
	NormBbox mNormExtent;		// extent of chunk in norm space
	NormBbox mClippedNormExtent;	// extent of contained data in norm space
	OmMipChunkCoord mCoordinate;
	OmMipChunkCoord mParentCoord;
	std::set<OmMipChunkCoord> mChildrenCoordinates;

	//voxel management
	OmSegIDsSet mModifiedVoxelValues;

private:
	OmMipVolumeCache *const cache_;

	boost::shared_ptr<OmChunkData> mChunkData;

	void ReadMetaData();
	void WriteMetaData();

	friend class OmMipVolume;
	friend class OmChunkData;
	friend class ProcessChunkVoxel;
	friend QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
};

#endif
