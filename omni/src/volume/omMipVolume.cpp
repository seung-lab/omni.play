#include "omMipVolume.h"
#include "omMipChunk.h"
#include "omVolume.h"

#include "common/omException.h"
#include "system/omProjectData.h"
#include "system/omEventManager.h"
#include "system/events/omProgressEvent.h"
#include "common/omVtk.h"
#include "common/omStd.h"
#include "utility/omImageDataIo.h"

#include <vtkImageData.h>
#include <vtkExtractVOI.h>
#include <vtkImageConstantPad.h>
#include "common/omDebug.h"
#include "utility/omHdf5.h"
#include <QFile>

static const char *MIP_VOLUME_FILENAME = "volume.dat";
static const QString MIP_CHUNK_META_DATA_FILE_NAME = "metachunk.dat";

/////////////////////////////////
///////
///////         OmMipVolume
///////

OmMipVolume::OmMipVolume()
 : MipChunkThreadedCache(RAM_CACHE_GROUP)
{
	sourceFilesWereSet = false;

        SetCacheName("OmMipVolume");

	//init
	SetFilename(MIP_VOLUME_FILENAME);
	SetDirectoryPath("default/");
	SetBuildState(MIPVOL_UNBUILT);
	SetChunksStoreMetaData(false);

	mBytesPerSample = 1;
	mSubsampleMode = SUBSAMPLE_NONE;
	mCompleteDelete=false;

	mSimpleChunkThreadedCache = new OmSimpleChunkThreadedCache(this);
}

OmMipVolume::~OmMipVolume()
{
	//debug("genone","OmMipVolume::~OmMipVolume()");

	//flush edits
	if (!mCompleteDelete) Flush();

	//clear cache before destructing since chunks depend on existance 
	//of parent mipvolume
	if (!mCompleteDelete) MipChunkThreadedCache::Clear();

}

void OmMipVolume::PrepareForCompleteDelete()
{
	mCompleteDelete=true;
	this->KillFetchThread();
}

/////////////////////////////////
///////          DataVolume

/*
 *	Flush all changes to disk
 */
void
 OmMipVolume::Flush()
{
	//build any edited leaf chunks
	BuildEditedLeafChunks();

	//flush all chunks in the cache
	MipChunkThreadedCache::Flush();
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
	mSourceFilenamesAndPaths = sourceFilenamesAndPaths;
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

	foreach( QFileInfo fi, mSourceFilenamesAndPaths ){
		if( !fi.exists() ){
			return false;
		}
	}
	
	return true;
}

/////////////////////////////////
///////          Mip Data Properties

const DataBbox & OmMipVolume::GetExtent()
{
	debug("hdf5image", "extents: %i,%i,%i\n", DEBUGV3(OmVolume::GetDataExtent().getMax()));
	return OmVolume::GetDataExtent();
}

int OmMipVolume::GetChunkDimension()
{
	return OmVolume::GetChunkDimension();
}

Vector3 < int > OmMipVolume::GetChunkDimensions()
{
	return Vector3 < int >(GetChunkDimension(), GetChunkDimension(), GetChunkDimension());
}

void OmMipVolume::SetSubsampleMode(int subMode)
{
	mSubsampleMode = (SubsampleMode) subMode;
}

int OmMipVolume::GetSubsampleMode()
{
	return mSubsampleMode;
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
void OmMipVolume::UpdateMipProperties()
{
	if (IsSourceValid()) {
		//get source dimensions
		Vector3 < int >source_dims = OmImageDataIo::om_imagedata_get_dims( mSourceFilenamesAndPaths );

		debug("hdf5image", "%i:%i:%i, from %s and %s\n", DEBUGV3(source_dims));

		//if dim differs from OmVolume alert user
		if (OmVolume::GetDataDimensions() != source_dims) {
			printf("OmMipVolume::UpdateMipProperties: CHANGING VOLUME DIMENSIONS\n");

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
	Vector3 < int >source_dims = GetExtent().getUnitDimensions();
	int max_source_dim = source_dims.getMaxComponent();
	int mipchunk_dim = GetChunkDimension();

	if (max_source_dim <= mipchunk_dim) {
		mMipRootLevel = 0;
	} else {
		//use log base 2 to determine levels needed to contain source dims
		mMipRootLevel = ceil(log((float) (max_source_dim) / GetChunkDimension()) / log((float)2));
	}
}

/*
 *	Calculate the data dimensions needed to contain the volume at a given compression level.
 */
Vector3 < int > OmMipVolume::MipLevelDataDimensions(int level)
{
	//get dimensions
	DataBbox source_extent = GetExtent();
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
bool OmMipVolume::ContainsMipChunkCoord(const OmMipChunkCoord & mipCoord)
{

	//if level is greater than root level
	if (mipCoord.Level > GetRootMipLevel())
		return false;

	//convert to data box in leaf
	DataBbox mip_coord_data_bbox = MipCoordToDataBbox(mipCoord, 0);

	//insersect and check if not empty
	mip_coord_data_bbox.intersect(GetExtent());
	if (mip_coord_data_bbox.isEmpty())
		return false;

	//else valid mip coord
	return true;
}

/*
 *	Finds set of children coordinates that are valid for this MipVolume.
 */
void OmMipVolume::ValidMipChunkCoordChildren(const OmMipChunkCoord & mipCoord, set < OmMipChunkCoord > &children)
{
	//clear set
	children.clear();

	//get all possible children
	OmMipChunkCoord possible_children[8];
	mipCoord.ChildrenCoords(possible_children);

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
void OmMipVolume::GetChunk(QExplicitlySharedDataPointer < OmMipChunk > &p_value, const OmMipChunkCoord & rMipCoord, bool block)
{
	//ensure either built or building
	assert(mBuildState != MIPVOL_UNBUILT);

	MipChunkThreadedCache::Get(p_value, rMipCoord, block);
}

void OmMipVolume::GetSimpleChunk(QExplicitlySharedDataPointer < OmSimpleChunk > &p_value, const OmMipChunkCoord & rMipCoord, bool block)
{
	//ensure either built or building
	assert(mBuildState != MIPVOL_UNBUILT);

	mSimpleChunkThreadedCache->Get(p_value, rMipCoord, block);
}
/*
 *	Store chunk by adding it to the cache.
 */
void OmMipVolume::StoreChunk(const OmMipChunkCoord & rMipCoord, OmMipChunk * pMipChunk)
{

	assert(ContainsMipChunkCoord(rMipCoord));

	MipChunkThreadedCache::Add(rMipCoord, pMipChunk);
}

/*
 *	A cache miss causes an initialization of a chunk.
 */
OmMipChunk *OmMipVolume::HandleCacheMiss(const OmMipChunkCoord & rMipCoord)
{

	assert(ContainsMipChunkCoord(rMipCoord));

	return new OmMipChunk(rMipCoord, this);
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
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
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
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	GetChunk(p_chunk, leaf_mip_coord);
	//get voxel data
	p_chunk->SetVoxelValue(vox, val);
	//note the chunk has been edited
	mEditedLeafChunks.insert(leaf_mip_coord);

	// //debug("FIXME", << "OmMipVolume::SetVoxelValue done" << endl;
}

/////////////////////////////////
///////         Mip Construction Methods

/*
 *	Allocate the image data for all mip level volumes.
 *	Note: root level, leaf dim, bytes per sample must already be set
 */
void OmMipVolume::AllocInternalData()
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
		
		OmHdf5Path mip_volume_level_path;
		mip_volume_level_path.setPathQstr( MipLevelInternalDataPath(i) );;

		//debug("genone","OmMipVolume::AllocInternalData: %s \n", mip_volume_level_path.data());
		OmProjectData::GetDataWriter()->dataset_image_create_tree_overwrite(mip_volume_level_path, 
										    &rounded_data_dims,
										    &chunkdims, 
										    GetBytesPerSample());
	}

}

void OmMipVolume::DeleteVolumeData()
{
	DeleteInternalData();
}


/**
 *	Deletes all data on disk associated with this MipVolume if it exists.
 */
void OmMipVolume::DeleteInternalData()
{
	OmHdf5Path path;
	path.setPathQstr( mDirectoryPath );

	//TODO: mutex lock this!!!!
	if (OmProjectData::GetProjectDataReader()->group_exists(path)) {
		OmProjectData::GetDataWriter()->group_delete(path);
	}
}

/////////////////////////////////
///////          Building

/*
 *	Build all MipLevel resolutions of the MipVolume.
 */
void OmMipVolume::Build()
{
	//unbuild
	SetBuildState(MIPVOL_BUILDING);
	
	//update properties
	UpdateMipProperties();

	//delete old
	DeleteInternalData();

	//alloc new
	AllocInternalData();

	//if source data valid
	if (!IsSourceValid()) {
		printf("OmMipVolume::Build: blank build complete\n");
		SetBuildState(MIPVOL_BUILT);
		return;
	}
	//copy source data
	if (!ImportSourceData()) {
		DeleteInternalData();
		SetBuildState(MIPVOL_UNBUILT);
		return;
	}
	//build volume
	if (!BuildVolume()) {
		DeleteInternalData();
		SetBuildState(MIPVOL_UNBUILT);
		return;
	}

	//build complete
	SetBuildState(MIPVOL_BUILT);
}

/*
 *	Build all chunks in MipLevel of this MipVolume
 */
bool OmMipVolume::BuildVolume()
{
	//debug("FIXME", << "OmMipVolume::BuildVolume()" << endl;
	//init progress bar
	int prog_count = 0;
	OmEventManager::
	    PostEvent(new
		      OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, string("Building volume...               "), 0,
				      MipChunksInVolume()));

	//for each level
	for (int level = 0; level <= GetRootMipLevel(); ++level) {
		printf("building mip level %d...\n", level );

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
	}

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
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	GetChunk(p_chunk, rMipCoord);

	//read original data
	OmHdf5Path source_data_path;
	source_data_path.setPathQstr( MipLevelInternalDataPath(rMipCoord.Level - 1) );
	DataBbox source_data_bbox = MipCoordToDataBbox(rMipCoord, rMipCoord.Level - 1);

	//read and get pointer to data
	vtkImageData *p_source_data =
		OmProjectData::GetProjectDataReader()->dataset_image_read_trim(source_data_path, 
									source_data_bbox, 
									GetBytesPerSample());

	//subsample
	vtkImageData *p_subsampled_data = NULL;

	//switch on scalar type
	switch (GetBytesPerSample()) {
	case 1:
		p_subsampled_data = SubsampleImageData < unsigned char >(p_source_data, GetSubsampleMode());
		break;
	case 4:
		p_subsampled_data = SubsampleImageData < unsigned int >(p_source_data, GetSubsampleMode());
		break;
	default:
		assert(false);
	}

	//set or replace image data (chunk now owns pointer)
	p_chunk->SetImageData(p_subsampled_data);

	//delete source data
	p_source_data->Delete();
	p_source_data = NULL;
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
		QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
		GetChunk(p_chunk, *itr);

		//rebuild ancestors
		BuildChunkAndParents(*itr);

		//clear modified values
		p_chunk->ClearModifiedVoxelValues();
	}

	//edited chunks clean
	mEditedLeafChunks.clear();
}

/////////////////////////////////
///////          IO

/*
 *	Given a valid source data volume, thes copies entire source volume to
 *	the leaf mip level of this mip volume.
 */
bool OmMipVolume::ImportSourceData()
{
	//debug("genone","OmMipVolume::ImportSourceData()");

	//init progress bar
	int prog_count = 0;
	OmEventManager::PostEvent( new OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, string("Importing data..."), 0,
						       MipChunksInMipLevel(0)));
	//dim of leaf coords
	Vector3 < int >leaf_mip_dims = MipLevelDimensionsInMipChunks(0);
	OmHdf5Path leaf_volume_path;
	leaf_volume_path.setPathQstr( MipLevelInternalDataPath(0) );

	printf("importing data...\n");

	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {

				//get chunk data bbox
				OmMipChunkCoord chunk_coord = OmMipChunkCoord(0, x, y, z);
				DataBbox chunk_data_bbox = MipCoordToDataBbox(chunk_coord, 0);
				//debug("FIXME", << "OmMipVolume::ImportSourceData: " << chunk_data_bbox << endl;

				//read chunk image data from source
				vtkImageData *p_img_data =
					OmImageDataIo::om_imagedata_read(mSourceFilenamesAndPaths, 
									 GetExtent(), 
									 chunk_data_bbox, 
									 GetBytesPerSample());
				
				//write to project data
				OmProjectData::GetDataWriter()->dataset_image_write_trim(leaf_volume_path, 
											 &chunk_data_bbox, 
											 GetBytesPerSample(),
											 p_img_data);

				//delete read data
				p_img_data->Delete();

				//check for cancel (auto hides on cancel)
				if (OmProgressEvent::GetWasCanceled()){
					return false;
				}

				//update progress
				OmEventManager::
				    PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_VALUE, ++prog_count));
			}
		}
	}

	//hide progress bar
	OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_HIDE));
	return true;
}

/*
 *	Export leaf volume data to HDF5 format.  Calls ExportImageDataFilter so subclass can 
 *	post-process the image data before it is written.
 */
void OmMipVolume::ExportInternalData(QString fileNameAndPath)
{
	debug("hdf5image", "OmMipVolume::ExportInternalData(%s)\n", qPrintable(fileNameAndPath));

	//get leaf data extent
	DataBbox leaf_data_extent = GetExtent();

	//dim of leaf coords
	Vector3 < int >leaf_mip_dims = MipLevelDimensionsInMipChunks(0);
	OmHdf5Path mip_volume_path;
	mip_volume_path.setPathQstr( MipLevelInternalDataPath(0) );
        //round up to nearest chunk

        OmHdf5 hdfExport( fileNameAndPath, false, false );
        OmHdf5Path fpath;
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
				vtkImageData *p_chunk_img_data =
					OmProjectData::GetProjectDataReader()->dataset_image_read_trim(mip_volume_path, 
												chunk_data_bbox, 
												GetBytesPerSample());

				//apply export filter
				ExportDataFilter(p_chunk_img_data);

				//write to hdf5 file
				//debug("FIXME", << "OmMipVolume::Export:" << chunk_data_bbox << endl;

				hdfExport.dataset_image_write_trim(OmHdf5Helpers::getDefaultDatasetName(),
                                           (DataBbox*)&chunk_data_bbox, GetBytesPerSample(), p_chunk_img_data);


				//delete read data
				p_chunk_img_data->Delete();
			}
		}
	}
	hdfExport.close();
}

/////////////////////////////////
///////          Subsampling Methods            

/*
 *	Given a valid vtkImageData source and a specified SubsampleMode, this method quickly
 *	subsamples the source such that the returned vtkImageData is half the original size
 *	on all dimensions.
 *
 *	The source image data is required to have even sized dimensions.
 */
template < typename T > vtkImageData * OmMipVolume::SubsampleImageData(vtkImageData * srcData, int subsampleMode)
{
	//debug("FIXME", << "OmMipVolume::SubsampleImageData" << endl;

	//get image data dimensions
	Vector3 < int >src_dims;
	srcData->GetDimensions(src_dims.array);
	int scalar_type = srcData->GetScalarType();
	int num_scalar_components = srcData->GetNumberOfScalarComponents();

	//assert proper dims
	//debug("FIXME", << "OmMipVolume::SubsampleImageData: " << src_dims << endl;
	assert((src_dims.x == src_dims.y) && (src_dims.y == src_dims.z));
	assert(src_dims.x % 2 == 0);

	//alloc dest image data
	Vector3 < int >dest_dims = src_dims / 2;

	vtkImageData *p_dest_data = vtkImageData::New();
	p_dest_data->SetDimensions(dest_dims.array);
	p_dest_data->SetScalarType(scalar_type);
	p_dest_data->SetNumberOfScalarComponents(num_scalar_components);
	p_dest_data->AllocateScalars();
	p_dest_data->Update();

	//get pointer into subsampled data
	T *dest_data_ptr = static_cast < T * >(p_dest_data->GetScalarPointer());
	Vector3 < int >src_voxel, dest_voxel;

	//alloc octal values
	static T octal_values[8];

	//for all voxels in destination data
	for (dest_voxel.z = 0; dest_voxel.z < dest_dims.z; ++dest_voxel.z) {
		for (dest_voxel.y = 0; dest_voxel.y < dest_dims.y; ++dest_voxel.y) {
			for (dest_voxel.x = 0; dest_voxel.x < dest_dims.x; ++dest_voxel.x) {

				//get voxel position in source
				src_voxel = dest_voxel * 2;

				//store octal values from source
				octal_values[0] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x, src_voxel.y, src_voxel.z));
				octal_values[1] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x + 1, src_voxel.y, src_voxel.z));
				octal_values[2] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x + 1, src_voxel.y + 1, src_voxel.z));
				octal_values[3] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x, src_voxel.y + 1, src_voxel.z));
				octal_values[4] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x, src_voxel.y, src_voxel.z + 1));
				octal_values[5] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x + 1, src_voxel.y, src_voxel.z + 1));
				octal_values[6] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x + 1, src_voxel.y + 1, src_voxel.z + 1));
				octal_values[7] =
				    *static_cast <
				    T * >(srcData->GetScalarPointer(src_voxel.x, src_voxel.y + 1, src_voxel.z + 1));

				//switch on subsample mode
				switch (subsampleMode) {
				case SUBSAMPLE_MEAN:
					*dest_data_ptr = CalculateAverage < T > (octal_values, 8);
					break;

				case SUBSAMPLE_MODE:
					*dest_data_ptr = CalculateMode < T > (octal_values, 8);
					break;

				case SUBSAMPLE_RANDOM:
					*dest_data_ptr = octal_values[rand() % 8];
					break;

				case SUBSAMPLE_NONE:
					*dest_data_ptr = octal_values[0];
					break;

				default:
					assert(false);
				}

				//adv to next voxel (1 sample per voxel)
				++dest_data_ptr;
			}
		}
	}

	//return subsampled image data
	return p_dest_data;
}

template < typename T > T OmMipVolume::CalculateAverage(T * array, int size)
{
	unsigned long sum = 0;
	//for each element in the array
	for (int i = 0; i < size; ++i) {
		sum += array[i];
	}
	return round(float (sum) / size);
}

template < typename T > T OmMipVolume::CalculateMode(T * array, int size)
{

	//to avoid thrashing the stack
	static std::map < T, int >frequency_map;
	frequency_map.clear();

	T cur_value = 0;
	int cur_freq = 0;
	T max_freq_value = 0;
	int max_freq = 0;

	//for each element in the array
	for (int i = 0; i < size; ++i) {

		cur_value = array[i];

		//if element already exists
		if (frequency_map.count(cur_value)) {
			cur_freq = ++frequency_map[cur_value];
		} else {
			//add to map
			frequency_map[cur_value] = 1;
			cur_freq = 1;
		}

		//update max value if necessary
		if (cur_freq > max_freq) {
			max_freq = cur_freq;
			max_freq_value = cur_value;
		}
	}

	return max_freq_value;
}

bool OmMipVolume::ContainsVoxel(const DataCoord & vox)
{
	return GetExtent().contains(vox);
}

int OmMipVolume::GetBytesPerSample()
{
	return mBytesPerSample;
}


void OmMipVolume::SetBytesPerSample(int bytesPerSample)
{
	mBytesPerSample = bytesPerSample;
}

OmSimpleChunkThreadedCache* 
OmMipVolume::GetSimpleChunkThreadedCache()
{
	return mSimpleChunkThreadedCache;
}
