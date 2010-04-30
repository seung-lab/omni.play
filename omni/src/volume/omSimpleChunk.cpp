
#include "omSimpleChunk.h"
#include "omMipVolume.h"
#include "omVolumeCuller.h"
#include "omSegmentation.h"

#include "segment/omSegment.h"
#include "system/omStateManager.h"
#include "system/omProjectData.h"
#include "common/omUtility.h"
#include "common/omGl.h"
#include "common/omVtk.h"
#include "utility/omImageDataIo.h"
#include "utility/omDataArchiveQT.h"

#include <vtkImageData.h>
#include <vtkType.h>

#include "common/omDebug.h"

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
:OmCacheableBase(dynamic_cast < OmCacheBase * >(pMipVolume)), mpMipVolume(pMipVolume)
{

	//debug("genone","OmSimpleChunk::OmSimpleChunk()");   

	//init chunk properties
	InitChunk(rMipCoord);

	containedValuesDataLoaded = false;

	//update cache size
	UpdateSize(sizeof(OmSimpleChunk));

	mpImageData = NULL;
	mLock = new QMutex();

	mIsOpen = false;
}

OmSimpleChunk::~OmSimpleChunk()
{
	//debug("genone","OmSimpleChunk::~OmSimpleChunk()");

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
 *	Initialize chunk with properties of given coordinate.
 */
void
 OmSimpleChunk::InitChunk(const OmMipChunkCoord & rMipCoord)
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
	mMetaDataDirty = false;
}

/////////////////////////////////
///////          DataVolume Methods

/*
 *	Overridden to load data from project data mip volume.
 */
void OmSimpleChunk::Open()
{
        //QMutexLocker locker(mLock);

	//ignore if already open
	if (IsOpen())
		return;


	//read volume data
	ReadVolumeData();

	//debug("simplechunk", "read in mpImageData: %i, rc:%i\n", mpImageData, mpImageData->GetReferenceCount());

	//set open
	SetOpen(true);
}

void OmSimpleChunk::OpenForWrite()
{
	if (IsOpen()) {
		return;
	}

	OmHdf5Path mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );
	
	//assert(OmProjectData::DataExists(mip_level_vol_path));
	int size = GetSize();
	void *data = OmProjectData::GetProjectDataReader()->dataset_raw_read( mip_level_vol_path, &size);

	SetImageData(data);

	SetOpen(true);
}

/*
 *	Flush data to project data mip volume.
 */
void OmSimpleChunk::Flush()
{
	//only write if dirty
	if (IsVolumeDataDirty()) {
		printf("flushing chunk\n");
		WriteVolumeData();
	}

	//write meta data if dirty
	if (IsMetaDataDirty()) {
		WriteMetaData();
	}
}

/*
 *	Close volume data
 */
void OmSimpleChunk::Close()
{
	//debug("genone","OmSimpleChunk::Close()\n");

	//ignore if already closed
	if (!IsOpen())
		return;

	//flush if dirty
	if (IsDirty())
		Flush();

	//close
	SetOpen(false);

	//assert image data exists
	assert(mpImageData);

	//remove image data size from cache (convert to bytes)
	int est_mem_bytes = GetSize();
	UpdateSize(-float (est_mem_bytes) * MIP_CHUNK_DATA_SIZE_SCALE_FACTOR);

	//delete image data
	free(mpImageData);
	mpImageData = NULL;
}

/////////////////////////////////
///////          Data Properties

const DataBbox & OmSimpleChunk::GetExtent()
{
	return mDataExtent;
}

int  OmSimpleChunk::GetSize()
{
 	Vector3i dimensions = mDataExtent.getMax() - mDataExtent.getMin() + Vector3i::ONE;
        return (dimensions.x * dimensions.y * dimensions.z * GetBytesPerSample());
}

/////////////////////////////////
///////          Dirty Methods

bool OmSimpleChunk::IsDirty()
{
	return IsVolumeDataDirty() || IsMetaDataDirty();
}

bool OmSimpleChunk::IsVolumeDataDirty()
{
	return mVolumeDataDirty;
}

bool OmSimpleChunk::IsMetaDataDirty()
{
	return mpMipVolume->GetChunksStoreMetaData() && mMetaDataDirty;
}

/////////////////////////////////
///////           Data IO

void OmSimpleChunk::ReadVolumeData()
{
	//get path to mip level volume
	OmHdf5Path mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel()) );

	//read from project data
	if (!OmProjectData::GetProjectDataReader()->dataset_exists(mip_level_vol_path)) {
		assert(0);
		throw OmIoException("no MIP data found");
	}

	void *data = OmProjectData::GetProjectDataReader()->dataset_read_raw_chunk_data(mip_level_vol_path, GetExtent(), GetBytesPerSample());
	if (data ==NULL) delete this;
	//debug("mipchunk", "data: %i, refcount of:%i\n", data, data->GetReferenceCount ());

	//set this image data
	SetImageData(data);

	// Need to undo the side effect caused by setting the image data. Don't want to write out the data just
	// because we set the newly loaded data.
	mVolumeDataDirty = false;
}

void OmSimpleChunk::WriteVolumeData()
{
	if (!IsOpen()) {
		OpenForWrite();
	}

	OmHdf5Path mip_level_vol_path;
	mip_level_vol_path.setPathQstr( mpMipVolume->MipLevelInternalDataPath(GetLevel() ) );

	if (mpImageData) {
		OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( mip_level_vol_path, GetSize(), mpImageData);
	}

	mVolumeDataDirty = false;
}

void OmSimpleChunk::ReadMetaData()
{
	OmHdf5Path dat_file_path;
	dat_file_path.setPathQstr( mpMipVolume->MipChunkMetaDataPath(mCoordinate) );

	//read archive if it exists
	if (OmProjectData::GetProjectDataReader()->dataset_exists(dat_file_path)) {
		OmDataArchiveQT::ArchiveRead(dat_file_path, this);
	}
}

void OmSimpleChunk::WriteMetaData()
{
	OmHdf5Path dat_file_path;
	dat_file_path.setPathQstr(mpMipVolume->MipChunkMetaDataPath(mCoordinate));

	OmDataArchiveQT::ArchiveWrite(dat_file_path, this);

	mMetaDataDirty = false;
}

/////////////////////////////////
///////          MetaData Accessors

const SegmentDataSet & OmSimpleChunk::GetModifiedVoxelValues()
{
	return mModifiedVoxelValues;
}

void OmSimpleChunk::ClearModifiedVoxelValues()
{
	mModifiedVoxelValues.clear();
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
	quint8* char_ptr = (quint8*) mpImageData;
	uint32_t* int_ptr = (uint32_t*) mpImageData;

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
	mVolumeDataDirty = true;
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
	quint8* char_ptr = (quint8*) mpImageData;
	uint32_t* int_ptr = (uint32_t*) mpImageData;

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
void OmSimpleChunk::SetImageData(void * pImageData)
{

	//assert valid pointer
	assert(pImageData);

	//forget about making sure given image data has expected dimensions

	if (mpImageData) {
		free(mpImageData);
		mpImageData=NULL;
	}

	//set this image data to given
	mpImageData = pImageData;

	//set data causes chunk to be open and dirty
	SetOpen(true);
	mVolumeDataDirty = true;

	//remove image data size from cache (convert to bytes)
	int est_mem_bytes = GetSize();
	UpdateSize(float (est_mem_bytes) * MIP_CHUNK_DATA_SIZE_SCALE_FACTOR);
}

/////////////////////////////////
///////          Mip Accessors

int OmSimpleChunk::GetLevel()
{
	return mCoordinate.Level;
}

bool OmSimpleChunk::IsRoot()
{
	//if parent is null
	return mParentCoord == OmMipChunkCoord::NULL_COORD;
}

bool OmSimpleChunk::IsLeaf()
{
	return mCoordinate.Level == 0;
}

const OmMipChunkCoord & OmSimpleChunk::GetCoordinate()
{
	return mCoordinate;
}

const OmMipChunkCoord & OmSimpleChunk::GetParentCoordinate()
{
	return mParentCoord;
}

const set < OmMipChunkCoord > & OmSimpleChunk::GetChildrenCoordinates()
{
	return mChildrenCoordinates;
}

/////////////////////////////////
///////          Property Accessors

const NormBbox & OmSimpleChunk::GetNormExtent()
{
	return mNormExtent;
}

const NormBbox & OmSimpleChunk::GetClippedNormExtent()
{
	return mClippedNormExtent;
}

/////////////////////////////////
///////          MetaData Accessors

/*
 *	Returns reference to set of all values directly contained by
 *	the image data of this MipChunk
 */
const SegmentDataSet & OmSimpleChunk::GetDirectDataValues()
{
	loadMetadataIfPresent();
	return mDirectlyContainedValues;
}

/*
 *	Returns reference to set of values indirectly (spatially) contained
 *	by the bounds of this MipChunk.  That is all the direcly contained
 *	values of the children of this MipChunk.
 */
const SegmentDataSet & OmSimpleChunk::GetIndirectDataValues()
{
	loadMetadataIfPresent();
	return mIndirectlyContainedValues;
}

/*
 *	Returns reference to set of all values directly contained by
 *	the image data of this MipChunk
 */
SegmentDataSet & OmSimpleChunk::GetDirectDataValuesInternal()
{
	loadMetadataIfPresent();
	return mDirectlyContainedValues;
}

/*
 *	Returns reference to set of values indirectly (spatially) contained
 *	by the bounds of this MipChunk.  That is all the direcly contained
 *	values of the children of this MipChunk.
 */
SegmentDataSet & OmSimpleChunk::GetIndirectDataValuesInternal()
{
	loadMetadataIfPresent();
	return mIndirectlyContainedValues;
}

void OmSimpleChunk::loadMetadataIfPresent()
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
void OmSimpleChunk::RefreshDirectDataValues()
{
	//uses mpImageData so ensure chunk is open
	Open();

	//clear previous segments
	GetDirectDataValuesInternal().clear();

	//get number of values in chunk array
	int NumberOfValues = GetSize()/GetBytesPerSample();
	int index;

	//get pointer to native scalar data
	if (SEGMENT_DATA_BYTES_PER_SAMPLE == GetBytesPerSample()) {
		SEGMENT_DATA_TYPE *p_scalar_data = static_cast < SEGMENT_DATA_TYPE * >(mpImageData);

		//for all voxels in the chunk
		for ( index = 0;index < NumberOfValues;index++) {
			//if non-null insert in set
			if (NULL_SEGMENT_DATA != *p_scalar_data) {
				GetDirectDataValuesInternal().insert(*p_scalar_data);
			}
			//adv to next scalar
			++p_scalar_data;
		}
	} else if (1 == GetBytesPerSample()) {
		unsigned char *p_scalar_data = static_cast < unsigned char *>(mpImageData);
		SEGMENT_DATA_TYPE my_scalar_data;

		//for all voxels in the chunk
		for ( index = 0;index < NumberOfValues;index++) {
			//if non-null insert in set
			if ('\0' != *p_scalar_data) {
				my_scalar_data = (SEGMENT_DATA_TYPE) (*p_scalar_data);
				GetDirectDataValuesInternal().insert(my_scalar_data);
			}
			//adv to next scalar
			++p_scalar_data;
		}
	}

	//note metadata is dirty
	mMetaDataDirty = true;
}

/*
 *	Update the spacially contained segment information of a chunk associated to a given MipCoord.
 *	Leafs have the have spatial ids set to previously computed data ids.  Chunks with children
 *	have spacial content of union of all children's spacial content.
 */
void OmSimpleChunk::RefreshIndirectDataValues()
{

	//uses mpImageData so ensure chunk is open
	Open();

	//clear previous segment ids
	GetIndirectDataValuesInternal().clear();

	//if leaf, then spatial is data
	if (IsLeaf()) {
		GetIndirectDataValuesInternal() = GetDirectDataValuesInternal();
		return;
	}
	//for each valid child
	foreach( OmMipChunkCoord coord, mChildrenCoordinates ){

		//get child
		QExplicitlySharedDataPointer < OmSimpleChunk > p_child_chunk = QExplicitlySharedDataPointer < OmSimpleChunk > ();
		mpMipVolume->GetSimpleChunkThreadedCache()->GetChunk(p_child_chunk, coord );
		const SegmentDataSet & r_child_indirect_data_values = p_child_chunk->GetIndirectDataValuesInternal();

		//insert spatially contained children
		GetIndirectDataValuesInternal().unite(r_child_indirect_data_values);

	}

	//note metadata is dirty
	mMetaDataDirty = true;
}

/////////////////////////////////
///////          Slice Data Access

/*
 *	Returns a bounding box that represents a slice of the volume
 *	along the specified plane at the specified coordinate.
 */

AxisAlignedBoundingBox < int > OmSimpleChunk::ExtractSliceExtent(OmDataVolumePlane plane, int coord)
{
	//ensure open
	Open();

	//create slice bounding box
	AxisAlignedBoundingBox < int >slice_bbox;

	//get min max of current extent
	const Vector3 < int >&data_extent_min = GetExtent().getMin();
	const Vector3 < int >&data_extent_max = GetExtent().getMax();

	switch (plane) {
	case VOL_XY_PLANE:
		slice_bbox.setMin(Vector3 < int >(data_extent_min.x, data_extent_min.y, coord));
		slice_bbox.setMax(Vector3 < int >(data_extent_max.x, data_extent_max.y, coord));
		break;

	case VOL_XZ_PLANE:
		slice_bbox.setMin(Vector3 < int >(data_extent_min.x, coord, data_extent_min.z));
		slice_bbox.setMax(Vector3 < int >(data_extent_max.x, coord, data_extent_max.z));
		break;

	case VOL_YZ_PLANE:
		slice_bbox.setMin(Vector3 < int >(coord, data_extent_min.y, data_extent_min.z));
		slice_bbox.setMax(Vector3 < int >(coord, data_extent_max.y, data_extent_max.z));
		break;
	}

	return slice_bbox;
}

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
void *OmSimpleChunk::ExtractDataSlice(OmDataVolumePlane plane, int offset, Vector2 < int >&sliceDims, bool fast)
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
	// create two difft types of pointers to this data;
	quint8* char_ptr = (quint8*) mpImageData;
	uint32_t* int_ptr = (uint32_t*) mpImageData;

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

	// create the two possible resulting arrays
	quint8* char_result = (quint8*) malloc(sliceDims.x*sliceDims.y);
	uint32_t* int_result = (uint32_t*) malloc(sliceDims.x*sliceDims.y*4);

	// 
	int index_x, index_y, sliceIndex;
	sliceIndex = 0;

	for (index_y=0; index_y<sliceDims.y; index_y++){
		for (index_x=0; index_x<sliceDims.x; index_x++){
			if (bytesPerSample == 1) char_result[sliceIndex] = char_ptr[imageIndex];
			if (bytesPerSample == 4) int_result[sliceIndex] = int_ptr[imageIndex];
			imageIndex+=imageIncrement.x;
			sliceIndex++;
		}
		imageIndex += imageIncrement.y;
	}
	
	if  (bytesPerSample == 1) return (void*) char_result;
	if  (bytesPerSample == 4) return (void*) int_result;
	
	assert(0);
}

/////////////////////////////////
///////          Meshing

/*
 *	Returns new ImageData containing the entire extent of data needed
 *	to form continuous meshes with adjacent MipChunks.  This means an extra
 *	voxel of data is included on each dimensions.
 */
void *OmSimpleChunk::GetMeshImageData()
{

	//This function should be reconsidered . . . I'm hoping
	// Alex's code can be modified such that 
	// The edges of the volume can be 'covered'
	// without adding a voxel layer.
	// this would prevent an unnecessary mem-copy
	// and would result in a faster mesh.

	assert(0);

/*	Open ();

	DataCoord data_dims;
	mpImageData->GetDimensions(data_dims.array);
	DataCoord mesh_data_dims = data_dims + DataCoord::ONE;

	//alloc new mesh volume data
	vtkImageData *p_mesh_data = vtkImageData::New();
	p_mesh_data->SetDimensions(mesh_data_dims.array);
	p_mesh_data->SetScalarType(bytesToVtkScalarType(SEGMENT_DATA_BYTES_PER_SAMPLE));
	p_mesh_data->SetNumberOfScalarComponents(SEGMENT_DATA_SAMPLES_PER_PIXEL);
	p_mesh_data->AllocateScalars();

	//initialize data
	OmImageDataIo::clearImageData(p_mesh_data);

	//for all 8 adjacent chunks
	for (int z = 0; z < 2; z++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {

				//form mip coord
				OmMipChunkCoord mip_coord(mCoordinate.getLevel(),
							  mCoordinate.getCoordinateX() + x,
							  mCoordinate.getCoordinateY() + y, 
							  mCoordinate.getCoordinateZ() + z);

				//skip invalid mip coord
				if (!mpMipVolume->ContainsMipChunkCoord(mip_coord))
					continue;

				//else get chunk
				QExplicitlySharedDataPointer < OmSimpleChunk > p_chunk;
				mpMipVolume->GetSimpleChunkThreadedCache()->Get(p_chunk, mip_coord);

				p_chunk->Open();

				debug("mipchunk", "got mpImageData: %i, rc:%i\n", p_chunk->mpImageData, p_chunk->mpImageData->GetReferenceCount());

				//get dim size
				int chunk_dim = mpMipVolume->GetChunkDimension();

				//copy intersected data from src to mesh
				Vector3 < int >offset = Vector3 < int >(x * chunk_dim, y * chunk_dim, z * chunk_dim);
				QMutexLocker locker(mOpenLock);
				OmImageDataIo::copyIntersectedImageDataFromOffset(p_mesh_data, p_chunk->mpImageData, offset);
				
				p_chunk = QExplicitlySharedDataPointer  < OmSimpleChunk > ();
				mpMipVolume->Remove(mip_coord);
			}
		}
	}
*/
	void* meshImageData;
	return meshImageData;  //p_mesh_data;
}

/////////////////////////////////
///////          Drawing

/*
 *	Given that the chunk is visible, determine if it should be drawn
 *	or if we should continue refining so as to draw children.
 */

bool OmSimpleChunk::DrawCheck(OmVolumeCuller & rCuller)
{
	//draw if leaf
	if (IsLeaf()) {
		return true;
	}

	NormCoord camera = rCuller.GetPosition();
	NormCoord center = mClippedNormExtent.getCenter();

	float camera_to_center = center.distance(camera);
	float distance = (mNormExtent.getMax() - mNormExtent.getCenter()).length();

	//if distance too large, just draw it - else keep breaking it down
	//debug("view3d", "backoff: %d\n", myBackoff );
	return (camera_to_center > distance);
}

/**
 * This funcion has been shut off I believe
 * perhaps it should be deleted
 *
 */
void OmSimpleChunk::DrawClippedExtent()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	//disable lighting for lines
	glDisable(GL_LIGHTING);

	//translate and scale to chunk norm extent
	Vector3f translate = mClippedNormExtent.getMin();
	Vector3f scale = mClippedNormExtent.getMax() - mClippedNormExtent.getMin();

	//transform model view
	glTranslatefv(translate.array);
	glScalefv(scale.array);

	glTranslatef(0.5, 0.5, 0.5);
	glColor3f(0.5, 0.5, 0.5);
	//omglWireCube(1);
	glTranslatef(-0.5, -0.5, -0.5);

	//glScalefv( (Vector3f::ONE/scale).array);
	//glTranslatefv( (-translate).array);

	glPopMatrix();
	glPopAttrib();
}

bool OmSimpleChunk::IsOpen()
{
	return mIsOpen;
}

void OmSimpleChunk::SetOpen(bool state)
{
	mIsOpen = state;
}

int OmSimpleChunk::GetBytesPerSample()
{
	return mpMipVolume->GetBytesPerSample();
}

bool OmSimpleChunk::ContainsVoxel(const DataCoord & vox)
{
	return GetExtent().contains(vox);
}

const Vector3 < int > OmSimpleChunk::GetDimensions()
{
	return GetExtent().getUnitDimensions();
}
