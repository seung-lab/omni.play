#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "project/omProject.h"
#include "utility/stringHelpers.h"
#include "common/omDebug.h"
#include "common/omException.h"
#include "common/omStd.h"
#include "common/omVtk.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "system/cache/omThreadChunkThreadedCache.h"
#include "system/events/omProgressEvent.h"
#include "system/omEventManager.h"
#include "system/omProjectData.h"
#include "utility/omImageDataIo.h"
#include "utility/omTimer.h"
#include "utility/sortHelpers.h"
#include "volume/omMipChunk.h"
#include "volume/omMipThreadManager.h"
#include "volume/omMipVolume.h"
#include "volume/omThreadChunkLevel.h"
#include "volume/omVolume.h"

#include <vtkImageData.h>

#include <QImage>

//Maximum number of powers of 2 that thread chunk is linearly larger than a chunk
#define MAX_THREAD_CHUNK_EXPONENT 2

//TODO: Get BuildThreadedVolume() to display progress somehow using OmMipThread::GetThreadChunksDone()

OmMipVolume::OmMipVolume()
	: mDataCache(new OmMipVolumeCache(this))
	, mVolDataType(OmVolDataType::UNKNOWN)
{
	sourceFilesWereSet = false;

	//init
	SetBuildState(MIPVOL_UNBUILT);
	SetChunksStoreMetaData(false);
}

OmMipVolume::~OmMipVolume()
{
	Flush();
	delete mDataCache;
}

/////////////////////////////////
///////          DataVolume

/*
 *	Flush all changes to disk
 */
void OmMipVolume::Flush()
{
	if(!OmProject::GetCanFlush()) {
		return;
	}

	//build any edited leaf chunks
	BuildEditedLeafChunks();

	//flush all chunks in the cache
	mDataCache->Flush();
}

/**
 *	Returns data path to internal MipLevel data.
 */
std::string OmMipVolume::MipLevelInternalDataPath(const int level)
{
	return OmDataPaths::MipLevelInternalDataPath(GetDirectoryPath(), level);
}

/**
 *	Returns path to MetaData of specified chunk
 */
std::string OmMipVolume::MipChunkMetaDataPath(const OmMipChunkCoord & rMipCoord)
{
	if(!GetChunksStoreMetaData()){
		throw new OmIoException("this mip volume has no chunk metadata????");
	}

	return OmDataPaths::MipChunkMetaDataPath(GetDirectoryPath(), rMipCoord);
}

/////////////////////////////////
///////          Source Data Properties
void OmMipVolume::SetSourceFilenamesAndPaths( QFileInfoList sourceFilenamesAndPaths )
{
	mSourceFilenamesAndPaths = SortHelpers::sortNaturally(sourceFilenamesAndPaths);
	sourceFilesWereSet = true;
}

QFileInfoList OmMipVolume::GetSourceFilenamesAndPaths()
{
	return mSourceFilenamesAndPaths;
}

bool OmMipVolume::IsSourceValid()
{
	if( mSourceFilenamesAndPaths.empty() ){
		return false;
	}

	foreach( const QFileInfo & fi, mSourceFilenamesAndPaths ){
		if( !fi.exists() ){
			return false;
		}
	}

	return true;
}

/////////////////////////////////
///////          Mip Data Properties


int OmMipVolume::GetChunkDimension()
{
	return OmVolume:: GetChunkDimension();
}

Vector3i OmMipVolume::GetChunkDimensions()
{
	return Vector3i(GetChunkDimension(), GetChunkDimension(), GetChunkDimension());
}

void OmMipVolume::SetChunksStoreMetaData(bool state)
{
	mStoreChunkMetaData = state;
}

bool OmMipVolume::GetChunksStoreMetaData()
{
	return mStoreChunkMetaData;
}

bool OmMipVolume::IsVolumeDataBuilt()
{
	return MIPVOL_BUILT == mBuildState;
}

bool OmMipVolume::IsBuilding()
{
	return MIPVOL_BUILDING == mBuildState;
}

void OmMipVolume::SetBuildState(MipVolumeBuildState state)
{
	mBuildState = state;
}

/*
 *	Refresh dependent variables.
 */
void OmMipVolume::UpdateMipProperties(OmDataPath & dataset)
{
	if (IsSourceValid()) {
		//get source dimensions
		Vector3i source_dims = get_dims(dataset);

		debug("hdf5image", "%i:%i:%i, from %s and %s\n", DEBUGV3(source_dims));

		//if dim differs from OmVolume alert user
		if (OmVolume::GetDataDimensions() != source_dims) {
			//printf("OmMipVolume::UpdateMipProperties: CHANGING VOLUME DIMENSIONS\n");

			//update volume dimensions
			OmVolume::SetDataDimensions(source_dims);
		}
	}

	//check for valid mip chunk dim
	if (GetChunkDimension() % 2)
		throw OmFormatException("Chunk dimensions must be even.");

	//set properties based on given leaf dim and source data
	UpdateRootLevel();
}

/////////////////////////////////
///////          Mip Level Methods

/*
 *	Returns root level.  Based on size of source extent and leaf dimension.
 */
int OmMipVolume::GetRootMipLevel()
{
	return mMipRootLevel;
}

/*
 *	Calculate MipRootLevel using GetChunkDimension().
 */
void OmMipVolume::UpdateRootLevel()
{
	//determine max level
	Vector3i source_dims = GetDataExtent().getUnitDimensions();
	int max_source_dim = source_dims.getMaxComponent();
	int mipchunk_dim = GetChunkDimension();

	if (max_source_dim <= mipchunk_dim) {
		mMipRootLevel = 0;
	} else {
                mMipRootLevel = ceil(log((float) (max_source_dim) / GetChunkDimension()) / log((float)2));
	}
}

/*
 *	Calculate the data dimensions needed to contain the volume at a given compression level.
 */
Vector3i  OmMipVolume::MipLevelDataDimensions(int level)
{
	//get dimensions
	DataBbox source_extent = GetDataExtent();
	Vector3 < float >source_dims = source_extent.getUnitDimensions();

	//dims in fraction of pixels
	Vector3 < float >mip_level_dims = source_dims / OMPOW(2, level);

	return Vector3i (ceil(mip_level_dims.x),
			       ceil(mip_level_dims.y),
			       ceil(mip_level_dims.z));
}

// TODO: this appear to NOT do the right thing for level=0 (purcaro)

/*
 *	Calculate the MipChunkCoord dims required to contain all the chunks of a given level.
 */
Vector3i  OmMipVolume::MipLevelDimensionsInMipChunks(int level)
{
	Vector3 < float >data_dims = MipLevelDataDimensions(level);
	return Vector3i (ceil(data_dims.x / GetChunkDimension()),
			       ceil(data_dims.y / GetChunkDimension()),
			       ceil(data_dims.z / GetChunkDimension()));
}

/*
 *	Returns the number of MipChunks in a given MipLevel of the MipVolume
 */
int OmMipVolume::MipChunksInMipLevel(int level)
{
	Vector3i mip_dims = MipLevelDimensionsInMipChunks(level);
	return mip_dims.x * mip_dims.y * mip_dims.z;
}

/*
 *	Returns the number of MipChunks in all MipLevels of the MipVolume
 */
int OmMipVolume::MipChunksInVolume()
{
	int total = 0;
	for (int i = 0; i <= GetRootMipLevel(); i++) {
		total += MipChunksInMipLevel(i);
	}
	return total;
}

/////////////////////////////////
///////          Thread Chunk Methods


/*
 *	Gets dimension of a thread chunk, which incrceases with num of mip levels,
 *	but is at least as large as a mip chunk, and also bounded from above to prevent
 *	having thread chunks that are too large in memory
 */
int OmMipVolume::GetThreadChunkDimension()
{
/*
	int unbounded_chunk_dim = OMPOW(2, mMipRootLevel);
	int lower_bound = GetChunkDimension();
	int upper_bound = GetChunkDimension()*OMPOW(2, MAX_THREAD_CHUNK_EXPONENT);
	int bounded_chunk_dim = min(max(unbounded_chunk_dim,lower_bound),upper_bound);
	return bounded_chunk_dim;
*/
	return GetChunkDimension();
}

Vector3i  OmMipVolume::GetThreadChunkDimensions()
{
	return Vector3i (GetThreadChunkDimension(), GetThreadChunkDimension(), GetThreadChunkDimension());
}


/*
 *	Calculates maximum number of consecutive subsamples before a thread chunk bottoms out
 */
int OmMipVolume::GetMaxConsecutiveSubsamples()
{
	double max_consecutive_subsamples = log(GetThreadChunkDimension()) / log(2);
	return (int) max_consecutive_subsamples;
}

/*
 *	Gets dimension of a thread chunk level, which depends on the mipcoord level
 *	but flows over and loops back if it gets too small
 */
int OmMipVolume::GetThreadChunkLevelDimension(int level)
{
	int effective_level = level;
	int max_effective_level = GetMaxConsecutiveSubsamples();
	//Flow over so that thread levels never get too small
	effective_level = effective_level % max_effective_level;
	//Effective level is never zero
	if ( 0 == effective_level ){ effective_level = max_effective_level; }
	return GetThreadChunkDimension() / OMPOW(2, effective_level);
}

Vector3i  OmMipVolume::GetThreadChunkLevelDimensions(int level)
{
	return Vector3i (GetThreadChunkLevelDimension(level), GetThreadChunkLevelDimension(level), GetThreadChunkLevelDimension(level));
}

/*
 *	Calculate the MipChunkCoord dims required to contain all the thread chunks in a mip level.
 */
Vector3i  OmMipVolume::MipLevelDimensionsInThreadChunks(int level)
{
	Vector3 < float >data_dims = MipLevelDataDimensions(level);

	return Vector3i (ceil(data_dims.x / GetThreadChunkDimension()),
			       ceil(data_dims.y / GetThreadChunkDimension()),
			       ceil(data_dims.z / GetThreadChunkDimension()));
}

/*
 *	Calculate the MipChunkCoord dims required to contain all the thread chunks levels of that level in a mip level.
 */
Vector3i  OmMipVolume::MipLevelDimensionsInThreadChunkLevels(int level)
{
	Vector3 < float >data_dims = MipLevelDataDimensions(level);

	return Vector3i (ceil(data_dims.x / GetThreadChunkLevelDimension(level)),
			       ceil(data_dims.y / GetThreadChunkLevelDimension(level)),
			       ceil(data_dims.z / GetThreadChunkLevelDimension(level)));
}

/*
 *	Returns the number of ThreadChunks in a given MipLevel of the MipVolume.
 */
int OmMipVolume::ThreadChunksInMipLevel(int level)
{
	Vector3i thread_dims = MipLevelDimensionsInThreadChunks(level);
	return thread_dims.x * thread_dims.y * thread_dims.z;
}

/*
 *	Returns the total number of ThreadChunks that will be built in a MipVolume.
 */
int OmMipVolume::ThreadChunksInVolume()
{
	int total = 0;
	for (int initLevel = 0; initLevel < GetRootMipLevel(); initLevel += GetMaxConsecutiveSubsamples()) {
		total += ThreadChunksInMipLevel(initLevel);
	}
	return total;
}

/////////////////////////////////
///////          MipCoordinate Methods

/*
 *	Returns MipChunkCoord containing given data coordinate for given MipLevel
 */
OmMipChunkCoord OmMipVolume::DataToMipCoord(const DataCoord & dataCoord, int level)
{
	int data_dim = OMPOW(2, level) * GetChunkDimension();

	return OmMipChunkCoord(level,
			       floor(float (dataCoord.x) / data_dim),
			       floor(float (dataCoord.y) / data_dim), floor(float (dataCoord.z) / data_dim));
}

OmMipChunkCoord OmMipVolume::NormToMipCoord(const NormCoord & normCoord, int level)
{
	return DataToMipCoord(OmVolume::NormToDataCoord(normCoord), level);
}

/*
 *	Returns the extent of the data in specified level covered by the given MipCoordinate.
 */
DataBbox OmMipVolume::MipCoordToDataBbox(const OmMipChunkCoord & rMipCoord, int newLevel)
{

	int old_level_factor = OMPOW(2, rMipCoord.Level);
	int new_level_factor = OMPOW(2, newLevel);

	//convert to leaf level dimensions
	int leaf_dim = GetChunkDimension() * old_level_factor;
	Vector3i leaf_dims = GetChunkDimensions() * old_level_factor;

	//min of extent in leaf data coordinates
	DataCoord leaf_min_coord = rMipCoord.Coordinate * leaf_dim;

	//convert to new level
	DataCoord new_extent_min_coord = leaf_min_coord / new_level_factor;
	//DataCoord new_extent_max_coord = (leaf_min_coord + leaf_dims) / new_level_factor;
	Vector3i new_dims = leaf_dims / new_level_factor;

	//return
	return DataBbox(new_extent_min_coord, new_dims.x, new_dims.y, new_dims.z);
}

NormBbox OmMipVolume::MipCoordToNormBbox(const OmMipChunkCoord & rMipCoord)
{
	return OmVolume::DataToNormBbox(MipCoordToDataBbox(rMipCoord, 0));
}

/*
 *	Returns the extent of a thread chunk, indepndent of mip coord level
 */
DataBbox OmMipVolume::MipCoordToThreadDataBbox(const OmMipChunkCoord & rMipCoord)
{
	int thread_dim = GetThreadChunkDimension();
	Vector3i thread_dims = GetThreadChunkDimensions();

	//min of extent in parent data coordinates
	DataCoord thread_min_coord = rMipCoord.Coordinate * thread_dim;

	return DataBbox(thread_min_coord, thread_dims.x, thread_dims.y, thread_dims.z);
}

/*
 *	Returns the extent of a thread chunk level based on mip coord level
 */
DataBbox OmMipVolume::MipCoordToThreadLevelDataBbox(const OmMipChunkCoord & rMipCoord)
{
	int level_dim = GetThreadChunkLevelDimension(rMipCoord.Level);
	Vector3i level_dims = GetThreadChunkLevelDimensions(rMipCoord.Level);

	//min of extent in parent data coordinates
	DataCoord level_min_coord = rMipCoord.Coordinate * level_dim;

	return DataBbox(level_min_coord, level_dims.x, level_dims.y, level_dims.z);
}

/////////////////////////////////
///////          MipChunk Methods

/*
 *	Returns root mip coordinate based on root mip level.
 */
OmMipChunkCoord OmMipVolume::RootMipChunkCoordinate()
{
	return OmMipChunkCoord(mMipRootLevel, 0, 0, 0);
}

/*
 *	Returns true if given MipCoordinate is a valid coordinate within the MipVolume.
 */
bool OmMipVolume::ContainsMipChunkCoord(const OmMipChunkCoord & rMipCoord)
{

	//if level is greater than root level
	if (rMipCoord.Level > GetRootMipLevel())
		return false;

	//convert to data box in leaf
	DataBbox mip_coord_data_bbox = MipCoordToDataBbox(rMipCoord, 0);

	//insersect and check if not empty
	mip_coord_data_bbox.intersect(GetDataExtent());
	if (mip_coord_data_bbox.isEmpty())
		return false;

	//else valid mip coord
	return true;
}

/*
 *	Returns true if coordinate of thread chunk level is a valid coordinate within the MipVolume.
 */
bool OmMipVolume::ContainsThreadChunkCoord(const OmMipChunkCoord & rMipCoord)
{

	//if level is greater than root level
	if (rMipCoord.Level > GetRootMipLevel())
		return false;

	//check if coord is in volume
	DataCoord thread_coord_dims = MipLevelDimensionsInThreadChunks(0);
	if (rMipCoord.Coordinate.x > thread_coord_dims.x ||
	    rMipCoord.Coordinate.y > thread_coord_dims.y ||
	    rMipCoord.Coordinate.z > thread_coord_dims.z){
		return false;
	}

	//else valid mip coord
	return true;
}

/*
 *	Returns true if coordinate of thread chunk is a valid coordinate within the MipVolume.
 */
bool OmMipVolume::ContainsThreadChunkLevelCoord(const OmMipChunkCoord & rMipCoord)
{

	//if level is greater than root level
	if (rMipCoord.Level > GetRootMipLevel())
		return false;

	//check if coord is in volume
	DataCoord thread_coord_dims = MipLevelDimensionsInThreadChunkLevels(rMipCoord.Level);
	if (rMipCoord.Coordinate.x > thread_coord_dims.x ||
	    rMipCoord.Coordinate.y > thread_coord_dims.y ||
	    rMipCoord.Coordinate.z > thread_coord_dims.z){
		return false;
	}

	//else valid mip coord
	return true;
}

/*
 *	Finds set of children coordinates that are valid for this MipVolume.
 */
void OmMipVolume::ValidMipChunkCoordChildren(const OmMipChunkCoord & rMipCoord, set < OmMipChunkCoord > &children)
{
	//clear set
	children.clear();

	//get all possible children
	OmMipChunkCoord possible_children[8];
	rMipCoord.ChildrenCoords(possible_children);

	//for all possible children
	for (int i = 0; i < 8; i++) {
		if (ContainsMipChunkCoord(possible_children[i])) {
			children.insert(possible_children[i]);
		}
	}
}

/////////////////////////////////
///////          MipChunk Access

/*
 *	Returns a QExplicitlySharedDataPointer to an open OmMipChunk covering the specified coordinate.
 *
 *	The shared pointer allows the cache system to keep track of of the use of the chunk
 *	so it will not delete the chunk if another part of the system is using it.
 *
 *	NOTE: DO NOT DELETE the data volume returned as a shared pointer
 *			this is taken care of by the cache system.
 */
void OmMipVolume::GetChunk(OmMipChunkPtr& p_value, const OmMipChunkCoord & rMipCoord, bool block)
{
	//ensure either built or building
	assert(mBuildState != MIPVOL_UNBUILT);

	mDataCache->Get(p_value, rMipCoord, block);
}

void OmMipVolume::GetThreadChunkLevel(OmThreadChunkLevelPtr& p_value, const OmMipChunkCoord & rMipCoord, bool block)
{
	//ensure either built or building
	assert(mBuildState != MIPVOL_UNBUILT);

	mThreadChunkThreadedCache->Get(p_value, rMipCoord, block);
}

/////////////////////////////////
///////          Data Accessors

quint32 OmMipVolume::GetVoxelValue(const DataCoord & vox)
{
	if (!ContainsVoxel(vox))
		return 0;

	//find mip_coord and offset
	OmMipChunkCoord leaf_mip_coord = DataToMipCoord(vox, 0);

	OmMipChunkPtr p_chunk;
	GetChunk(p_chunk, leaf_mip_coord);

	//get voxel data
	return p_chunk->GetVoxelValue(vox);
}

void OmMipVolume::SetVoxelValue(const DataCoord & vox, uint32_t val)
{
	if (!ContainsVoxel(vox))
		return;

	//find mip_coord and offset
	OmMipChunkCoord leaf_mip_coord = DataToMipCoord(vox, 0);

	OmMipChunkPtr p_chunk;
	GetChunk(p_chunk, leaf_mip_coord);

	p_chunk->SetVoxelValue(vox, val);

	//note the chunk has been edited
	mEditedLeafChunks.insert(leaf_mip_coord);
}

/////////////////////////////////
///////         Mip Construction Methods

Vector3i OmMipVolume::getDimsRoundedToNearestChunk(const int level)
{
	const Vector3i data_dims = MipLevelDataDimensions(level);

	return Vector3i(ROUNDUP(data_dims.x, GetChunkDimension()),
			ROUNDUP(data_dims.y, GetChunkDimension()),
			ROUNDUP(data_dims.z, GetChunkDimension()));
}

void OmMipVolume::DeleteVolumeData()
{
	OmDataPath path(GetDirectoryPath());

	OmProjectData::DeleteInternalData(path);
}

/////////////////////////////////
///////          Building

/*
 *	Build all MipLevel resolutions of the MipVolume.
 */
void OmMipVolume::Build(OmDataPath & dataset)
{
	//unbuild
	SetBuildState(MIPVOL_BUILDING);

	//update properties
	UpdateMipProperties(dataset);

	//delete old
	DeleteVolumeData();

	//if source data valid
	if (!IsSourceValid()) {
		// printf("OmMipVolume::Build: blank build complete\n");
		SetBuildState(MIPVOL_BUILT);
		return;
	}

	//copy source data
	if (!ImportSourceData(dataset)) {
		DeleteVolumeData();
		SetBuildState(MIPVOL_UNBUILT);
		return;
	}

	//build volume
	if (!BuildVolume()) {
		DeleteVolumeData();
		SetBuildState(MIPVOL_UNBUILT);
		return;
	}

	mDataCache->Clear();

	//build complete
	SetBuildState(MIPVOL_BUILT);
}

/*
 *	Build all chunks in MipLevel of this MipVolume.
 */
bool OmMipVolume::BuildVolume()
{
	return BuildThreadedVolume();
}

/*
 *	Build all chunks in MipLevel of this MipVolume with multithreading
 */
bool OmMipVolume::BuildThreadedVolume()
{
	mThreadChunkThreadedCache = new OmThreadChunkThreadedCache(this);

	OmTimer vol_timer;

	if (isDebugCategoryEnabled("perftest")){
       		vol_timer.start();
	}

 	//Distribute and subsample thread chunks as much as possible before redistributing
 	for (int initLevel = 0; initLevel < GetRootMipLevel(); initLevel += GetMaxConsecutiveSubsamples()){

 		printf("Reading mip level %i, ",initLevel);
 		if ( (initLevel+1) == min(GetRootMipLevel(), initLevel + GetMaxConsecutiveSubsamples()) ){
 			printf("building mip level %i...\n",initLevel+1);
 		} else {
 			printf("building mip levels %i-%i...\n",
			       initLevel+1,
			       min(GetRootMipLevel(),
				   initLevel + GetMaxConsecutiveSubsamples()));
 		}

 		OmMipThreadManager* mipThreadManager =
			new OmMipThreadManager(this,
					       OmMipThread::THREAD_CHUNK,
					       initLevel,
					       false);
 		mipThreadManager->SpawnThreads(ThreadChunksInMipLevel(initLevel));
 		mipThreadManager->run();
 		mipThreadManager->wait();
 		mipThreadManager->StopThreads();
 		delete mipThreadManager;

 		//flush cache so that all thread chunks are flushed to disk
 		printf("Flushing thread chunks...\n");
 		mThreadChunkThreadedCache->Flush();

 	}

 	printf("done.\n");

	if (isDebugCategoryEnabled("perftest")){
		vol_timer.stop();
		printf("OmMipVolume:BuildThreadedVolume() done : %.6f secs\n",vol_timer.s_elapsed());
	}

	mThreadChunkThreadedCache->closeDownThreads();
	delete mThreadChunkThreadedCache;

	copyAllMipDataIntoMemMap();

	return true;
}

/*
 *	Build chunk data from children.
 *	rMipCoord: specifies the chunk to be built
 */
void OmMipVolume::BuildChunk(const OmMipChunkCoord & rMipCoord, bool)
{
	//leaf chunks are skipped since no children to build from
	if (rMipCoord.IsLeaf())
		return;

	//otherwise chunk is a parent, so get pointer to chunk
	OmMipChunkPtr p_chunk;
	GetChunk(p_chunk, rMipCoord);

	//read original data
	OmDataPath source_data_path(MipLevelInternalDataPath(rMipCoord.Level-1));
	DataBbox source_data_bbox = MipCoordToDataBbox(rMipCoord, rMipCoord.Level - 1);

	//read and get pointer to data
	OmDataWrapperPtr p_source_data =
		OmProjectData::GetProjectDataReader()->dataset_image_read_trim(source_data_path,
									source_data_bbox);

	//subsample
	OmDataWrapperPtr p_subsampled_data;

	p_subsampled_data = p_source_data->SubsampleData();

	//set or replace image data (chunk now owns pointer)
	p_chunk->SetImageData(p_subsampled_data);
	p_chunk->setVolDataDirty();
}

/*
 *	Builds a chunk and its ancestors of a chunk, propagating any changes of
 *	the chunk to parent chunks.
 */
void OmMipVolume::BuildChunkAndParents(const OmMipChunkCoord & rMipCoord)
{
	//build the chunk
	OmMipVolume::BuildChunk(rMipCoord);
	BuildChunk(rMipCoord, true);

	//if mipCoord is not root
	if (rMipCoord.Level != mMipRootLevel) {

		//get parent
		OmMipChunkCoord parent_coord = rMipCoord.ParentCoord();

		//build parent chunk and recurse
		BuildChunkAndParents(parent_coord);
	}
}

/*
 *	Rebuild all chunks in the edited chunk set.
 */
void OmMipVolume::BuildEditedLeafChunks()
{
	//first build all the edited leaf chunks

	FOR_EACH(itr, mEditedLeafChunks ){

		OmMipChunkPtr p_chunk;
		GetChunk(p_chunk, *itr);

		//rebuild ancestors
		BuildChunkAndParents(*itr);

		//clear modified values
		p_chunk->ClearModifiedVoxelValues();
	}

	//edited chunks clean
	mEditedLeafChunks.clear();
}

/*
 *	Build one level of a thread chunk and return image data for use by next level
 */
OmDataWrapperPtr OmMipVolume::BuildThreadChunkLevel(const OmMipChunkCoord & rMipCoord,
						    OmDataWrapperPtr p_source_data,
						    bool initCall)
{
	//read and return data if this is initial call
	if ( initCall ){

		//read original data
		OmDataPath source_data_path(MipLevelInternalDataPath(rMipCoord.Level));
		DataBbox source_data_bbox = MipCoordToThreadDataBbox(rMipCoord);

		OmDataWrapperPtr p_read_data =
			OmProjectData::GetProjectDataReader()->
			dataset_image_read_trim(source_data_path, source_data_bbox);

		return p_read_data;

	} else {

		//otherwise get pointer to thread chunk level
		OmThreadChunkLevelPtr p_chunklevel;
		GetThreadChunkLevel(p_chunklevel, rMipCoord);
		debug("mipthread","Thread chunk level coordinates: %s\n", qPrintable( rMipCoord.getCoordsAsString() ));
		debug("mipthread","Thread chunk level dimensions: %i,%i,%i\n", p_chunklevel->GetDimensions().x, p_chunklevel->GetDimensions().y, p_chunklevel->GetDimensions().z);

		OmDataWrapperPtr p_subsampled_data = p_source_data->SubsampleData();
		//set or replace image data (chunk level now owns pointer)
		p_chunklevel->SetImageData(p_subsampled_data);

		return p_subsampled_data;
	}
}

/*
 *	Recursively build thread chunk levels with minimal IO by passing image data from
 *	the previous level build to the next level build
 */
void OmMipVolume::BuildThreadChunk(const OmMipChunkCoord & rMipCoord, OmDataWrapperPtr data, bool initCall){

	//build level
	OmDataWrapperPtr p_image_data = BuildThreadChunkLevel(rMipCoord, data, initCall);

	//continue only if image is large enough to subsample
	//this assumes initial calls get image data which is large enough
	if ( GetThreadChunkLevelDimension(rMipCoord.Level) >= 2 || initCall ) {
		//if mipCoord is not root
		if ( rMipCoord.Level != mMipRootLevel ) {
			//get parent level
			OmMipChunkCoord parent_coord = rMipCoord;
			parent_coord.Level++;

			//build parent chunk level and recurse
			//not initial call
			BuildThreadChunk(parent_coord, p_image_data, false);
		}
	}
}

/////////////////////////////////
///////        Comparison

/*
 *	Returns true if two given volumes are exactly the same.
 *	Prints all positions where volumes differ if verbose flag is set.
 */
bool OmMipVolume::CompareVolumes(OmMipVolume *pMipVolume1, OmMipVolume *pMipVolume2, bool verbose)
{
	//check if dimensions are the same
	if (pMipVolume1->GetDataExtent().getUnitDimensions() !=
	    pMipVolume2->GetDataExtent().getUnitDimensions()){
		printf("Volumes differ: Different dimensions.\n");
		return false;
	}

	bool diff = false;

	//root mip level should be the same if data dimensions are the same
	if( pMipVolume1->GetRootMipLevel() != pMipVolume2->GetRootMipLevel() ){
		printf("Volumes differ: Different number of MIP levels.\n");
		return false;
	}

	//loop through levels
	for (int level = 0; level <= pMipVolume1->GetRootMipLevel(); ++level) {

		if (verbose){
			printf("Comparing mip level %i\n",level);
		}

		//dim of miplevel in mipchunks
		Vector3i mip_coord_dims =
			pMipVolume1->MipLevelDimensionsInMipChunks(level);

		//for all coords
		for (int z = 0; z < mip_coord_dims.z; ++z){
			for (int y = 0; y < mip_coord_dims.y; ++y){
				for (int x = 0; x < mip_coord_dims.x; ++x){

					if (verbose){
						printf("Comparing chunks at (%i,%i,%i,%i)\n",level,x,y,z);
					}

					//construct mip chunks
					OmMipChunk *pMipChunk1 = new OmMipChunk(OmMipChunkCoord(level, x, y, z),pMipVolume1);
					OmMipChunk *pMipChunk2 = new OmMipChunk(OmMipChunkCoord(level, x, y, z),pMipVolume2);

					if(!CompareChunks(pMipChunk1,pMipChunk2,verbose)){
						printf("Volumes differ: Chunks at (%i,%i,%i,%i) are different.\n",level,x,y,z);

						//delete chunks
						delete pMipChunk1;
						delete pMipChunk2;

						if (verbose){
							//set diff flag, don't return
							diff = true;
						} else {
							return false;
						}
					} else {
						//delete chunks
						delete pMipChunk1;
						delete pMipChunk2;
					}
				}
			}
		}
	}

	return !diff;
}

/*
 *	Returns true if two given chunks contain the exact same image data
 *	Returns true even if chunk position differs
 */
bool OmMipVolume::CompareChunks(OmMipChunk* chunk1, OmMipChunk* chunk2, bool)
{
	return chunk1->compare(chunk2);
}

/////////////////////////////////
///////          IO

/*
 *	Export leaf volume data to HDF5 format.  Calls ExportImageDataFilter so subclass can
 *	post-process the image data before it is written.
 */
void OmMipVolume::ExportInternalData(QString fileNameAndPath)
{
	debug("hdf5image", "OmMipVolume::ExportInternalData(%s)\n",
	      qPrintable(fileNameAndPath));

	//get leaf data extent
	DataBbox leaf_data_extent = GetDataExtent();

	//dim of leaf coords
	Vector3i leaf_mip_dims = MipLevelDimensionsInMipChunks(0);
	OmDataPath mip_volume_path(MipLevelInternalDataPath(0));

        OmHdf5 hdfExport( fileNameAndPath, false );
        OmDataPath fpath;
        fpath.setPath("main");

	if( !QFile::exists(fileNameAndPath) ){
        	hdfExport.create();
        	hdfExport.open();
		Vector3<int> full = MipLevelDataDimensions(0);
        	const Vector3i rounded_data_dims = getDimsRoundedToNearestChunk(0);
        	hdfExport.dataset_image_create_tree_overwrite(fpath,
							      rounded_data_dims,
							      full,
							      mVolDataType);
	} else {
        	hdfExport.open();
	}


	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {
				const OmMipChunkCoord coord(0, x, y, z);
				doExportChunk(coord, hdfExport);
			}
		}
	}
	hdfExport.close();
}

void OmMipVolume::doExportChunk(const OmMipChunkCoord & leaf_coord,
				OmHdf5 & )
{

	assert(0 && "FIXME");
	const DataBbox chunk_data_bbox = MipCoordToDataBbox(leaf_coord, 0);
	/*
	OmMipChunkPtr chunk;
	Get(chunk, leaf_coord);

	OmDataWrapperPtr chunkData = chunk->RawReadChunkDataHDF5();
	uint32_t* data =

	for(int i = 0; i < 128*128*128; ++i){

	// rewrite
	ExportDataFilter(p_chunk_img_data);

// FIXME: this needs to get refactored into some sort of helper...
void OmSegmentCache::ExportDataFilter(vtkImageData * pImageData)
{
	QMutexLocker locker( &mMutex );

	//get data extent (varify it is a chunk)
	int extent[6];
	pImageData->GetExtent(extent);

	//get pointer to native scalar data
	assert(pImageData->GetScalarSize() == SEGMENT_DATA_BYTES_PER_SAMPLE);
	OmSegID * p_scalar_data = static_cast<OmSegID*>( pImageData->GetScalarPointer() );

	//for all voxels in the chunk
	int x, y, z;
	for (z = extent[0]; z <= extent[1]; z++) {
		for (y = extent[2]; y <= extent[3]; y++) {
			for (x = extent[4]; x <= extent[5]; x++) {

				//if non-null segment value
				if (NULL_SEGMENT_VALUE != *p_scalar_data) {
					*p_scalar_data = mImpl->findRootID( *p_scalar_data );
				}
				//adv to next scalar
				++p_scalar_data;
			}
		}
	}
}

	//write to hdf5 file
	//debug("FIXME", "OmMipVolume::Export:" chunk_data_bbox  endl;

	hdfExport.dataset_image_write_trim(OmDataPaths::getDefaultDatasetName(),
					   (DataBbox*)&chunk_data_bbox,
					   p_chunk_img_data);
	*/
}

bool OmMipVolume::ContainsVoxel(const DataCoord & vox)
{
	return GetDataExtent().contains(vox);
}

OmThreadChunkThreadedCache* OmMipVolume::GetThreadChunkThreadedCache()
{
	return mThreadChunkThreadedCache;
}

Vector3i OmMipVolume::get_dims(const OmDataPath dataset )
{
	if(areImportFilesImages()){
		QImage img(mSourceFilenamesAndPaths[0].absoluteFilePath());
		Vector3i dims(img.width(), img.height(), mSourceFilenamesAndPaths.size());
		printf("dims are %dx%dx%d\n", DEBUGV3(dims));
		return dims;
	}

	return OmImageDataIo::om_imagedata_get_dims_hdf5(mSourceFilenamesAndPaths, dataset);
}

void OmMipVolume::copyDataIn()
{
	Vector3i mip_coord_dims = MipLevelDimensionsInMipChunks(0);

	const int total = mip_coord_dims.z*mip_coord_dims.y*mip_coord_dims.x;
	int counter = 0;

	for (int z = 0; z < mip_coord_dims.z; ++z){
		for (int y = 0; y < mip_coord_dims.y; ++y){
			for (int x = 0; x < mip_coord_dims.x; ++x){

				OmMipChunkCoord coord(0, x, y, z);
				OmMipChunkPtr chunk;
				GetChunk(chunk, coord);
				chunk->copyChunkFromMemMapToHDF5();

				++counter;
				printf("\rwrote chunk %dx%dx%d to HDF5 (%d of %d total)",
				       x, y, z, counter, total);
				fflush(stdout);
			}
		}
	}
	printf("\n");
}

//FIXME: move into OmChannel/OmSegmentation so we don't assume default type
void OmMipVolume::BuildBlankVolume(const Vector3i & dims)
{
	SetBuildState(MIPVOL_BUILDING);

	OmVolume::SetDataDimensions(dims);
	UpdateRootLevel();

	DeleteVolumeData();
	assert(0);
	//AllocInternalData(OM_UINT32); // FIXME: don't assume default type

	SetBuildState(MIPVOL_BUILT);
}

bool OmMipVolume::areImportFilesImages()
{
	if( mSourceFilenamesAndPaths[0].fileName().endsWith(".h5", Qt::CaseInsensitive) ||
	    mSourceFilenamesAndPaths[0].fileName().endsWith(".hdf5", Qt::CaseInsensitive)){
		return false;
	}

	return true;
}

void OmMipVolume::copyAllMipDataIntoMemMap()
{
	Flush();

	for (int level = 0; level <= GetRootMipLevel(); ++level) {

		Vector3i mip_coord_dims =
			MipLevelDimensionsInMipChunks(level);

		for (int z = 0; z < mip_coord_dims.z; ++z){
			for (int y = 0; y < mip_coord_dims.y; ++y){
				for (int x = 0; x < mip_coord_dims.x; ++x){

					OmMipChunkCoord coord(level, x, y, z);

					OmMipChunkPtr chunk;
					GetChunk(chunk, coord);
					chunk->copyDataFromHDF5toMemMap();
				}
			}
		}
	}
}
