
#include "omMipChunk.h"
#include "omMipVolume.h"
#include "omVolumeCuller.h"
#include "omSegmentation.h"

#include "segment/omSegment.h"
#include "system/omSystemTypes.h"
#include "system/omStateManager.h"
#include "system/omProjectData.h"
#include "common/omUtility.h"
#include "common/omGl.h"
#include "common/omVtk.h"
#include "utility/omImageDataIo.h"

#include <vtkImageData.h>
#include <vtkType.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include "common/omDebug.h"
namespace bfs = boost::filesystem;

#define DEBUG 0

static const float MIP_CHUNK_DATA_SIZE_SCALE_FACTOR = 1.4f;

#pragma mark -
#pragma mark OmMipChunk Class
/////////////////////////////////
///////
///////         OmMipChunk Class
///////

/*
 *	Constructor speicifies MipCoord and MipVolume the chunk extracts data from
 *	Note: optional cache pointer if this is a cached chunk
 */
OmMipChunk::OmMipChunk(const OmMipChunkCoord & rMipCoord, OmMipVolume * pMipVolume)
:OmCacheableBase(dynamic_cast < OmCacheBase * >(pMipVolume)), mpMipVolume(pMipVolume)
{

	//debug("genone","OmMipChunk::OmMipChunk()");   

	//init chunk properties
	InitChunk(rMipCoord);

	//read meta data if needed
	if (mpMipVolume->GetChunksStoreMetaData()) {
		ReadMetaData();
	}

	//update cache size
	UpdateSize(sizeof(OmMipChunk));

	mpImageData = NULL;
	mOpenLock = new QMutex();
}

OmMipChunk::~OmMipChunk()
{
	//debug("genone","OmMipChunk::~OmMipChunk()");

	//since parent destructor is called after this child destructor, we need to call
	//child Close() here, or else child Close() won't be called (since child won't exist)
	//when called in parent destructor
	if (IsOpen())
		Close();

	//remove object size from cache
	UpdateSize(-int (sizeof(OmMipChunk)));
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

	//set bytes per sample
	mBytesPerSample = mpMipVolume->GetBytesPerSample();

	//set if mipvolume uses metadata
	mMetaDataDirty = false;
}

#pragma mark
#pragma mark DataVolume Methods
/////////////////////////////////
///////          DataVolume Methods

/*
 *	Overridden to load data from project data mip volume.
 */
void OmMipChunk::Open()
{
        QMutexLocker locker(mOpenLock);

	//ignore if already open
	if (IsOpen())
		return;


	//read volume data
	ReadVolumeData();

	debug("mipchunk", "read in mpImageData: %i, rc:%i\n", mpImageData, mpImageData->GetReferenceCount());

	//set open
	SetOpen(true);
}

void OmMipChunk::OpenForWrite()
{
	//debug("genone","OmMipChunk::OpenForWrite()");

	//ignore if already open
	if (IsOpen())
		return;

	//get path to mip level volume
	string mip_level_vol_path = mpMipVolume->MipLevelInternalDataPath(GetLevel());

	//read from project data
	//assert(OmProjectData::DataExists(mip_level_vol_path));
	vtkImageData *data = OmProjectData::ReadImageData(mip_level_vol_path, GetExtent(), GetBytesPerSample());
	//debug("FIXME", << "BPS: " << GetBytesPerSample() << endl;

	//set this image data
	SetImageData(data);

	//set open
	SetOpen(true);
}

/*
 *	Flush data to project data mip volume.
 */
void OmMipChunk::Flush()
{
	//debug("genone", "OmMipChunk::Flush()\n" );

	//only write if dirty
	if (IsVolumeDataDirty()) {
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
void OmMipChunk::Close()
{
	//debug("genone","OmMipChunk::Close()\n");

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
	int est_mem_bytes = mpImageData->GetEstimatedMemorySize() * 1024;
	UpdateSize(-float (est_mem_bytes) * MIP_CHUNK_DATA_SIZE_SCALE_FACTOR);

	//delete image data
	debug("mipchunk", "freeing mpImageData: %p, rc:%i\n", mpImageData, mpImageData->GetReferenceCount());
	debug("meshercrash", "freeing mpImageData: %p, rc:%i\n", mpImageData, mpImageData->GetReferenceCount());
	mpImageData->Delete();
	mpImageData = NULL;
}

#pragma mark
#pragma mark Data Properties
/////////////////////////////////
///////          Data Properties

const DataBbox & OmMipChunk::GetExtent()
{
	return mDataExtent;
}

#pragma mark
#pragma mark Dirty Methods
/////////////////////////////////
///////          Dirty Methods

bool OmMipChunk::IsDirty() const
{
	return IsVolumeDataDirty() || IsMetaDataDirty();
}

bool OmMipChunk::IsVolumeDataDirty() const
{
	return mVolumeDataDirty;
}

bool OmMipChunk::IsMetaDataDirty() const
{
	return mpMipVolume->GetChunksStoreMetaData() && mMetaDataDirty;
}

#pragma mark
#pragma mark  Data IO
/////////////////////////////////
///////           Data IO

void OmMipChunk::ReadVolumeData()
{
	//get path to mip level volume
	string mip_level_vol_path = mpMipVolume->MipLevelInternalDataPath(GetLevel());

	//read from project data
	if (!OmProjectData::DataExists(mip_level_vol_path)) {
		assert(0);
		throw OmIoException("no MIP data found");
	}

	vtkImageData *data = OmProjectData::ReadImageData(mip_level_vol_path, GetExtent(), GetBytesPerSample());
	debug("mipchunk", "data: %i, refcount of:%i\n", data, data->GetReferenceCount ());

	//set this image data
	SetImageData(data);

	// Need to undo the side effect caused by setting the image data. Don't want to write out the data just
	// because we set the newly loaded data.
	mVolumeDataDirty = false;
}

void OmMipChunk::WriteVolumeData()
{
	if (!IsOpen())
		OpenForWrite();
	//assert(IsOpen());
	//get path to mip level volume
	string mip_level_vol_path = mpMipVolume->MipLevelInternalDataPath(GetLevel());
	//write to project data
	if (mpImageData)
		OmProjectData::WriteImageData(mip_level_vol_path, GetExtent(), GetBytesPerSample(), mpImageData);
	//data clean
	mVolumeDataDirty = false;
}

void OmMipChunk::ReadMetaData()
{
	// TODO: must we do this? (purcaro)

	string fpath = mpMipVolume->MipChunkMetaDataPath(mCoordinate);

	//read archive if it exists
	if (OmProjectData::DataExists(fpath)) {
		OmProjectData::ArchiveRead < OmMipChunk > (fpath, this);
	}

	//otherwise, no metadata to read in
}

void OmMipChunk::WriteMetaData()
{
	string fpath = mpMipVolume->MipChunkMetaDataPath(mCoordinate);

	OmProjectData::ArchiveWrite < OmMipChunk > (fpath, this);

	//meta data clean
	mMetaDataDirty = false;
}

#pragma mark
#pragma mark MetaData Accessors
/////////////////////////////////
///////          MetaData Accessors

const SegmentDataSet & OmMipChunk::GetModifiedVoxelValues() const
{
	return mModifiedVoxelValues;
}

void OmMipChunk::ClearModifiedVoxelValues()
{
	mModifiedVoxelValues.clear();
}

#pragma mark
#pragma mark Data Accessors
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
	void *p_scalar = mpImageData->GetScalarPointer(offset.x, offset.y, offset.z);

	//cast to appropriate type and return as uint
	switch (mBytesPerSample) {
	case 1:
		*((uint8_t *) p_scalar) = (uint8_t) val;
		break;
	case 4:
		*((uint32_t *) p_scalar) = val;
		break;
	default:
		//assert(false && ""What?! Data must be 1 or 4 bytes large");
		//debug("FIXME", << "What?! Data must be 1 or 4 bytes large, not " << mBytesPerSample << endl;
		break;
	}

	//data volume now dirty
	mVolumeDataDirty = true;
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
	void *p_scalar = mpImageData->GetScalarPointer(offset.x, offset.y, offset.z);

	//cast to appropriate type and return as uint
	switch (mBytesPerSample) {
	case 1:
		return *((uint8_t *) p_scalar);
	case 4:
		return *((uint32_t *) p_scalar);
	default:
		assert(false);
	}
}

/*
 *	Set the image data of this MipChunk to data at the given pointer.
 *	Closes if already open and sets MipChunk to be open and dirty.
 */
void OmMipChunk::SetImageData(vtkImageData * pImageData)
{

	//assert valid pointer
	assert(pImageData);

	//make sure given image data has expected dimensions
	Vector3 < int >dims;
	pImageData->GetDimensions(dims.array);
	assert(dims == GetDimensions());

	//close if already open
	//if(IsOpen()) Close();

	if (mpImageData) {
		mpImageData->Delete ();
	}

	//set this image data to given
	mpImageData = pImageData;

	//set data causes chunk to be open and dirty
	SetOpen(true);
	mVolumeDataDirty = true;

	//get property from set data
	mBytesPerSample = mpImageData->GetScalarSize();

	//remove image data size from cache (convert to bytes)
	int est_mem_bytes = mpImageData->GetEstimatedMemorySize() * 1024;
	UpdateSize(float (est_mem_bytes) * MIP_CHUNK_DATA_SIZE_SCALE_FACTOR);
}

#pragma mark
#pragma mark Mip Accessors
/////////////////////////////////
///////          Mip Accessors

int OmMipChunk::GetLevel() const
{
	return mCoordinate.Level;
}

bool OmMipChunk::IsRoot() const
{
	//if parent is null
	return mParentCoord == OmMipChunkCoord::NULL_COORD;
}

bool OmMipChunk::IsLeaf() const
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

#pragma mark
#pragma mark Property Accessors
/////////////////////////////////
///////          Property Accessors

const NormBbox & OmMipChunk::GetNormExtent() const
{
	return mNormExtent;
}

const NormBbox & OmMipChunk::GetClippedNormExtent() const
{
	return mClippedNormExtent;
}

#pragma mark
#pragma mark MetaData Accessors
/////////////////////////////////
///////          MetaData Accessors

/*
 *	Returns reference to set of all values directly contained by
 *	the image data of this MipChunk
 */
const SegmentDataSet & OmMipChunk::GetDirectDataValues() const
{
	return mDirectlyContainedDataValuesSet;
}

/*
 *	Returns reference to set of values indirectly (spatially) contained
 *	by the bounds of this MipChunk.  That is all the direcly contained
 *	values of the children of this MipChunk.
 */
const SegmentDataSet & OmMipChunk::GetIndirectDataValues() const
{
	return mIndirectlyContainedDataValuesSet;
}

/*
 *	Analyze segmentation ImageData in the chunk associated to a MipCoord and store 
 *	all values in the DataSegmentId set of the chunk.
 */
void OmMipChunk::RefreshDirectDataValues()
{

	//uses mpImageData so ensure chunk is open
	Open();

	//clear previous segments
	mDirectlyContainedDataValuesSet.clear();

	//get data extent (varify it is a chunk)
	int extent[6];
	mpImageData->GetExtent(extent);

	//get pointer to native scalar data
	//assert(mpImageData->GetScalarSize() == SEGMENT_DATA_BYTES_PER_SAMPLE);
	if (SEGMENT_DATA_BYTES_PER_SAMPLE == mpImageData->GetScalarSize()) {
		SEGMENT_DATA_TYPE *p_scalar_data = static_cast < SEGMENT_DATA_TYPE * >(mpImageData->GetScalarPointer());

		//for all voxels in the chunk
		int x, y, z;
		for (z = extent[0]; z <= extent[1]; z++)
			for (y = extent[2]; y <= extent[3]; y++)
				for (x = extent[4]; x <= extent[5]; x++) {

					//if non-null insert in set
					if (NULL_SEGMENT_DATA != *p_scalar_data) {
						mDirectlyContainedDataValuesSet.insert(*p_scalar_data);
					}
					//adv to next scalar
					++p_scalar_data;
				}

	} else if (1 == mpImageData->GetScalarSize()) {
		unsigned char *p_scalar_data = static_cast < unsigned char *>(mpImageData->GetScalarPointer());

		//for all voxels in the chunk
		int x, y, z;
		for (z = extent[0]; z <= extent[1]; z++)
			for (y = extent[2]; y <= extent[3]; y++)
				for (x = extent[4]; x <= extent[5]; x++) {

					//if non-null insert in set
					if ('\0' != *p_scalar_data) {
						SEGMENT_DATA_TYPE my_scalar_data = (SEGMENT_DATA_TYPE) (*p_scalar_data);
						mDirectlyContainedDataValuesSet.insert(my_scalar_data);
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
void OmMipChunk::RefreshIndirectDataValues()
{

	//uses mpImageData so ensure chunk is open
	Open();

	//clear previous segment ids
	mIndirectlyContainedDataValuesSet.clear();

	//if leaf, then spatial is data
	if (IsLeaf()) {
		mIndirectlyContainedDataValuesSet = mDirectlyContainedDataValuesSet;
		return;
	}
	//for each valid child
	set < OmMipChunkCoord >::iterator itr;
	for (itr = mChildrenCoordinates.begin(); itr != mChildrenCoordinates.end(); itr++) {
		//get child
		shared_ptr < OmMipChunk > p_child_chunk = shared_ptr < OmMipChunk > ();
		mpMipVolume->GetChunk(p_child_chunk, *itr);
		const SegmentDataSet & r_child_indirect_data_values = p_child_chunk->GetIndirectDataValues();

		//insert spatially contained children
		mIndirectlyContainedDataValuesSet.insert(r_child_indirect_data_values.begin(),
							 r_child_indirect_data_values.end());

	}

	//note metadata is dirty
	mMetaDataDirty = true;
}

#pragma mark
#pragma mark Slice Data Access
/////////////////////////////////
///////          Slice Data Access

/*
 *	Returns a bounding box that represents a slice of the volume
 *	along the specified plane at the specified coordinate.
 */

AxisAlignedBoundingBox < int > OmMipChunk::ExtractSliceExtent(OmDataVolumePlane plane, int coord)
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

	default:
		assert(false);
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
void *OmMipChunk::ExtractDataSlice(OmDataVolumePlane plane, int offset, Vector2 < int >&sliceDims, bool fast)
{
	//debug("genone","OmMipChunk::ExtractDataSlice()\n");

	if (!IsOpen() && fast)	// If we want to extract fast leave fast if not open.
		return NULL;

	//ensure open
	Open();

	//get global slice coordinate
	int slice_coord;
	switch (plane) {
	case VOL_XY_PLANE:
		slice_coord = GetExtent().getMin().z + offset;
		break;
	case VOL_XZ_PLANE:
		slice_coord = GetExtent().getMin().y + offset;
		break;
	case VOL_YZ_PLANE:
		slice_coord = GetExtent().getMin().x + offset;
		break;
	default:
		assert(false);
	}

	//get bbox for slice
	AxisAlignedBoundingBox < int >slice_extent = ExtractSliceExtent(plane, slice_coord);
	Vector3 < int >slice_dims = slice_extent.getUnitDimensions();

	//form result dims
	switch (plane) {
	case VOL_XY_PLANE:
		sliceDims.x = slice_dims.x;
		sliceDims.y = slice_dims.y;
		break;
	case VOL_XZ_PLANE:
		sliceDims.x = slice_dims.x;
		sliceDims.y = slice_dims.z;
		break;
	case VOL_YZ_PLANE:
		sliceDims.x = slice_dims.y;
		sliceDims.y = slice_dims.z;
		break;
	default:
		assert(false);
	}

	//convert to extent relative to chunk image data
	AxisAlignedBoundingBox < int >relative_slice_bbox = slice_extent;
	relative_slice_bbox.offset(-GetExtent().getMin());

	//return pointer to data
	return copyImageData(mpImageData, relative_slice_bbox);
}

#pragma mark
#pragma mark Meshing
/////////////////////////////////
///////          Meshing

/*
 *	Returns new ImageData containing the entire extent of data needed
 *	to form continuous meshes with adjacent MipChunks.  This means an extra
 *	voxel of data is included on each dimensions.
 */
vtkImageData *OmMipChunk::GetMeshImageData()
{
	//debug("FIXME", << "OmMipChunk::GetMeshImageData: " << mCoordinate << endl;
	Open ();

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
	clearImageData(p_mesh_data);

	//for all 8 adjacent chunks
	for (int z = 0; z < 2; z++)
		for (int y = 0; y < 2; y++)
			for (int x = 0; x < 2; x++) {

				//form mip coord
				OmMipChunkCoord mip_coord(mCoordinate.get < 0 > (),
							  mCoordinate.get < 1 > () + x,
							  mCoordinate.get < 2 > () + y, mCoordinate.get < 3 > () + z);

				//skip invalid mip coord
				if (!mpMipVolume->ContainsMipChunkCoord(mip_coord))
					continue;

				//else get chunk
				shared_ptr < OmMipChunk > p_chunk;
				mpMipVolume->GetChunk(p_chunk, mip_coord);

				p_chunk->Open();

				debug("mipchunk", "got mpImageData: %i, rc:%i\n", p_chunk->mpImageData, p_chunk->mpImageData->GetReferenceCount());


				//get pointer to image data
				//vtkImageData *p_src_data = p_chunk->mpImageData;

				debug ("genone", "got %i\n", p_chunk->mpImageData);

				//get dim size
				int chunk_dim = mpMipVolume->GetChunkDimension();

				//copy intersected data from src to mesh
				Vector3 < int >offset = Vector3 < int >(x * chunk_dim, y * chunk_dim, z * chunk_dim);
				copyIntersectedImageDataFromOffset(p_mesh_data, p_chunk->mpImageData, offset);

				//p_chunk = shared_ptr < OmMipChunk > ();
				//mpMipVolume->Remove (mip_coord);
			}

	return p_mesh_data;
	//return NULL;
}

/*
DataBbox sliceDataBbox(const DataBbox &bbox, OmDataVolumePlane plane, int depth) {
	DataBbox slice_bbox;
	const Vector3<int> &data_extent_min = bbox.getMin();
	const Vector3<int> &data_extent_max = bbox.getMax();
	
	switch(plane) {
		case VOL_XY_PLANE:
			if(depth < data_extent_min.z || depth > data_extent_max.z)
				throw OmAccessException("Specified depth not valid.");
			
			slice_bbox.setMin(Vector3<int>(data_extent_min.x, data_extent_min.y, depth));
			slice_bbox.setMax(Vector3<int>(data_extent_max.x, data_extent_max.y, depth));
			break;
			
		case VOL_XZ_PLANE:
			if(depth < data_extent_min.y || depth > data_extent_max.y)
				throw OmAccessException("Specified depth not valid.");
			
			slice_bbox.setMin(Vector3<int>(data_extent_min.x, depth, data_extent_min.z));
			slice_bbox.setMax(Vector3<int>(data_extent_max.x, depth, data_extent_max.z));
			break;
			
		case VOL_YZ_PLANE:
			if(depth < data_extent_min.x || depth > data_extent_max.x)
				throw OmAccessException("Specified depth not valid.");
			
			slice_bbox.setMin(Vector3<int>(depth, data_extent_min.y, data_extent_min.z));
			slice_bbox.setMax(Vector3<int>(depth, data_extent_max.y, data_extent_max.z));
			break;
			
		default:
			assert(false);
	}
	
	return slice_bbox;
}


*/

#pragma mark
#pragma mark Drawing
/////////////////////////////////
///////          Drawing

/*
 *	Given that the chunk is visible, determine if it should be drawn
 *	or if we should continue refining so as to draw children.
 */

bool OmMipChunk::DrawCheck(const OmVolumeCuller & rCuller)
{
	//draw if leaf
	if (IsLeaf())
		return true;

	NormCoord camera = rCuller.GetPosition();
	//NormCoord center = mNormExtent.getCenter();
	NormCoord center = mClippedNormExtent.getCenter();

	float camera_to_center = center.distance(camera);
	float distance = (mNormExtent.getMax() - mNormExtent.getCenter()).length();

	//if distance too large, just draw it - else keep breaking it down
	//debug("view3d", "backoff: %d\n", myBackoff );
	return (camera_to_center > distance / OmStateManager::getMyBackoff() );
}

void OmMipChunk::DrawClippedExtent()
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
	omglWireCube(1);
	glTranslatef(-0.5, -0.5, -0.5);

	//glScalefv( (Vector3f::ONE/scale).array);
	//glTranslatefv( (-translate).array);

	glPopMatrix();
	glPopAttrib();
}

#pragma mark
#pragma mark ostream
/////////////////////////////////
///////          ostream

ostream & operator<<(ostream & out, const OmMipChunk & mc)
{

	out << (OmDataVolume &) mc;

	//debug("FIXME", << "Root: " << mc.IsRoot() << "\n";
	//debug("FIXME", << "Leaf: " << mc.IsLeaf() << "\n";
	out << "Normalized Extent: " << mc.mNormExtent << "\n";

	return out;
}
