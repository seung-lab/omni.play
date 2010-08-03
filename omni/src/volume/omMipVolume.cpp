#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "project/omProject.h"
#include "volume/omLoadImageThread.h"
#include "utility/stringHelpers.h"
#include "common/omDebug.h"
#include "common/omException.h"
#include "common/omStd.h"
#include "common/omVtk.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
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
#include <vtkExtractVOI.h>
#include <vtkImageConstantPad.h>

#include <QFile>
#include <QImage>
#include <QThreadPool>

//Maximum number of powers of 2 that thread chunk is linearly larger than a chunk
#define MAX_THREAD_CHUNK_EXPONENT 2

//TODO: Get BuildThreadedVolume() to display progress somehow using OmMipThread::GetThreadChunksDone()

static const char *MIP_VOLUME_FILENAME = "volume.dat";
static const QString MIP_CHUNK_META_DATA_FILE_NAME = "metachunk.dat";

/////////////////////////////////
///////
///////         OmMipVolume
///////

OmMipVolume::OmMipVolume()
	: mDataCache(new OmMipVolumeCache(this))
{
	sourceFilesWereSet = false;

	//init
	SetFilename(MIP_VOLUME_FILENAME);
	SetDirectoryPath("default/");
	SetBuildState(MIPVOL_UNBUILT);
	SetChunksStoreMetaData(false);

	mBytesPerSample = 1;
}

OmMipVolume::~OmMipVolume()
{
	Flush();
	delete mDataCache;

	foreach(QFile * f, mFileVec){
		delete f;
		f = NULL;
	}
}

/////////////////////////////////
///////          DataVolume

/*
 *	Flush all changes to disk
 */
void OmMipVolume::Flush()
{
	//build any edited leaf chunks
	BuildEditedLeafChunks();

	//flush all chunks in the cache
	mDataCache->Flush();
}

/////////////////////////////////
///////          Internal Data Properties

void OmMipVolume::SetFilename(const QString & fname)
{
	mFilename = fname;
}

QString OmMipVolume::GetFilename()
{
	return mFilename;
}

QString OmMipVolume::GetDirectoryPath()
{
	return mDirectoryPath;
}

void OmMipVolume::SetDirectoryPath(const QString & dpath)
{
	mDirectoryPath = dpath;
}

/*
 *	Returns data path to internal MipLevel data.
 */
QString OmMipVolume::MipLevelInternalDataPath(int level)
{
	return QString("%1%2/%3")
		.arg(mDirectoryPath)
		.arg(level)
		.arg( mFilename );
}

/*
 *	Returns path to MetaData of specified chunk
 */
QString OmMipVolume::MipChunkMetaDataPath(const OmMipChunkCoord & rMipCoord)
{
	//assert this mip volume has chunk metadata
	assert(GetChunksStoreMetaData());

	QString p = QString("%1/%2_%3_%4/")
		.arg(rMipCoord.Level)
		.arg(rMipCoord.Coordinate.x)
		.arg(rMipCoord.Coordinate.y)
		.arg(rMipCoord.Coordinate.z);

	return GetDirectoryPath() + p + MIP_CHUNK_META_DATA_FILE_NAME;
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

Vector3 < int > OmMipVolume::GetChunkDimensions()
{
	return Vector3 < int >(GetChunkDimension(), GetChunkDimension(), GetChunkDimension());
}

void OmMipVolume::SetChunksStoreMetaData(bool state)
{
	mStoreChunkMetaData = state;
}

bool OmMipVolume::GetChunksStoreMetaData()
{
	return mStoreChunkMetaData;
}

bool OmMipVolume::IsBuilt()
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
		Vector3 < int >source_dims = get_dims(dataset);

		debug("hdf5image", "%i:%i:%i, from %s and %s\n", DEBUGV3(source_dims));

		//if dim differs from OmVolume alert user
		if (OmVolume::GetDataDimensions() != source_dims) {
			//			printf("OmMipVolume::UpdateMipProperties: CHANGING VOLUME DIMENSIONS\n");

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
	Vector3 < int >source_dims = GetDataExtent().getUnitDimensions();
	int max_source_dim = source_dims.getMaxComponent();
	int mipchunk_dim = GetChunkDimension();

	if (max_source_dim <= mipchunk_dim) {
		mMipRootLevel = 0;
	} else {
                mMipRootLevel = ceil(log((float) (max_source_dim) / GetChunkDimension()) / log((float)2));
#if 0
		//use log base 2 to determine levels needed to contain source dims
		double numPowsOfTwo = log((float) (max_source_dim) / GetChunkDimension()) / log((float)2);
		//round to closest integer
		mMipRootLevel = (int) floor(numPowsOfTwo);
		if (numPowsOfTwo-mMipRootLevel >= 0.5){mMipRootLevel++;}
#endif
	}
}

/*
 *	Calculate the data dimensions needed to contain the volume at a given compression level.
 */
Vector3 < int > OmMipVolume::MipLevelDataDimensions(int level)
{
	//get dimensions
	DataBbox source_extent = GetDataExtent();
	Vector3 < float >source_dims = source_extent.getUnitDimensions();

	//dims in fraction of pixels
	Vector3 < float >mip_level_dims = source_dims / OMPOW(2, level);

	return Vector3 < int >(ceil(mip_level_dims.x),
			       ceil(mip_level_dims.y),
			       ceil(mip_level_dims.z));
}

// TODO: this appear to NOT do the right thing for level=0 (purcaro)

/*
 *	Calculate the MipChunkCoord dims required to contain all the chunks of a given level.
 */
Vector3 < int > OmMipVolume::MipLevelDimensionsInMipChunks(int level)
{
	Vector3 < float >data_dims = MipLevelDataDimensions(level);
	return Vector3 < int >(ceil(data_dims.x / GetChunkDimension()),
			       ceil(data_dims.y / GetChunkDimension()),
			       ceil(data_dims.z / GetChunkDimension()));
}

/*
 *	Returns the number of MipChunks in a given MipLevel of the MipVolume
 */
int OmMipVolume::MipChunksInMipLevel(int level)
{
	Vector3 < int >mip_dims = MipLevelDimensionsInMipChunks(level);
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
	int unbounded_chunk_dim = OMPOW(2, mMipRootLevel);
	int lower_bound = GetChunkDimension();
	int upper_bound = GetChunkDimension()*OMPOW(2, MAX_THREAD_CHUNK_EXPONENT);
	int bounded_chunk_dim = min(max(unbounded_chunk_dim,lower_bound),upper_bound);
	return bounded_chunk_dim;
}

Vector3 < int > OmMipVolume::GetThreadChunkDimensions()
{
	return Vector3 < int >(GetThreadChunkDimension(), GetThreadChunkDimension(), GetThreadChunkDimension());
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

Vector3 < int > OmMipVolume::GetThreadChunkLevelDimensions(int level)
{
	return Vector3 < int >(GetThreadChunkLevelDimension(level), GetThreadChunkLevelDimension(level), GetThreadChunkLevelDimension(level));
}

/*
 *	Calculate the MipChunkCoord dims required to contain all the thread chunks in a mip level.
 */
Vector3 < int > OmMipVolume::MipLevelDimensionsInThreadChunks(int level)
{
	Vector3 < float >data_dims = MipLevelDataDimensions(level);

	return Vector3 < int >(ceil(data_dims.x / GetThreadChunkDimension()),
			       ceil(data_dims.y / GetThreadChunkDimension()), 
			       ceil(data_dims.z / GetThreadChunkDimension()));
}

/*
 *	Calculate the MipChunkCoord dims required to contain all the thread chunks levels of that level in a mip level.
 */
Vector3 < int > OmMipVolume::MipLevelDimensionsInThreadChunkLevels(int level)
{
	Vector3 < float >data_dims = MipLevelDataDimensions(level);

	return Vector3 < int >(ceil(data_dims.x / GetThreadChunkLevelDimension(level)),
			       ceil(data_dims.y / GetThreadChunkLevelDimension(level)), 
			       ceil(data_dims.z / GetThreadChunkLevelDimension(level)));
}

/*
 *	Returns the number of ThreadChunks in a given MipLevel of the MipVolume.
 */
int OmMipVolume::ThreadChunksInMipLevel(int level)
{
	Vector3 < int >thread_dims = MipLevelDimensionsInThreadChunks(level);
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
	Vector3 < int >leaf_dims = GetChunkDimensions() * old_level_factor;

	//min of extent in leaf data coordinates
	DataCoord leaf_min_coord = rMipCoord.Coordinate * leaf_dim;

	//convert to new level
	DataCoord new_extent_min_coord = leaf_min_coord / new_level_factor;
	//DataCoord new_extent_max_coord = (leaf_min_coord + leaf_dims) / new_level_factor;
	Vector3 < int >new_dims = leaf_dims / new_level_factor;

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
	Vector3 < int >thread_dims = GetThreadChunkDimensions();

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
	Vector3 < int >level_dims = GetThreadChunkLevelDimensions(rMipCoord.Level);

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
	//assert valid
	//assert(OmDataVolume::ContainsVoxel(vox));
	if (!ContainsVoxel(vox))
		return 0;

	//find mip_coord and offset
	OmMipChunkCoord leaf_mip_coord = DataToMipCoord(vox, 0);

	//get chunk
	OmMipChunkPtr p_chunk;
	GetChunk(p_chunk, leaf_mip_coord);

	//get voxel data
	return p_chunk->GetVoxelValue(vox);
}

void OmMipVolume::SetVoxelValue(const DataCoord & vox, uint32_t val)
{
	//assert valid
	//assert(OmDataVolume::ContainsVoxel(vox));
	if (!ContainsVoxel(vox))
		return;
	//find mip_coord and offset
	OmMipChunkCoord leaf_mip_coord = DataToMipCoord(vox, 0);
	//get chunk
	OmMipChunkPtr  p_chunk;
	GetChunk(p_chunk, leaf_mip_coord);
	//get voxel data
	p_chunk->SetVoxelValue(vox, val);
	//note the chunk has been edited
	mEditedLeafChunks.insert(leaf_mip_coord);

	// //debug("FIXME", "OmMipVolume::SetVoxelValue done";
}

/////////////////////////////////
///////         Mip Construction Methods

/*
 *	Allocate the image data for all mip level volumes.
 *	Note: root level, leaf dim, bytes per sample must already be set
 */
void OmMipVolume::AllocInternalData(OmDataWrapperPtr data)
{
	Vector3<int> chunkdims = GetChunkDimensions();
	//for all levels, alloc image data
	for (int i = 0; i <= GetRootMipLevel(); i++) {
		//debug("genone","OmMipVolume::AllocInternalData()\n");

		//get dim of mip level volume
		Vector3 < int >data_dims = MipLevelDataDimensions(i);
		//round up to nearest chunk
		Vector3 < int >rounded_data_dims = Vector3 < int >(ROUNDUP(data_dims.x, GetChunkDimension()),
								   ROUNDUP(data_dims.y, GetChunkDimension()),
								   ROUNDUP(data_dims.z, GetChunkDimension()));

		//alloc image data

		OmDataPath mip_volume_level_path;
		mip_volume_level_path.setPathQstr( MipLevelInternalDataPath(i) );;

		//debug("genone","OmMipVolume::AllocInternalData: %s \n", mip_volume_level_path.data());
		OmProjectData::GetDataWriter()->
			dataset_image_create_tree_overwrite(mip_volume_level_path,
							    &rounded_data_dims,
							    &chunkdims,
							    data->getHdf5FileType());
	}

	mBytesPerSample = data->getSizeof();
}

void OmMipVolume::DeleteVolumeData()
{
	OmDataPath path;
	path.setPathQstr( mDirectoryPath );

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

	//build complete
	SetBuildState(MIPVOL_BUILT);
}

/*
 *	Build all chunks in MipLevel of this MipVolume.
 *	For channel builds, simply use multithreading.
 */
bool OmMipVolume::BuildVolume()
{
	return BuildThreadedVolume();
}

/*
 *	Old way of building volume on a single thread, still used by OmSegmentation.
 */
bool OmMipVolume::BuildSerialVolume()
{

	//debug("FIXME", "OmMipVolume::BuildVolume()"
	//init progress bar
	int prog_count = 0;
	OmEventManager::
	    PostEvent(new
		      OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, string("Building volume...               "), 0,
				      MipChunksInVolume()));
	OmTimer vol_timer;

	if (isDebugCategoryEnabled("perftest")){
		//timer start
		vol_timer.start();
	}

	//for each level
	for (int level = 0; level <= GetRootMipLevel(); ++level) {
		printf("\tbuilding mip level %d...", level );
		fflush(stdout);

		//dim of miplevel in mipchunks
		Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

		//for all coords
		for (int z = 0; z < mip_coord_dims.z; ++z)
			for (int y = 0; y < mip_coord_dims.y; ++y)
				for (int x = 0; x < mip_coord_dims.x; ++x) {

					//build chunk
					BuildChunk(OmMipChunkCoord(level, x, y, z));

					//check for cancel
					if (OmProgressEvent::GetWasCanceled())
						return false;

					//update progress
					OmEventManager::
					    PostEvent(new
						      OmProgressEvent(OmProgressEvent::PROGRESS_VALUE, ++prog_count));

				}

		//flush cache so that all chunks of this level are flushed to disk
		Flush();
		printf("done\n");
	}

	if (isDebugCategoryEnabled("perftest")){
		//timer end
		vol_timer.stop();
		printf("OmMipVolume:BuildSerialVolume() done : %.6f secs\n",vol_timer.s_elapsed());
	}

	//hide progress bar
	OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_HIDE));
	return true;
}

/*
 *	Build all chunks in MipLevel of this MipVolume with multithreading
 */
bool OmMipVolume::BuildThreadedVolume()
{

	//debug("FIXME", "OmMipVolume::BuildThreadedVolume()"
	//init progress bar, needs fixing
	OmEventManager::
	    PostEvent(new
		      OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, string("Building volume...               "), 0,
				      ThreadChunksInVolume()));

	mThreadChunkThreadedCache = new OmThreadChunkThreadedCache(this);

	OmTimer vol_timer;

	if (isDebugCategoryEnabled("perftest")){
       		//timer start
       		vol_timer.start();
	}

 	//Distribute and subsample thread chunks as much as possible before redistributing
 	for (int initLevel = 0; initLevel < GetRootMipLevel(); initLevel += GetMaxConsecutiveSubsamples()){
 
 		printf("Reading mip level %i, ",initLevel);
 		if ( (initLevel+1) == min(GetRootMipLevel(), initLevel + GetMaxConsecutiveSubsamples()) ){
 			printf("building mip level %i...\n",initLevel+1);
 		} else {
 			printf("building mip levels %i-%i...\n",initLevel+1,min(GetRootMipLevel(), initLevel + GetMaxConsecutiveSubsamples()));
 		}
 
 		OmMipThreadManager *mipThreadManager = new OmMipThreadManager(this,OmMipThread::THREAD_CHUNK,initLevel,false);
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

		//timer end
		vol_timer.stop();
		printf("OmMipVolume:BuildThreadedVolume() done : %.6f secs\n",vol_timer.s_elapsed());

	}

	mThreadChunkThreadedCache->closeDownThreads();
	delete mThreadChunkThreadedCache;

	//hide progress bar
	OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_HIDE));
	return true;
}

/*
 *	Build chunk data from children.
 *	rMipCoord: specifies the chunk to be built
 */
void OmMipVolume::BuildChunk(const OmMipChunkCoord & rMipCoord)
{

	//debug("genone","OmMipVolume::BuildChunk()\n");

	//leaf chunks are skipped since no children to build from
	if (rMipCoord.IsLeaf())
		return;

	//otherwise chunk is a parent, so get pointer to chunk
	OmMipChunkPtr p_chunk;
	GetChunk(p_chunk, rMipCoord);

	//read original data
	OmDataPath source_data_path;
	source_data_path.setPathQstr( MipLevelInternalDataPath(rMipCoord.Level - 1) );
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
}

/*
 *	Builds a chunk and its ancestors of a chunk, propagating any changes of
 *	the chunk to parent chunks.
 */
void OmMipVolume::BuildChunkAndParents(const OmMipChunkCoord & rMipCoord)
{

	//build the chunk
	BuildChunk(rMipCoord);

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
	set < OmMipChunkCoord >::iterator itr;
	for (itr = mEditedLeafChunks.begin(); itr != mEditedLeafChunks.end(); itr++) {
		//get pointer to chunk
		OmMipChunkPtr p_chunk;
		GetChunk(p_chunk, *itr);

		//rebuild ancestors
		BuildChunkAndParents(*itr);

		//clear modified values
		if(p_chunk) p_chunk->ClearModifiedVoxelValues();
	}

	//edited chunks clean
	mEditedLeafChunks.clear();

}

/*
 *	Build one level of a thread chunk and return image data for use by next level
 */
OmDataWrapperPtr OmMipVolume::BuildThreadChunkLevel(const OmMipChunkCoord & rMipCoord, OmDataWrapperPtr p_source_data, bool initCall)
{
	//read and return data if this is initial call
	if ( initCall ){

		//read original data
		OmDataPath source_data_path;
		source_data_path.setPathQstr( MipLevelInternalDataPath(rMipCoord.Level) );
		DataBbox source_data_bbox = MipCoordToThreadDataBbox(rMipCoord);

		OmDataWrapperPtr p_read_data =
			OmProjectData::GetProjectDataReader()->dataset_image_read_trim(source_data_path, source_data_bbox);

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
	if (pMipVolume1->GetDataExtent().getUnitDimensions() != pMipVolume2->GetDataExtent().getUnitDimensions()){
		printf("Volumes differ: Different dimensions.\n");
		return false;
	}

	bool diff = false;

	//root mip level should be the same if data dimensions are the same
	assert(pMipVolume1->GetRootMipLevel() == pMipVolume2->GetRootMipLevel());

	//loop through levels
	for (int level = 0; level <= pMipVolume1->GetRootMipLevel(); ++level) {

		if (verbose){
			printf("Comparing mip level %i\n",level);
		}

		//dim of miplevel in mipchunks
		Vector3 < int > mip_coord_dims = pMipVolume1->MipLevelDimensionsInMipChunks(level);

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

	if (diff){ return false; } else { return true; }

}

/*
 *	Returns true if two given chunks contain the exact same image data
 *	Returns true even if chunk position differs
 */
bool OmMipVolume::CompareChunks(OmMipChunk *pMipChunk1, OmMipChunk *pMipChunk2, bool verbose)
{
	//check if mip level is the same
	if (pMipChunk1->GetCoordinate().Level != pMipChunk2->GetCoordinate().Level){
		printf("Chunks differ: Different mip levels.\n");
		return false;
	}

	//check if dimensions are the same
	if (pMipChunk1->GetDimensions() != pMipChunk2->GetDimensions()){
		printf("Chunks differ: Different dimensions.\n");
		return false;
	}

	//uses mpImageData so ensure chunks are open
	pMipChunk1->Open();
	pMipChunk2->Open();

	//get extents
	int extent1[6];
	int extent2[6];
	pMipChunk1->GetImageData()->GetExtent(extent1);
	pMipChunk2->GetImageData()->GetExtent(extent2);

	//check if scalar size is the same
	int scalarSize1 = pMipChunk1->GetImageData()->GetScalarSize();
	int scalarSize2 = pMipChunk2->GetImageData()->GetScalarSize();

	//Set flag if chunks differ
	bool diff = false;

	if (scalarSize1 != scalarSize2){
		printf("Chunks differ: Different scalar types.\n");
		return false;
	} else if (scalarSize1 == 4){
		quint32 *p_scalar_data1 = static_cast < quint32* >(pMipChunk1->GetImageData()->GetScalarPointer());
		quint32 *p_scalar_data2 = static_cast < quint32* >(pMipChunk2->GetImageData()->GetScalarPointer());

		//check if each voxel is the same as the corresponding voxel in the other chunk
		//if verbose, loop through and print positions of all differing voxels
		for (int z1 = extent1[0], z2 = extent2[0]; (z1 <= extent1[1]) && (z2 <= extent2[1]); z1++, z2++){
			for (int y1 = extent1[2], y2 = extent2[2]; (y1 <= extent1[3]) && (y2 <= extent2[3]); y1++, y2++){
				for (int x1 = extent1[4], x2 = extent2[4]; (x1 <= extent1[5]) && (x2 <= extent2[5]); x1++, x2++){

					if (*p_scalar_data1 != *p_scalar_data2){
						printf("Voxel (%i,%i,%i) in Chunk 1 differs from Voxel (%i,%i,%i) in Chunk 2.\n",x1,y1,z1,x2,y2,z2);
						if (verbose){
							//set diff flag, don't return
							diff = true;
						} else {
							return false;
						}
					}

					//advance to next scalars
					++p_scalar_data1;
					++p_scalar_data2;

				}
			}
		}
	} else if (scalarSize1 == 1){
		quint8 *p_scalar_data1 = static_cast < quint8* >(pMipChunk1->GetImageData()->GetScalarPointer());
		quint8 *p_scalar_data2 = static_cast < quint8* >(pMipChunk2->GetImageData()->GetScalarPointer());

		//check if each voxel is the same as the corresponding voxel in the other chunk
		//if verbose, loop through and print positions of all differing voxels
		for (int z1 = extent1[0], z2 = extent2[0]; (z1 <= extent1[1]) && (z2 <= extent2[1]); z1++, z2++){
			for (int y1 = extent1[2], y2 = extent2[2]; (y1 <= extent1[3]) && (y2 <= extent2[3]); y1++, y2++){
				for (int x1 = extent1[4], x2 = extent2[4]; (x1 <= extent1[5]) && (x2 <= extent2[5]); x1++, x2++){

					if (*p_scalar_data1 != *p_scalar_data2){
						printf("Voxel (%i,%i,%i) in Chunk 1 differs from Voxel (%i,%i,%i) in Chunk 2.\n",x1,y1,z1,x2,y2,z2);
						if (verbose){
							//set diff flag, don't return
							diff = true;
						} else {
							return false;
						}
					}

					//advance to next scalars
					++p_scalar_data1;
					++p_scalar_data2;

				}
			}
		}
	}

	if (diff){ return false; } else { return true; }
}

/////////////////////////////////
///////          IO

/*
 *	Export leaf volume data to HDF5 format.  Calls ExportImageDataFilter so subclass can
 *	post-process the image data before it is written.
 */
void OmMipVolume::ExportInternalData(QString fileNameAndPath)
{
	debug("hdf5image", "OmMipVolume::ExportInternalData(%s)\n", qPrintable(fileNameAndPath));

	//get leaf data extent
	DataBbox leaf_data_extent = GetDataExtent();

	//dim of leaf coords
	Vector3 < int >leaf_mip_dims = MipLevelDimensionsInMipChunks(0);
	OmDataPath mip_volume_path;
	mip_volume_path.setPathQstr( MipLevelInternalDataPath(0) );
        //round up to nearest chunk

        OmHdf5 hdfExport( fileNameAndPath, false );
        OmDataPath fpath;
        fpath.setPath("main");

	if( !QFile::exists(fileNameAndPath) ){
        	hdfExport.create();
        	hdfExport.open();
		Vector3<int> full = MipLevelDataDimensions(0);
        	Vector3<int>rounded_data_dims = Vector3 <int>(ROUNDUP(full.x, GetChunkDimension()),
							      ROUNDUP(full.y, GetChunkDimension()),
							      ROUNDUP(full.z, GetChunkDimension()));
        	hdfExport.dataset_image_create_tree_overwrite(fpath, &rounded_data_dims, &full, GetBytesPerSample());
	} else {
        	hdfExport.open();
	}


	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {

				//form mip chunk coord
				OmMipChunkCoord leaf_coord(0, x, y, z);

				//get chunk data bbox
				DataBbox chunk_data_bbox = MipCoordToDataBbox(leaf_coord, 0);

				//read from project data
				OmDataWrapperPtr p_chunk_img_data =
					OmProjectData::GetProjectDataReader()->dataset_image_read_trim(mip_volume_path,
												chunk_data_bbox);

				//apply export filter
				ExportDataFilter(p_chunk_img_data);

				//write to hdf5 file
				//debug("FIXME", "OmMipVolume::Export:" chunk_data_bbox  endl;

				hdfExport.dataset_image_write_trim(OmDataPaths::getDefaultDatasetName(),
                                           (DataBbox*)&chunk_data_bbox, p_chunk_img_data);

			}
		}
	}
	hdfExport.close();
}

bool OmMipVolume::ContainsVoxel(const DataCoord & vox)
{
	return GetDataExtent().contains(vox);
}

int OmMipVolume::GetBytesPerSample()
{
	return mBytesPerSample;
}

void OmMipVolume::SetBytesPerSample(int bytesPerSample)
{
	mBytesPerSample = bytesPerSample;
}

OmThreadChunkThreadedCache* OmMipVolume::GetThreadChunkThreadedCache()
{
	return mThreadChunkThreadedCache;
}

bool OmMipVolume::areImportFilesImages()
{
	if( mSourceFilenamesAndPaths[0].fileName().endsWith(".h5", Qt::CaseInsensitive) ||
	    mSourceFilenamesAndPaths[0].fileName().endsWith(".hdf5", Qt::CaseInsensitive)){
		return false;
	}

	return true;
}

/*
 *	Given a valid source data volume, thes copies entire source volume to
 *	the leaf mip level of this mip volume.
 */
bool OmMipVolume::ImportSourceData(OmDataPath & dataset)
{
	// use VTK for HDF5 import...
	if(!areImportFilesImages()){
		return ImportSourceDataVTK(dataset);
	}

	return ImportSourceDataQT();
}

bool OmMipVolume::ImportSourceDataVTK(OmDataPath & dataset)
{
	//dim of leaf coords
	const Vector3i leaf_mip_dims = MipLevelDimensionsInMipChunks(0);

	OmDataPath leaf_volume_path;
	leaf_volume_path.setPathQstr( MipLevelInternalDataPath(0) );

	printf("\timporting data...\n");
	fflush(stdout);

	OmTimer import_timer;
	import_timer.start();

	bool initialized = false;

	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {

				//get chunk data bbox
				const OmMipChunkCoord chunk_coord = OmMipChunkCoord(0, x, y, z);
				DataBbox chunk_data_bbox = MipCoordToDataBbox(chunk_coord, 0);

				//read chunk image data from source
				OmDataWrapperPtr p_img_data =
					OmImageDataIo::om_imagedata_read_hdf5(mSourceFilenamesAndPaths,
									      chunk_data_bbox,
									      dataset);
				if(!initialized) {
					initialized = true;
					AllocInternalData(p_img_data);
				}

				//write to project data
				OmProjectData::GetDataWriter()->dataset_image_write_trim(leaf_volume_path,
											 &chunk_data_bbox,
											 p_img_data);
			}
		}
	}

	import_timer.stop();

	printf("done in %.6f secs\n",import_timer.s_elapsed());
	return true;
}

void OmMipVolume::figureOutNumberOfBytesImg()
{
	const int depth = QImage(mSourceFilenamesAndPaths[0].absoluteFilePath()).depth();

	int numberOfBytes = 1;
	if(32 == depth){
		numberOfBytes=4;
	}
	SetBytesPerSample(numberOfBytes);
}

bool OmMipVolume::ImportSourceDataQT()
{
	printf("\timporting data...\n");
	fflush(stdout);

	//timer start
	OmTimer import_timer;
	import_timer.start();

	figureOutNumberOfBytesImg();

	// alloc must happen after setBytesPerSample....
	AllocMemMapFiles();

	mSliceNum = 0;

	const int maxThreads=1;
	QThreadPool threads;
	threads.setMaxThreadCount(maxThreads);
	for(int i=0; i<maxThreads; ++i){
		OmLoadImageThread* t = new OmLoadImageThread(this);
		threads.start(t);
	}
	threads.waitForDone();

	printf("\ndone with image import; copying to HDF5 file...\n");

	// silly way to allocate internal data
	foreach(const OmMipChunkCoord & c, chunksToCopy){
		OmMipChunkPtr chunk;
		GetChunk(chunk, c);
		OmDataWrapperPtr dataPtrMapped = chunk->RawReadChunkDataUCHARmapped();
		AllocInternalData(dataPtrMapped);
		break;
	}

	copyDataIn(chunksToCopy);

	import_timer.stop();

	printf("done in %.2f secs\n",import_timer.s_elapsed());
	return true;
}

std::pair<int,QString> OmMipVolume::getNextImgToProcess()
{
	QMutexLocker lock(&mChunkCoords);

	if(mSourceFilenamesAndPaths.size() == mSliceNum){
		return std::pair<int,QString>(-1,"");
	}

	const QString ret = mSourceFilenamesAndPaths[mSliceNum].absoluteFilePath();

	return std::pair<int,QString>(mSliceNum++, ret);
}

void OmMipVolume::addToChunkCoords(const OmMipChunkCoord chunk_coord)
{
	QMutexLocker lock(&mChunkCoords);
	chunksToCopy.insert(chunk_coord);
}

void OmMipVolume::AllocMemMapFiles()
{
	QMutexLocker lock(&mChunkCoords);

	mFileVec.resize(GetRootMipLevel()+1);
	mFileMapPtr.resize(GetRootMipLevel()+1);

	for (int level = 0; level <= GetRootMipLevel(); level++) {

		Vector3 < int >data_dims = MipLevelDataDimensions(level);

		//round up to nearest chunk
		Vector3i rdims =
			Vector3i(ROUNDUP(data_dims.x, GetChunkDimension()),
				 ROUNDUP(data_dims.y, GetChunkDimension()),
				 ROUNDUP(data_dims.z, GetChunkDimension()));

		const qint64 size = (qint64)rdims.x
			*(qint64)rdims.y
			*(qint64)rdims.z
			*(qint64)GetBytesPerSample();

		assert(size);

		printf("mip %d: size is: %s (%dx%dx%d)\n",
		       level, qPrintable(StringHelpers::commaDeliminateNumber(size)),
		       rdims.x, rdims.y, rdims.z);

		const QString fn=QString("%1_%2_mip%3_%4bit.raw")
			.arg(OmProject::GetFileName().replace(".omni",""))
			.arg(GetDirectoryPath().replace("channels/","").replace("segmentations/","").replace("/",""))
			.arg(level)
			.arg(8*GetBytesPerSample());

		const QString fnp = OmProjectData::getAbsolutePath()+"/"+fn;
		QFile* file = mFileVec[level] = new QFile(fnp);
		file->remove();
		if(!file->open(QIODevice::ReadWrite)){
			printf("could not create chunk file %s\n", qPrintable(fnp));
			assert(0);
		}
		file->resize(size);
		/*
		printf("\tpre-allocating...\n");
		for( qint64 i=0; i < size; i+=(qint64)4096){
			file->seek(i);
			file->putChar(0);
		}
		printf("\tflushing...\n");
		file->flush();
		*/
		mFileMapPtr[level] = file->map(0,size);
		file->close();
	}
}

unsigned char * OmMipVolume::getChunkPtr( OmMipChunkCoord & coord)
{
	//	QMutexLocker lock(&mChunkCoords);

	const int level = coord.getLevel();
	Vector3 < int >data_dims = MipLevelDataDimensions(level);

	//round up to nearest chunk
	Vector3i rdims =
		Vector3i(ROUNDUP(data_dims.x, GetChunkDimension()),
			 ROUNDUP(data_dims.y, GetChunkDimension()),
			 ROUNDUP(data_dims.z, GetChunkDimension()));

	const qint64 x = (qint64)coord.getCoordinateX();
	const qint64 y = (qint64)coord.getCoordinateY();
	const qint64 z = (qint64)coord.getCoordinateZ();

	const qint64 xWidth  = 128;
	const qint64 yDepth  = 128;
	const qint64 zHeight = 128;

	const qint64 slabSize = (qint64)rdims.x * (qint64)rdims.y * (qint64)zHeight * (qint64)GetBytesPerSample();
	const qint64 rowSize =  (qint64)rdims.x * (qint64)yDepth  * (qint64)zHeight * (qint64)GetBytesPerSample();
	const qint64 cSize =    (qint64)xWidth  * (qint64)yDepth  * (qint64)zHeight * (qint64)GetBytesPerSample();

	const qint64 offset = slabSize*z + rowSize*y + cSize*x;

	debug("newimport", "offset is: %llu (%d,%d,%d) for (%d,%d,%d)\n", offset,
	      DEBUGV3(rdims), DEBUGV3(coord.Coordinate));

	return mFileMapPtr[level]+offset;
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

void OmMipVolume::copyDataIn( std::set<OmMipChunkCoord> & chunksToCopy)
{
	extern hid_t GlobalHDF5id;

	assert(-1 != GlobalHDF5id);
	printf("hdf5 id is :%d\n", GlobalHDF5id);

	OmDataPath path;
	path.setPathQstr( MipLevelInternalDataPath(0 ) );

	//Opens an existing dataset.
	//hid_t H5Dopen(hid_t loc_id, const char *name  )
	hid_t dataset_id = H5Dopen2(GlobalHDF5id, path.getString().c_str(), H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + path.getString());
	}


	//Returns an identifier for a copy of the datatype for a dataset.
	//hid_t H5Dget_type(hid_t dataset_id  )
	hid_t dataset_type_id = H5Dget_type(dataset_id);

	//assert that dest datatype size matches desired size
	assert(H5Tget_size(dataset_type_id) == (unsigned int)GetBytesPerSample());

	//Returns an identifier for a copy of the dataspace for a dataset.
	//hid_t H5Dget_space(hid_t dataset_id  )
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0) {
		throw OmIoException("Could not get HDF5 dataspace.");
	}

	int counter=0;
	const int total = chunksToCopy.size();

	foreach(const OmMipChunkCoord & c, chunksToCopy){
		OmMipChunkPtr chunk;
		GetChunk(chunk, c);

		if(4 == GetBytesPerSample()){
			assert(0);
		} else {
			OmDataWrapperPtr dataPtrMapped = chunk->RawReadChunkDataUCHARmapped();
			unsigned char* imageData = dataPtrMapped->getPtr<unsigned char>();

			DataBbox extent = chunk->GetExtent();

			//create start, stride, count, block
			//flip coordinates cuz thats how hdf5 likes it
			Vector3 < hsize_t > start = extent.getMin();
			Vector3 < hsize_t > end   = extent.getMax();
			Vector3 < hsize_t > start_flipped(start.z, start.y, start.x);

			Vector3 < hsize_t > stride = Vector3i::ONE;
			Vector3 < hsize_t > count = Vector3i::ONE;

			Vector3 < hsize_t > block = extent.getUnitDimensions();
			Vector3 < hsize_t > block_flipped(block.z, block.y, block.x);

			herr_t ret = H5Sselect_hyperslab(dataspace_id,
							 H5S_SELECT_SET,
							 start_flipped.array,
							 stride.array,
							 count.array,
							 block_flipped.array);
			if (ret < 0)
				throw OmIoException("Could not select HDF5 hyperslab.");

			hid_t mem_dataspace_id = H5Screate_simple(3, block.array, block.array);
			if (mem_dataspace_id < 0)
				throw OmIoException("Could not create scratch HDF5 dataspace.");

			Vector3 < int >extent_dims = extent.getUnitDimensions();

			hid_t mem_type_id = 1 == GetBytesPerSample() ?  H5T_NATIVE_UCHAR : H5T_NATIVE_UINT;
			ret = H5Dwrite(dataset_id,
				       mem_type_id,
				       mem_dataspace_id,
				       dataspace_id,
				       H5P_DEFAULT,
				       imageData);
			if (ret < 0) {
				throw OmIoException("Could not write HDF5 dataset "+path.getString());
			}


			ret = H5Sclose(mem_dataspace_id);
			if (ret < 0)
				throw OmIoException("Could not close HDF5 scratch dataspace.");
		}

		++counter;
		printf("\rwrote chunk %dx%dx%d to HDF5 (%d of %d total)",
		       chunk->GetCoordinate().Coordinate.x,
		       chunk->GetCoordinate().Coordinate.y,
		       chunk->GetCoordinate().Coordinate.z,
		       counter, total);
		fflush(stdout);
	}

	herr_t ret = H5Sclose(dataspace_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataspace.");

	//Closes the specified dataset.
	//herr_t H5Dclose(hid_t dataset_id  )
	ret = H5Dclose(dataset_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataset.");

	printf("\n");
}
