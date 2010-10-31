#include "volume/build/omVolumeAllocater.hpp"
#include "system/cache/omMipVolumeCache.h"
#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "project/omProject.h"
#include "utility/stringHelpers.h"
#include "common/omDebug.h"
#include "common/omException.h"
#include "common/omStd.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "datalayer/omDataLayer.h"
#include "system/omProjectData.h"
#include "utility/omImageDataIo.h"
#include "utility/omTimer.h"
#include "utility/sortHelpers.h"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"
#include "volume/omVolumeData.hpp"

#include <QImage>

OmMipVolume::OmMipVolume()
	: mVolDataType(OmVolDataType::UNKNOWN)
	, mMaxVal(0)
	, mMinVal(0)
	, mWasBounded(false)

{
	sourceFilesWereSet = false;

	//init
	SetBuildState(MIPVOL_UNBUILT);
	SetChunksStoreMetaData(false);
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
void OmMipVolume::SetSourceFilenamesAndPaths(const QFileInfoList& srcFiles)
{
	mSourceFilenamesAndPaths = SortHelpers::SortNaturally(srcFiles);
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

Vector3i OmMipVolume::GetChunkDimensions() const
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

		//debug(hdf5image, "%i:%i:%i, from %s and %s\n", DEBUGV3(source_dims));

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
Vector3i OmMipVolume::MipLevelDataDimensions(int level) const
{
	//get dimensions
	DataBbox source_extent = GetDataExtent();
	Vector3f source_dims = source_extent.getUnitDimensions();

	//dims in fraction of pixels
	Vector3f mip_level_dims = source_dims / om::pow2int(level);

	return Vector3i (ceil(mip_level_dims.x),
					 ceil(mip_level_dims.y),
					 ceil(mip_level_dims.z));
}

// TODO: this appear to NOT do the right thing for level=0 (purcaro)

/*
 *	Calculate the MipChunkCoord dims required to contain all the chunks of a given level.
 */
Vector3i OmMipVolume::MipLevelDimensionsInMipChunks(int level)
{
	Vector3f data_dims = MipLevelDataDimensions(level);
	return Vector3i (ceil(data_dims.x / GetChunkDimension()),
					 ceil(data_dims.y / GetChunkDimension()),
					 ceil(data_dims.z / GetChunkDimension()));
}

/////////////////////////////////
///////          MipCoordinate Methods

OmMipChunkCoord OmMipVolume::NormToMipCoord(const NormCoord & normCoord, int level)
{
	return DataToMipCoord(OmVolume::NormToDataCoord(normCoord), level);
}

/*
 *	Returns the extent of the data in specified level covered by the given MipCoordinate.
 */
DataBbox OmMipVolume::MipCoordToDataBbox(const OmMipChunkCoord & rMipCoord, int newLevel)
{

	int old_level_factor = om::pow2int(rMipCoord.Level);
	int new_level_factor = om::pow2int(newLevel);

	//convert to leaf level dimensions
	int leaf_dim = GetChunkDimension() * old_level_factor;
	Vector3i leaf_dims = GetChunkDimensions() * old_level_factor;

	//min of extent in leaf data coordinates
	DataCoord leaf_min_coord = rMipCoord.Coordinate * leaf_dim;

	//convert to new level
	DataCoord new_extent_min_coord = leaf_min_coord / new_level_factor;

	Vector3i new_dims = leaf_dims / new_level_factor;

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
bool OmMipVolume::ContainsMipChunkCoord(const OmMipChunkCoord & rMipCoord)
{
	//if level is greater than root level
	if(rMipCoord.Level < 0 ||
	   rMipCoord.Level > GetRootMipLevel()){
	   return false;
	}

	//convert to data box in leaf (MIP 0)
	DataBbox bbox = MipCoordToDataBbox(rMipCoord, 0);

	bbox.intersect(GetDataExtent());
	if(bbox.isEmpty()){
		return false;
	}

	//else valid mip coord
	return true;
}

/*
 *	Finds set of children coordinates that are valid for this MipVolume.
 */
void OmMipVolume::ValidMipChunkCoordChildren(const OmMipChunkCoord & rMipCoord,
											 std::set<OmMipChunkCoord>& children)
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
void OmMipVolume::GetChunk(OmMipChunkPtr& p_value,
						   const OmMipChunkCoord & rMipCoord)
{
	//ensure either built or building
	assert(mBuildState != MIPVOL_UNBUILT);

	getDataCache()->Get(p_value, rMipCoord);
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

Vector3i OmMipVolume::getDimsRoundedToNearestChunk(const int level) const
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

	getVolData()->downsample(this);

	//build volume
	if (!BuildThreadedVolume()) {
		DeleteVolumeData();
		SetBuildState(MIPVOL_UNBUILT);
		return;
	}

	//build complete
	SetBuildState(MIPVOL_BUILT);
}

/**
 *	Build all chunks in MipLevel of this MipVolume with multithreading
 */
bool OmMipVolume::BuildThreadedVolume()
{
	OmTimer vol_timer;
	vol_timer.start();

	try{
		doBuildThreadedVolume();
	} catch(...){
		return false;
	}

	printf("OmMipVolume:BuildThreadedVolume() done : %.6f secs\n",
		   vol_timer.s_elapsed());

	return true;
}

/*
 *	Rebuild all chunks in the edited chunk set.
 */
void OmMipVolume::BuildEditedLeafChunks()
{
	//first build all the edited leaf chunks
/*
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
*/
}

/////////////////////////////////
///////        Comparison

/*
 *	Returns true if two given volumes are exactly the same.
 *	Prints all positions where volumes differ if verbose flag is set.
 */
bool OmMipVolume::CompareVolumes(OmMipVolume *vol1, OmMipVolume *vol2)
{
	//check if dimensions are the same
	if (vol1->GetDataExtent().getUnitDimensions() !=
	    vol2->GetDataExtent().getUnitDimensions()){
		printf("Volumes differ: Different dimensions.\n");
		return false;
	}


	//root mip level should be the same if data dimensions are the same
	if( vol1->GetRootMipLevel() != vol2->GetRootMipLevel() ){
		printf("Volumes differ: Different number of MIP levels.\n");
		return false;
	}

	for (int level = 0; level <= vol1->GetRootMipLevel(); ++level) {
		printf("Comparing mip level %i\n",level);

		const Vector3i mip_coord_dims =
			vol1->MipLevelDimensionsInMipChunks(level);

		//for all coords
		for (int z = 0; z < mip_coord_dims.z; ++z){
			for (int y = 0; y < mip_coord_dims.y; ++y){
				for (int x = 0; x < mip_coord_dims.x; ++x){

					OmMipChunkCoord coord(level,x,y,z);
					if(CompareChunks(coord, vol1, vol2)){
						continue;
					}

					std::cout << "\tchunks differ at "
							  << coord << "; aborting...\n";
					return false;
				}
			}
		}
	}

	return true;
}

/*
 *	Returns true if two given chunks contain the exact same image data
 *	Returns true even if chunk position differs
 */
bool OmMipVolume::CompareChunks(const OmMipChunkCoord& coord,
								OmMipVolume* vol1, OmMipVolume* vol2)
{
	OmMipChunkPtr chunk1;
	vol1->GetChunk(chunk1, coord);

	OmMipChunkPtr chunk2;
	vol2->GetChunk(chunk2, coord);

	return chunk1->compare(chunk2);
}

bool OmMipVolume::ContainsVoxel(const DataCoord & vox)
{
	return GetDataExtent().contains(vox);
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
	const uint32_t numChunks = computeTotalNumChunks();
	uint32_t counter = 0;

	for (int level = 0; level <= GetRootMipLevel(); ++level) {

		Vector3i mip_coord_dims =
			MipLevelDimensionsInMipChunks(level);

		for (int z = 0; z < mip_coord_dims.z; ++z){
			for (int y = 0; y < mip_coord_dims.y; ++y){
				for (int x = 0; x < mip_coord_dims.x; ++x){

					OmMipChunkCoord coord(level, x, y, z);

					OmMipChunkPtr chunk;
					GetChunk(chunk, coord);

					++counter;
					printf("\rcopying chunk %d of %d...", counter, numChunks);
					chunk->copyDataFromHDF5toMemMap();
				}
			}
		}
	}

	fflush(stdout);
}

/**
 *	Export leaf volume data to HDF5 format.
 */
void OmMipVolume::ExportInternalData(const QString& fileNameAndPath,
									 const bool rerootSegments)
{
	printf("starting export...\n");
	const DataBbox leaf_data_extent = GetDataExtent();
	const Vector3i leaf_mip_dims = MipLevelDimensionsInMipChunks(0);
	const OmDataPath mip_volume_path(MipLevelInternalDataPath(0));

	OmIDataWriter* hdfExport =
		OmDataLayer::getWriter(fileNameAndPath.toStdString(), false);
	OmDataPath fpath("main");

	if( !QFile::exists(fileNameAndPath) ){
		hdfExport->create();
		hdfExport->open();
		const Vector3i full = MipLevelDataDimensions(0);
		const Vector3i rounded_data_dims = getDimsRoundedToNearestChunk(0);
		hdfExport->allocateChunkedDataset(fpath,
										  rounded_data_dims,
										  full,
										  mVolDataType);
	} else {
		hdfExport->open();
	}

	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {
				const OmMipChunkCoord coord(0, x, y, z);
				OmDataWrapperPtr data = doExportChunk(coord, rerootSegments);

				const DataBbox chunk_data_bbox =
					MipCoordToDataBbox(coord, 0);
				hdfExport->writeChunk(OmDataPaths::getDefaultDatasetName(),
									  chunk_data_bbox,
									  data);
			}
		}
	}

	hdfExport->close();

	printf("\nexport done!\n");
}

uint32_t OmMipVolume::computeTotalNumChunks()
{
	uint32_t numChunks = 0;

	for (int level = 0; level <= GetRootMipLevel(); ++level) {
		const Vector3i dims = MipLevelDimensionsInMipChunks(level);
		numChunks += dims.x * dims.y * dims.z;
	}

	return numChunks;
}

// TODO: ignore 0?
void OmMipVolume::updateMinMax(const double inMin, const double inMax)
{
	static zi::mutex mutex;
	zi::guard g(mutex);

	mMinVal = std::min(inMin, mMinVal);
	mMaxVal = std::max(inMax, mMaxVal);
}

uint64_t OmMipVolume::ComputeChunkPtrOffsetBytes(const OmMipChunkCoord& coord) const
{
	const int level = coord.Level;
	const Vector3<int64_t> volDims = getDimsRoundedToNearestChunk(level);
	const Vector3<int64_t> chunkDims = GetChunkDimensions();
	const int64_t bps = GetBytesPerSample();

	const int64_t slabSize  = volDims.x   * volDims.y   * chunkDims.z * bps;
	const int64_t rowSize   = volDims.x   * chunkDims.y * chunkDims.z * bps;
	const int64_t chunkSize = chunkDims.x * chunkDims.y * chunkDims.z * bps;

	const Vector3<int64_t> chunkPos = coord.Coordinate; // bottom left corner
	const int64_t offset =
		slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;

	ZiLOG(DEBUG, io) << "offset is: " << offset
					 << " (" << volDims << ") for "
					 << coord.Coordinate << "\n";
	return offset;
}
