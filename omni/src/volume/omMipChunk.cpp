#include "common/omDebug.h"
#include "common/omGl.h"
#include "common/omVtk.h"
#include "datalayer/archive/omDataArchiveQT.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "project/omProject.h"
#include "segment/omSegment.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/image/omImage.hpp"
#include "utility/omImageDataIo.h"
#include "volume/omChunkData.hpp"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolumeCuller.h"
#include "volume/omVolumeData.hpp"

#include <vtkImageData.h>
#include <vtkType.h>


static const float MIP_CHUNK_DATA_SIZE_SCALE_FACTOR = 1.4f;

/**
 *	Constructor speicifies MipCoord and MipVolume the chunk extracts data from
 *	Note: optional cache pointer if this is a cached chunk
 */
OmMipChunk::OmMipChunk(const OmMipChunkCoord & rMipCoord, OmMipVolume * pMipVolume)
	: OmCacheableBase(pMipVolume->mDataCache)
	, mIsRawChunkOpen(false)
	, mIsRawMappedChunkOpen(false)
	, mpMipVolume(pMipVolume)
	, mChunkData(new OmChunkData(mpMipVolume, rMipCoord))
{
	//init chunk properties
	InitChunk(rMipCoord);

	containedValuesDataLoaded = false;

	mChunkVolumeDataDirty = false;
	mChunkMetaDataDirty = false;

	mIsOpen = false;
}

OmMipChunk::~OmMipChunk()
{
	//debug("genone","OmMipChunk::~OmMipChunk()");

	//since parent destructor is called after this child destructor, we need to call
	//child Close() here, or else child Close() won't be called (since child won't exist)
	//when called in parent destructor
	if (IsOpen()) {
		Close();
	}
}

/*
 *	Initialize chunk with properties of given coordinate.
 */
void
 OmMipChunk::InitChunk(const OmMipChunkCoord & rMipCoord)
{
	//set coordinate
	mCoordinate = rMipCoord;

	//set parent, if any
	if (rMipCoord.Level == mpMipVolume->GetRootMipLevel()) {
		mParentCoord = OmMipChunkCoord::NULL_COORD;
	} else {
		mParentCoord = rMipCoord.ParentCoord();
	}

	//set children
	mpMipVolume->ValidMipChunkCoordChildren(rMipCoord, mChildrenCoordinates);

	//get extent from coord
	mDataExtent = mpMipVolume->MipCoordToDataBbox(rMipCoord, rMipCoord.Level);

	//set norm extent
	mNormExtent = mpMipVolume->MipCoordToNormBbox(rMipCoord);

	//set clipped norm extent
	mClippedNormExtent = mpMipVolume->MipCoordToNormBbox(rMipCoord);
	mClippedNormExtent.intersect(AxisAlignedBoundingBox < float >::UNITBOX);

	//set if mipvolume uses metadata
	setMetaDataClean();
}

/////////////////////////////////
///////          DataVolume Methods

/*
 *	Overridden to load data from project data mip volume.
 */
void OmMipChunk::Open()
{
        QMutexLocker locker(&mOpenLock);

	if (IsOpen()) {
		return;
	}

	ReadVolumeData();

	SetOpen(true);
}

void OmMipChunk::OpenForWrite()
{
	if (IsOpen()) {
		return;
	}

	OmDataPath mip_level_vol_path(mpMipVolume->MipLevelInternalDataPath(GetLevel()));

	//assert(OmProjectData::DataExists(mip_level_vol_path));
	OmDataWrapperPtr data = OmProjectData::GetProjectDataReader()->dataset_image_read_trim( mip_level_vol_path, GetExtent());

	SetImageData(data);

	SetOpen(true);
}

/*
 *	Flush data to project data mip volume.
 */
void OmMipChunk::Flush()
{
	if(!OmProject::GetCanFlush()) {
		return;
	}

	//only write if dirty
	if (IsVolumeDataDirty()) {
		WriteVolumeData();
	}

	//write meta data if dirty
	if (IsMetaDataDirty()) {
		WriteMetaData();
	}

	dealWithCrazyNewStuff();
}

/*
 *	Close volume data
 */
void OmMipChunk::Close()
{
	dealWithCrazyNewStuff();

	if (!IsOpen()) {
		return;
	}

	if (IsDirty()) {
		Flush();
	}

	SetOpen(false);
}

/////////////////////////////////
///////          Data Properties

const DataBbox & OmMipChunk::GetExtent()
{
	return mDataExtent;
}

/////////////////////////////////
///////          Dirty Methods

bool OmMipChunk::IsDirty()
{
	return IsVolumeDataDirty() || IsMetaDataDirty();
}

bool OmMipChunk::IsVolumeDataDirty()
{
	return mChunkVolumeDataDirty;
}

bool OmMipChunk::IsMetaDataDirty()
{
	//TODO: why isn't this OR (ie. ||)?
	return mpMipVolume->GetChunksStoreMetaData()
		&& mChunkMetaDataDirty;
}

/////////////////////////////////
///////           Data IO

void OmMipChunk::ReadVolumeData()
{
	OmDataPath mip_level_vol_path(mpMipVolume->MipLevelInternalDataPath(GetLevel()));

	OmDataWrapperPtr data = OmProjectData::GetProjectDataReader()->dataset_image_read_trim(mip_level_vol_path, GetExtent());

	//set this image data
	SetImageData(data);

	// Need to undo the side effect caused by setting the image data. Don't want to write out the data just
	// because we set the newly loaded data.
	setVolDataClean();
}

void OmMipChunk::WriteVolumeData()
{
	if (!IsOpen()) {
		OpenForWrite();
	}

	OmDataPath mip_level_vol_path(mpMipVolume->MipLevelInternalDataPath(GetLevel()));

	if (mData->getVTKPtr()) {
		OmProjectData::GetDataWriter()->
			dataset_image_write_trim(mip_level_vol_path,
						 (DataBbox*)&GetExtent(),
						 mData);
	}

	setVolDataClean();
}

void OmMipChunk::ReadMetaData()
{
	OmDataPath dat_file_path(mpMipVolume->MipChunkMetaDataPath(mCoordinate));

	//read archive if it exists
	if (OmProjectData::GetProjectDataReader()->dataset_exists(dat_file_path)) {
		OmDataArchiveQT::ArchiveRead(dat_file_path, this);
	}
}

void OmMipChunk::WriteMetaData()
{
	OmDataPath dat_file_path(mpMipVolume->MipChunkMetaDataPath(mCoordinate));

	OmDataArchiveQT::ArchiveWrite(dat_file_path, this);

	setMetaDataClean();
}

/////////////////////////////////
///////          MetaData Accessors

const OmSegIDsSet & OmMipChunk::GetModifiedVoxelValues()
{
	return mModifiedVoxelValues;
}

void OmMipChunk::ClearModifiedVoxelValues()
{
	mModifiedVoxelValues.clear();
}

/////////////////////////////////
///////          Data Accessors

/*
 *	Set value of voxel in ImageData of MipChunk.  Causes volume data to become dirty.
 */
void OmMipChunk::SetVoxelValue(const DataCoord & voxel, uint32_t val)
{
	assert(ContainsVoxel(voxel));

	assert(0);

	/*
        if(mData->getHdf5MemoryType() == H5T_NATIVE_FLOAT) {
	  printf("fixme!\n");
        } else if(mData->getHdf5MemoryType() == H5T_NATIVE_UINT ||
                  mData->getHdf5MemoryType() == H5T_NATIVE_INT  ){
          OmImage<uint32_t, 3, OmImageRefData> chunk(OmExtents[128][128][128],
                                     RawReadChunkDataUINT32mapped()->getPtr<uint32_t>());

	  mModifiedVoxelValues.insert(chunk.getVoxel(voxel.x, voxel.y, voxel.z));
	  mModifiedVoxelValues.insert(val);
          chunk.setVoxel(voxel.x, voxel.y, voxel.z, val);
        } else if(mData->getHdf5MemoryType() == H5T_NATIVE_UCHAR ||
                  mData->getHdf5MemoryType() == H5T_NATIVE_CHAR  ){
	  printf("fixme!\n");
        }
	*/
	//data volume now dirty
	setVolDataDirty();
}

/*
 *	Get voxel value from the ImageData associated with this MipChunk.
 */
uint32_t OmMipChunk::GetVoxelValue(const DataCoord & volVoxel)
{

	assert(0);
	/*
        const DataCoord voxel = volVoxel - GetExtent().getMin();

	assert(ContainsVoxel(voxel));

        if(mData->getHdf5MemoryType() == H5T_NATIVE_FLOAT) {
	  printf("fixme!\n");
        } else if(mData->getHdf5MemoryType() == H5T_NATIVE_UINT ||
                  mData->getHdf5MemoryType() == H5T_NATIVE_INT  ){
          OmImage<uint32_t, 3, OmImageRefData> chunk(OmExtents[128][128][128],
                                     RawReadChunkDataUINT32mapped()->getPtr<uint32_t>());

          return chunk.getVoxel(voxel.x, voxel.y, voxel.z);
        } else if(mData->getHdf5MemoryType() == H5T_NATIVE_UCHAR ||
                  mData->getHdf5MemoryType() == H5T_NATIVE_CHAR  ){
	  printf("fixme!\n");
        }
	*/
	assert(0 && "fixme!");
}

/*
 *	Returns pointer to image data
 */

vtkImageData* OmMipChunk::GetImageData()
{
	return mData->getVTKPtr();
}

OmDataWrapperPtr OmMipChunk::GetImageDataWrapper()
{
	return mData;
}

/*
 *	Set the image data of this MipChunk to data at the given pointer.
 *	Closes if already open and sets MipChunk to be open and dirty.
 */
void OmMipChunk::SetImageData(OmDataWrapperPtr data)
{
	//set this image data to given
	mData = data;

	//set data causes chunk to be open and dirty
	SetOpen(true);
	setVolDataDirty();
}

/////////////////////////////////
///////          Mip Accessors

int OmMipChunk::GetLevel()
{
	return mCoordinate.Level;
}

bool OmMipChunk::IsRoot()
{
	//if parent is null
	return mParentCoord == OmMipChunkCoord::NULL_COORD;
}

bool OmMipChunk::IsLeaf()
{
	return mCoordinate.Level == 0;
}

const OmMipChunkCoord & OmMipChunk::GetCoordinate()
{
	return mCoordinate;
}

const OmMipChunkCoord & OmMipChunk::GetParentCoordinate()
{
	return mParentCoord;
}

const set < OmMipChunkCoord > & OmMipChunk::GetChildrenCoordinates()
{
	return mChildrenCoordinates;
}

/////////////////////////////////
///////          Property Accessors

const NormBbox & OmMipChunk::GetNormExtent()
{
	return mNormExtent;
}

const NormBbox & OmMipChunk::GetClippedNormExtent()
{
	return mClippedNormExtent;
}

/////////////////////////////////
///////          MetaData Accessors

/*
 *	Returns reference to set of all values directly contained by
 *	the image data of this MipChunk
 */
const OmSegIDsSet & OmMipChunk::GetDirectDataValues()
{
	//TODO: change to shared reader writer lock (purcaro)
	QMutexLocker lock(&mDirectDataValueLock);
	loadMetadataIfPresent();
	return mDirectlyContainedValues;
}

void OmMipChunk::loadMetadataIfPresent()
{
	if( containedValuesDataLoaded ){
		return;
	}

	if (mpMipVolume->GetChunksStoreMetaData()) {
		ReadMetaData();
	}

	containedValuesDataLoaded = true;
}

/*
 *	Analyze segmentation ImageData in the chunk associated to a MipCoord and store
 *	all values in the DataSegmentId set of the chunk.
 */
OmSegSizeMapPtr OmMipChunk::RefreshDirectDataValues(const bool computeSizes)
{
	mDirectlyContainedValues.clear();
	containedValuesDataLoaded = true;
	setMetaDataDirty();

	return mChunkData->RefreshDirectDataValues(this, computeSizes);
}

void OmMipChunk::addVoxelToBounds(const OmSegID val, const Vector3i & voxelPos)
{
	const Vector3i minVertexOfChunk = GetExtent().getMin();
	const DataBbox box(minVertexOfChunk + voxelPos,
			   minVertexOfChunk + voxelPos);
	if (mBounds[val].isEmpty()) {
		mBounds[val] = box;
	} else {
		mBounds[val].merge(box);
	}
}

Vector2i OmMipChunk::GetSliceDims()
{
  return Vector2i(128,128);
}

/**
 *      Returns new ImageData containing the entire extent of data needed
 *      to form continuous meshes with adjacent MipChunks.  This means an extra
 *      voxel of data is included on each dimensions.
 */
OmImage<uint32_t, 3> OmMipChunk::GetMeshOmImageData()
{
  OmImage<uint32_t, 3> retImage(OmExtents[129][129][129]);

  for (int z = 0; z < 2; z++) {
    for (int y = 0; y < 2; y++) {
      for (int x = 0; x < 2; x++) {

        int lenZ = z ? 1 : 128;
        int lenY = y ? 1 : 128;
        int lenX = x ? 1 : 128;

        //form mip coord
        OmMipChunkCoord mip_coord(mCoordinate.getLevel(),
                                  mCoordinate.getCoordinateX() + x,
                                  mCoordinate.getCoordinateY() + y,
                                  mCoordinate.getCoordinateZ() + z);

        //skip invalid mip coord
        if (!mpMipVolume->ContainsMipChunkCoord(mip_coord))
          continue;

        //else get chunk
        OmMipChunkPtr p_chunk;
        mpMipVolume->GetChunk(p_chunk, mip_coord);

	OmImage<uint32_t, 3> chunkImage;

	assert(0);
	/*
	if(mData->getHdf5MemoryType() == H5T_NATIVE_UINT ||
	   mData->getHdf5MemoryType() == H5T_NATIVE_INT  ){
	  chunkImage = OmImage<uint32_t, 3>(OmExtents[128][128][128],
					    p_chunk->RawReadChunkDataUINT32mapped()
					    ->getPtr<uint32_t>());
	} else if (mData->getHdf5MemoryType() == H5T_NATIVE_UCHAR) {
	  OmImage<unsigned char, 3> chunk8(OmExtents[128][128][128],
					   RawReadChunkDataUCHARmapped()->getPtr<unsigned char>());
	  chunkImage = chunk8.recast<uint32_t>();
	} else {
	  printf("type was %s...\n", p_chunk->mData->getTypeAsString().c_str());
	  assert(0 && "unrecognized type");
	}
	*/
        retImage.copyFrom(chunkImage,
			  OmExtents[z*128][y*128][x*128],
                          OmExtents[0][0][0],
			  OmExtents[lenZ][lenY][lenX]);

      }
    }
  }

  return retImage;
}

bool OmMipChunk::IsOpen()
{
	return mIsOpen;
}

void OmMipChunk::SetOpen(bool state)
{
	mIsOpen = state;
}

bool OmMipChunk::ContainsVoxel(const DataCoord & vox)
{
	return GetExtent().contains(vox);
}

const Vector3 < int > OmMipChunk::GetDimensions()
{
	return GetExtent().getUnitDimensions();
}

void OmMipChunk::setVolDataDirty()
{
	mChunkVolumeDataDirty = true;
}

void OmMipChunk::setMetaDataDirty()
{
	mChunkMetaDataDirty = true;
}

void OmMipChunk::setVolDataClean()
{
	mChunkVolumeDataDirty = false;
}

void OmMipChunk::setMetaDataClean()
{
	mChunkMetaDataDirty = false;
}

OmDataWrapperPtr OmMipChunk::RawReadChunkDataHDF5()
{
        QMutexLocker locker(&mOpenLock);
	if(!mIsRawChunkOpen){
		OmDataPath path(mpMipVolume->MipLevelInternalDataPath(GetLevel()));

		mRawChunk = OmProjectData::GetProjectDataReader()->
			dataset_read_raw_chunk_data(path, GetExtent());
		mIsRawChunkOpen=true;
	}

	return mRawChunk;
}

void OmMipChunk::dealWithCrazyNewStuff()
{
        QMutexLocker locker(&mOpenLock);

	if(mIsRawChunkOpen){
		mIsRawChunkOpen=false;
		mRawChunk=OmDataWrapperInvalid();
	}
	if(mIsRawMappedChunkOpen){
		mIsRawMappedChunkOpen=false;
		mRawMappedChunk=OmDataWrapperInvalid();
	}
}

void OmMipChunk::GetBounds(float & maxout, float & minout)
{
	return; //FIXME!

	Open();
	float * data = static_cast < float * >(mData->getVTKPtr()->GetScalarPointer());
	OmImage<float, 3> chunk(OmExtents[128][128][128], data);
	maxout = chunk.getMax();
	minout = chunk.getMin();
}

void* OmMipChunk::ExtractDataSlice(const ViewType plane, int offset)
{
	return mChunkData->ExtractDataSlice(plane, offset);
}

void OmMipChunk::copyInTile(const int sliceOffset, uchar* bits)
{
	mChunkData->copyInTile(sliceOffset, bits);
}

void OmMipChunk::copyChunkFromMemMapToHDF5()
{
	mChunkData->copyChunkFromMemMapToHDF5(this);
}

void OmMipChunk::writeHDF5()
{
        QMutexLocker locker(&mOpenLock);

	//get path to mip level volume
	OmDataPath path(mpMipVolume->MipLevelInternalDataPath(GetLevel()));

	OmProjectData::GetDataWriter()->
		dataset_write_raw_chunk_data( path, GetExtent(), mRawChunk);
}
