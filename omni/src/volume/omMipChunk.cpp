#include "common/omGl.h"
#include "common/omVtk.h"
#include "utility/image/omImage.hpp"
#include "datalayer/archive/omDataArchiveQT.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "datalayer/omDataWrapper.h"
#include "project/omProject.h"
#include "segment/omSegment.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/omImageDataIo.h"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolumeCuller.h"

#include <vtkImageData.h>
#include <vtkType.h>

#include "common/omDebug.h"

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

	//ignore if already open
	if (IsOpen())
		return;


	//read volume data
	ReadVolumeData();

	const int64_t size = 128*128*128*GetBytesPerSample();
	UpdateSize(size);
	std::cout << "increasing cache size by " << size << "\n";

	//set open
	SetOpen(true);
}

void OmMipChunk::OpenForWrite()
{
	if (IsOpen()) {
		return;
	}

	OmDataPath mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );

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

	//ignore if already closed
	if (!IsOpen())
		return;

	//flush if dirty
	if (IsDirty())
		Flush();

	//close
	SetOpen(false);

	const int64_t size = -128*128*128*GetBytesPerSample();
	UpdateSize(size);
	std::cout << "decreasing cache size by " << size << "\n";
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
	//get path to mip level volume
	OmDataPath mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );

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

	OmDataPath mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel() ) );

	if (mData->getVTKPtr()) {
		OmProjectData::GetDataWriter()->dataset_image_write_trim(mip_level_vol_path, (DataBbox*)&GetExtent(), mData);
	}

	setVolDataClean();
}

void OmMipChunk::ReadMetaData()
{
	OmDataPath dat_file_path;
	dat_file_path.setPathQstr( mpMipVolume->MipChunkMetaDataPath(mCoordinate) );

	//read archive if it exists
	if (OmProjectData::GetProjectDataReader()->dataset_exists(dat_file_path)) {
		OmDataArchiveQT::ArchiveRead(dat_file_path, this);
	}
}

void OmMipChunk::WriteMetaData()
{
	OmDataPath dat_file_path;
	dat_file_path.setPathQstr(mpMipVolume->MipChunkMetaDataPath(mCoordinate));

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
	//debug("FIXME", << "in OmMipChunk::SetVoxelValue" << endl;

	//assert valid
	assert(ContainsVoxel(voxel));

	//ensure open
	Open();

	//record old and new values as modified
	uint32_t old_val = GetVoxelValue(voxel);
	mModifiedVoxelValues.insert(old_val);
	mModifiedVoxelValues.insert(val);

	//get offset into data
	DataCoord offset = voxel - GetExtent().getMin();

	//get pointer to data and copy bytes
	void *p_scalar = mData->getVTKPtr()->GetScalarPointer(offset.x, offset.y, offset.z);
	void *p = mData->getVTKPtr()->GetScalarPointer();
	debug("FIXME", "%p\n", p);

	//cast to appropriate type and return as uint
	memcpy(p_scalar, &val, mData->getSizeof());

	//data volume now dirty
	setVolDataDirty();
}

/*
 *	Get voxel value from the ImageData associated with this MipChunk.
 */
uint32_t OmMipChunk::GetVoxelValue(const DataCoord & voxel)
{
	//assert valid
	assert(ContainsVoxel(voxel));

	//ensure open
	Open();

	//get offset into data
	DataCoord offset = voxel - GetExtent().getMin();

	//get pointer to data and copy bytes
	void *p_scalar = mData->getVTKPtr()->GetScalarPointer(offset.x, offset.y, offset.z);

	//cast to appropriate type and return as uint
	switch (GetBytesPerSample()) {
	case 1:
		return *((quint8 *) p_scalar);
	case 4:
		return *((uint32_t *) p_scalar);
	default:
		assert(false);
	}
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
boost::unordered_map< OmSegID, unsigned int> * OmMipChunk::RefreshDirectDataValues( const bool computeSizes )
{
	//uses mData->getVTKPtr() so ensure chunk is open
	Open();

	//clear previous segments
	loadMetadataIfPresent();
	mDirectlyContainedValues.clear();

	if(SEGMENT_DATA_BYTES_PER_SAMPLE == mData->getVTKPtr()->GetScalarSize()){
		OmSegID *p_scalar_data = static_cast<OmSegID*>(mData->getVTKPtr()->GetScalarPointer());
		return doRefreshDirectDataValues(computeSizes, p_scalar_data);
	} else if (1 == mData->getVTKPtr()->GetScalarSize()) {
		uchar* p_scalar_data = static_cast<uchar*>(mData->getVTKPtr()->GetScalarPointer());
		return doRefreshDirectDataValues(computeSizes, p_scalar_data);
	} else {
		assert(0 && "unsupported number of bytes per sample");
	}
}

template <typename C>
boost::unordered_map< OmSegID, unsigned int> * OmMipChunk::doRefreshDirectDataValues( const bool computeSizes,
										      C* p_scalar_data)
{
	boost::unordered_map< OmSegID, unsigned int> * sizes = NULL;
	if( computeSizes ){
		sizes = new boost::unordered_map< OmSegID, unsigned int>();
	}

	int extent[6];
	mData->getVTKPtr()->GetExtent(extent);

	//for all voxels in the chunk
	for(int z = extent[0]; z <= extent[1]; z++) {
		for(int y = extent[2]; y <= extent[3]; y++) {
			for(int x = extent[4]; x <= extent[5]; x++) {

				const C val = *p_scalar_data;

				if( 0 == val) {
					continue;
				}

				mDirectlyContainedValues.insert(val);

				if(!computeSizes){
					continue;
				}

				++((*sizes)[val]);

				const Vector3i voxelPos(x,y,z);
				const Vector3i minVertexOfChunk = GetExtent().getMin();
				const DataBbox box(minVertexOfChunk + voxelPos,
						   minVertexOfChunk + voxelPos);
				if (mBounds[val].isEmpty()) {
					mBounds[val] = box;
				} else {
					mBounds[val].merge(box);
				}

				//adv to next scalar
				++p_scalar_data;
			}
		}
	}

	setMetaDataDirty();

	return sizes;
}

Vector2i OmMipChunk::GetSliceDims()
{
  return Vector2i(128,128);
}

void * OmMipChunk::ExtractDataSlice(const ViewType plane, int offset)
{
	//	Open();

	//	printf("type is %s...\n", mData->getTypeAsString().c_str());

	if(mData->getHdf5MemoryType() == H5T_NATIVE_FLOAT) {
	  OmDataWrapperPtr dataPtrMapped = RawReadChunkDataUINT32mapped();
	  float* dataMapped = dataPtrMapped->getPtr<float>();

	  assert(dataMapped);

	  OmImage<float, 3> chunk(OmExtents[128][128][128], dataMapped);
	  OmImage<float, 2> sliceFloat = chunk.getSlice(plane, offset);
	  float mn = 0.0;
	  float mx = 1.0;
	  //	  mpMipVolume->GetBounds(mx, mn);
	  OmImage<unsigned char, 2> slice =
		  sliceFloat.rescaleAndCast<unsigned char>(mn, mx, 255.0);
	  return slice.getMallocCopyOfData();

	} else if(mData->getHdf5MemoryType() == H5T_NATIVE_UINT ||
		  mData->getHdf5MemoryType() == H5T_NATIVE_INT  ){

	  OmDataWrapperPtr dataPtrMapped = RawReadChunkDataUINT32mapped();
	  quint32* dataMapped = dataPtrMapped->getPtr<uint32_t>();

	  assert(dataMapped);

	  OmImage<uint32_t, 3> chunk(OmExtents[128][128][128], dataMapped);

	  OmImage<uint32_t, 2> slice = chunk.getSlice(plane, offset);
	  return slice.getMallocCopyOfData();

	} else if(mData->getHdf5MemoryType() == H5T_NATIVE_UCHAR ||
		  mData->getHdf5MemoryType() == H5T_NATIVE_CHAR  ){

	  OmDataWrapperPtr dataPtrMapped = RawReadChunkDataUCHARmapped();
	  unsigned char* dataMapped = dataPtrMapped->getPtr<unsigned char>();

	  assert(dataMapped);

	  OmImage<unsigned char, 3> chunk(OmExtents[128][128][128], dataMapped);
	  OmImage<unsigned char, 2> slice = chunk.getSlice(plane, offset);
	  return slice.getMallocCopyOfData();
	}

	assert(0);
}

/**
 *      Returns new ImageData containing the entire extent of data needed
 *      to form continuous meshes with adjacent MipChunks.  This means an extra
 *      voxel of data is included on each dimensions.
 */
OmImage<uint32_t, 3> OmMipChunk::GetMeshOmImageData()
{
	//  Open();

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

	//        p_chunk->Open();

	OmImage<uint32_t, 3> chunkImage;

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

int OmMipChunk::GetBytesPerSample()
{
	return mpMipVolume->GetBytesPerSample();
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

OmDataWrapperPtr OmMipChunk::RawReadChunkDataUCHAR()
{
        QMutexLocker locker(&mOpenLock);
	if(!mIsRawChunkOpen){
		OmDataPath path;
		path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );

		mRawChunk =
			OmProjectData::GetProjectDataReader()->
			dataset_read_raw_chunk_data(path, GetExtent());
		mIsRawChunkOpen=true;
	}

	return mRawChunk;
}

OmDataWrapperPtr OmMipChunk::RawReadChunkDataUINT32()
{
        QMutexLocker locker(&mOpenLock);
	if(!mIsRawChunkOpen){

		OmDataPath path;
		path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );

		mRawChunk = OmProjectData::GetProjectDataReader()->
			dataset_read_raw_chunk_data(path, GetExtent());
		mIsRawChunkOpen=true;
	}

	return mRawChunk;
}

void OmMipChunk::RawWriteChunkData(unsigned char * data)
{
        QMutexLocker locker(&mOpenLock);

	//get path to mip level volume
	OmDataPath path;
	path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel() ) );

	OmProjectData::GetDataWriter()->dataset_write_raw_chunk_data( path,
								      GetExtent(),
								      OmDataWrapperRaw(data));
}

void OmMipChunk::RawWriteChunkData(quint32* data)
{
        QMutexLocker locker(&mOpenLock);

	//get path to mip level volume
	OmDataPath path;
	path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel() ) );

	OmProjectData::GetDataWriter()->dataset_write_raw_chunk_data( path,
								      GetExtent(),
								      OmDataWrapperUint(data));
}

OmDataWrapperPtr OmMipChunk::RawReadChunkDataUCHARmapped()
{
        QMutexLocker locker(&mOpenLock);

	if(!mIsRawMappedChunkOpen){
		unsigned char * data =
			mpMipVolume->ucharData->getChunkPtr(mCoordinate);

		mRawMappedChunk = OmDataWrapper<unsigned char>::producemmap(data, OmMemoryMappedFile::FIXME(mpMipVolume));
		mIsRawMappedChunkOpen=true;
	}

	return mRawMappedChunk;
}

OmDataWrapperPtr OmMipChunk::RawReadChunkDataUINT32mapped()
{
        QMutexLocker locker(&mOpenLock);

	if(!mIsRawMappedChunkOpen){
		quint32* data =
			(quint32*)mpMipVolume->uint32Data->getChunkPtr(mCoordinate);
		mRawMappedChunk = OmDataWrapper<unsigned int>::producemmap(data, OmMemoryMappedFile::FIXME(mpMipVolume));
		mIsRawMappedChunkOpen=true;
	}

	return mRawMappedChunk;
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

