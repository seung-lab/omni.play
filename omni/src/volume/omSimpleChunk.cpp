#include "common/omDebug.h"
#include "common/omGl.h"
#include "common/omUtility.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "datalayer/archive/omDataArchiveQT.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "utility/omImageDataIo.h"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omSimpleChunk.h"
#include "volume/omVolumeCuller.h"

static const float MIP_CHUNK_DATA_SIZE_SCALE_FACTOR = 1.4f;

/////////////////////////////////
///////
///////         OmSimpleChunk Class
///////

/*
 *	Constructor speicifies MipCoord and MipVolume the chunk extracts data from
 *	Note: optional cache pointer if this is a cached chunk
 */
OmSimpleChunk::OmSimpleChunk(const OmMipChunkCoord & rMipCoord, OmMipVolume * pMipVolume)
	: OmMipChunk( rMipCoord, pMipVolume )
{
}

OmSimpleChunk::~OmSimpleChunk()
{
	//since parent destructor is called after this child destructor, we need to call
	//child Close() here, or else child Close() won't be called (since child won't exist)
	//when called in parent destructor
	if (IsOpen()) {
		Close();
	}
	
	//remove object size from cache
	UpdateSize(-int (sizeof(OmSimpleChunk)));
}

/*
 *	Close volume data
 */
void OmSimpleChunk::Close()
{
	//ignore if already closed
	if (!IsOpen())
		return;

	//flush if dirty
	if (IsDirty())
		Flush();

	//close
	SetOpen(false);

	//remove image data size from cache (convert to bytes)
	int est_mem_bytes = GetSize();
	UpdateSize(-float (est_mem_bytes) * MIP_CHUNK_DATA_SIZE_SCALE_FACTOR);
}

void OmSimpleChunk::OpenForWrite()
{
	if (IsOpen()) {
		return;
	}

	OmDataPath mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );
	
	//assert(OmProjectData::DataExists(mip_level_vol_path));
	int size = GetSize();
	mpImageData = OmProjectData::GetProjectDataReader()->dataset_raw_read( mip_level_vol_path, &size);

	SetImageData(NULL);

	SetOpen(true);
}

void OmSimpleChunk::WriteVolumeData()
{
	if (!IsOpen()) {
		OpenForWrite();
	}
 
	OmDataPath mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel() ) );

	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( mip_level_vol_path, GetSize(), mpImageData->getPtr());

	setVolDataClean();
}

int  OmSimpleChunk::GetSize()
{
 	Vector3i dimensions = mDataExtent.getMax() - mDataExtent.getMin() + Vector3i::ONE;
        return (dimensions.x * dimensions.y * dimensions.z * GetBytesPerSample());
}

/////////////////////////////////
///////           Data IO

void OmSimpleChunk::ReadVolumeData()
{
	//get path to mip level volume
	OmDataPath mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );

	//read from project data
	if (!OmProjectData::GetProjectDataReader()->dataset_exists(mip_level_vol_path)) {
		assert(0);
		throw OmIoException("no MIP data found");
	}

	mpImageData = OmProjectData::GetProjectDataReader()->dataset_read_raw_chunk_data(mip_level_vol_path, GetExtent(), GetBytesPerSample());

	if( NULL == mpImageData->getPtr() ){
		// TODO: how does this affect the class that owns the OmSimpleChunk object?
		delete this;
	}

	//set this image data
	SetImageData(NULL);

	// Need to undo the side effect caused by setting the image data. Don't want to write out the data just
	// because we set the newly loaded data.
	setVolDataClean();
}


/////////////////////////////////
///////          Data Accessors

/*
 *	Set value of voxel in ImageData of MipChunk.  Causes volume data to become dirty.
 */
void OmSimpleChunk::SetVoxelValue(const DataCoord & voxel, uint32_t val)
{
	//debug("FIXME", << "in OmSimpleChunk::SetVoxelValue" << endl;

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
	Vector3i dimensions = mDataExtent.getMax() - mDataExtent.getMin() + Vector3i::ONE;

	int index = offset.z*dimensions.y*dimensions.x + offset.y*dimensions.x + offset.x; 

	// create two difft types of pointers to this data;
	quint8* char_ptr = mpImageData->getQuint8Ptr();
	uint32_t* int_ptr = mpImageData->getUInt32Ptr();

	//cast to appropriate type and return as uint
	switch (GetBytesPerSample()) {
	case 1:
		char_ptr[index] = (quint8) val;
		break;
	case 4:
		int_ptr[index] = val;
		break;
	default:
		break;
	}

	//data volume now dirty
	setVolDataDirty();
}

/*
 *	Get voxel value from the ImageData associated with this MipChunk.
 */
uint32_t OmSimpleChunk::GetVoxelValue(const DataCoord & voxel)
{
	//assert valid
	assert(ContainsVoxel(voxel));

	//ensure open
	Open();

	//get offset into data
	DataCoord offset = voxel - GetExtent().getMin();
	Vector3i dimensions = mDataExtent.getMax() - mDataExtent.getMin() + Vector3i::ONE;
	int index = offset.z*dimensions.y*dimensions.x + offset.y*dimensions.x + offset.x; 

	// create two difft types of pointers to this data;
	quint8* char_ptr = mpImageData->getQuint8Ptr();
	uint32_t* int_ptr = mpImageData->getUInt32Ptr();

	//cast to appropriate type and return as uint
	switch (GetBytesPerSample()) {
	case 1:
		return char_ptr[index];

	case 4:
		return int_ptr[index];
	}

	assert(0);
}

/*
 *	Set the image data of this MipChunk to data at the given pointer.
 *	Closes if already open and sets MipChunk to be open and dirty.
 */
void OmSimpleChunk::SetImageData(void *)
{
	//set data causes chunk to be open and dirty
	SetOpen(true);
	setVolDataDirty();

	//remove image data size from cache (convert to bytes)
	int est_mem_bytes = GetSize();
	UpdateSize(float (est_mem_bytes) * MIP_CHUNK_DATA_SIZE_SCALE_FACTOR);
}

/*
 *	Analyze segmentation ImageData in the chunk associated to a MipCoord and store 
 *	all values in the DataSegmentId set of the chunk.
 */
boost::unordered_map< OmSegID, unsigned int> * OmSimpleChunk::RefreshDirectDataValues( OmSegmentCache * )
{
	assert(0); // GUI crashes when using this method instead of the one in OmMipChunk (purcaro)

	boost::unordered_map< OmSegID, unsigned int> * sizes = new boost::unordered_map< OmSegID, unsigned int>();

	//uses mpImageData so ensure chunk is open
	Open();

	//clear previous segments
	loadMetadataIfPresent();
	mDirectlyContainedValues.clear();

	//get number of values in chunk array
	int NumberOfValues = GetSize() / GetBytesPerSample();
	int index;

	//get pointer to native scalar data
	if (SEGMENT_DATA_BYTES_PER_SAMPLE == GetBytesPerSample()) {
		OmSegID *p_scalar_data = static_cast < OmSegID * >(mpImageData->getUInt32Ptr());

		//for all voxels in the chunk
		for ( index = 0; index < NumberOfValues; index++) {
			//if non-null insert in set
			if (NULL_SEGMENT_DATA != *p_scalar_data) {
				mDirectlyContainedValues.insert(*p_scalar_data);
				++(sizes->operator[](*p_scalar_data));
			}
			//adv to next scalar
			++p_scalar_data;
		}
	} else if (1 == GetBytesPerSample()) {
		unsigned char *p_scalar_data = static_cast < unsigned char *>(mpImageData->getQuint8Ptr());
		OmSegID my_scalar_data;

		//for all voxels in the chunk
		for ( index = 0; index < NumberOfValues; index++) {
			//if non-null insert in set
			if ('\0' != *p_scalar_data) {
				my_scalar_data = (OmSegID) (*p_scalar_data);
				mDirectlyContainedValues.insert(my_scalar_data);
				++(sizes->operator[](my_scalar_data));
			}
			//adv to next scalar
			++p_scalar_data;
		}
	}

	//note metadata is dirty
	setMetaDataDirty();

	return sizes;
}

/////////////////////////////////
///////          Slice Data Access
/*
 *	Returns a pointer to an allocated array of data representing the values
 *	on an orthogonal slice given the slice plane and depth.
 *
 *  plane is the VOL_XY_PLANE, VOL_XZ_PLANE, VOL_YZ_PLANE that intersects the volume
 *  offset is the relative depth that the given plane intersects the data extent
 *  memSize returns the size in bytes of the allocated array
 *	sliceDims is the dimensions of the resulting slice
 *
 *	NOTE: This returns ownership of the array so it MUST BE DELETED after use.
 */
void * OmSimpleChunk::ExtractDataSlice(OmDataVolumePlane plane, int offset, Vector2 < int >&sliceDims, bool fast)
{
	
	if (!IsOpen() && fast)	// If we want to extract fast leave fast if not open.
		return NULL;

	//ensure open
	Open();

	//get global slice coordinate
 	Vector3i dimensions = mDataExtent.getMax() - mDataExtent.getMin() + Vector3i::ONE;
	int bytesPerSample = GetBytesPerSample();

	Vector2i imageIncrement;
	int imageIndex;

	//form result dims
	switch (plane) {
	case VOL_XY_PLANE:
		sliceDims.x = dimensions.x;
		sliceDims.y = dimensions.y;
		imageIncrement.x = 1;
		imageIncrement.y = 0;
		imageIndex = offset*dimensions.x*dimensions.y;
		break;
	case VOL_XZ_PLANE:
		sliceDims.x = dimensions.x;
		sliceDims.y = dimensions.z;
		imageIncrement.x = 1;
		imageIncrement.y = dimensions.x*dimensions.y-dimensions.x;
		imageIndex = offset*dimensions.x;
		break;
	case VOL_YZ_PLANE:
		sliceDims.x = dimensions.y;
		sliceDims.y = dimensions.z;
		imageIncrement.x = dimensions.x;
		imageIncrement.y = 0; 
		imageIndex = offset;
		break;
	default:
		assert(false);
	}

	// create two difft types of pointers to this data;
	quint8* char_ptr = mpImageData->getQuint8Ptr();
	uint32_t* int_ptr =  mpImageData->getUInt32Ptr();

	// create the two possible resulting arrays
	quint8* char_result = (quint8*) malloc(sliceDims.x*sliceDims.y);
	uint32_t* int_result = (uint32_t*) malloc(sliceDims.x*sliceDims.y*4);

	// 
	int index_x, index_y, sliceIndex;
	sliceIndex = 0;

	for (index_y=0; index_y<sliceDims.y; index_y++){
		for (index_x=0; index_x<sliceDims.x; index_x++){
			if (bytesPerSample == 1){
				char_result[sliceIndex] = char_ptr[imageIndex];
			}
			if (bytesPerSample == 4){
				int_result[sliceIndex] = int_ptr[imageIndex];
			}
			imageIndex += imageIncrement.x;
			sliceIndex++;
		}
		imageIndex += imageIncrement.y;
	}
	
	if  (bytesPerSample == 1) return (void*) char_result;
	if  (bytesPerSample == 4) return (void*) int_result;
	
	assert(0);
}

void * OmSimpleChunk::GetMeshImageData()
{
	return mpImageData->getPtr();
}
