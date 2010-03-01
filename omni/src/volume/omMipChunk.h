#ifndef OM_MIP_CHUNK_H
#define OM_MIP_CHUNK_H

/*
 *	MipChunks are designed to associated to volume data set of static dimensions.  While the data can change,
 *	the size cannot.  This simplifies the interface so as to make it easier to add more power to opening
 *	and closing the volume.  Specifically a MipChunk is associated to a MipChunkCache that keeps track of
 *	the memory usage of a MipChunk.
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "omDataVolume.h"
#include "omVolumeTypes.h"
#include "omMipChunkCoord.h"

#include "segment/omSegmentTypes.h"
#include "system/omSystemTypes.h"
#include "system/omCacheableBase.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
#include <QMutex>
using namespace vmml;


enum OmDataVolumePlane { VOL_XY_PLANE, VOL_XZ_PLANE, VOL_YZ_PLANE };


class vtkImageData;
class OmMipVolume;
class OmVolumeCuller;

class OmMipChunk : public OmDataVolume, protected OmCacheableBase {

public:
	OmMipChunk(const OmMipChunkCoord &rMipCoord, OmMipVolume *pMipVolume);
	~OmMipChunk();
	
	
	//overridden datavolume methods so as to notify cache
	void Open();
	void OpenForWrite();
	void Flush();
	void Close();
	
	//properties
	const DataBbox& GetExtent();
	
	//dirty accessors
	bool IsDirty() const;
	bool IsVolumeDataDirty() const;
	bool IsMetaDataDirty() const;
	
	
	//data accessors
	uint32_t GetVoxelValue(const DataCoord &vox);
	void SetVoxelValue(const DataCoord &vox, uint32_t value);
	void SetImageData(vtkImageData *imageData);

	
	//meta data io
	void ReadVolumeData();
	void WriteVolumeData();
	void ReadMetaData();
	void WriteMetaData();
	
	
	//meta data accessors
	const SegmentDataSet& GetModifiedVoxelValues() const;
	void ClearModifiedVoxelValues();
	
	
	//mipchunk data accessors
	const SegmentDataSet& GetDirectDataValues() const;	
	const SegmentDataSet& GetIndirectDataValues() const;
	void RefreshDirectDataValues();
	void RefreshIndirectDataValues();
	

	//chunk extent
	const NormBbox& GetNormExtent() const;
	const NormBbox& GetClippedNormExtent() const;
	
	
	//mip properties
	int GetLevel() const;
	bool IsRoot() const;
	bool IsLeaf() const;
	const OmMipChunkCoord& GetCoordinate();
	const OmMipChunkCoord& GetParentCoordinate();
	const set<OmMipChunkCoord>& GetChildrenCoordinates();
	
	//slice
	AxisAlignedBoundingBox<int> ExtractSliceExtent(OmDataVolumePlane plane, int coord);
	void* ExtractDataSlice(OmDataVolumePlane plane, int offset, Vector2<int> &sliceDims, bool fast = false);
	
	//meshing
	vtkImageData* GetMeshImageData();
	
	
	//drawing
	bool DrawCheck( const OmVolumeCuller & );
	void DrawClippedExtent();
	
	
private:
	QMutex * mOpenLock;
	int mEstMemBytes;
	void InitChunk(const OmMipChunkCoord &rMipCoord);
	
	//mip volume this chunk belongs to
	OmMipVolume * const mpMipVolume;
	
	//image data of chunk
	vtkImageData *mpImageData;	
	
	
	//cache direct and indirectly contained values for drawing tree
	SegmentDataSet mDirectlyContainedDataValuesSet;	
	SegmentDataSet mIndirectlyContainedDataValuesSet;
	
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
	
	
	//friend class OmMipVolume;
	friend ostream& operator<<(ostream &out, const OmMipChunk &mc);
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};





#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmMipChunk, 0)

template<class Archive>
void 
OmMipChunk::serialize(Archive & ar, const unsigned int file_version) {

	//meta data
	ar & mDirectlyContainedDataValuesSet;
	ar & mIndirectlyContainedDataValuesSet;
	ar & mModifiedVoxelValues;
}




#endif
