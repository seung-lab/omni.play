#include "common/omDebug.h"
#include "datalayer/archive/omDataArchiveMipChunk.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "volume/omChunkData.hpp"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"
#include "volume/omVolumeData.hpp"

/**
 *	Constructor speicifies MipCoord and MipVolume the chunk extracts data from
 */
OmMipChunk::OmMipChunk(const OmMipChunkCoord & coord, OmMipVolume* vol)
	: vol_(vol)
	, containedValuesDataLoaded(false)
	, cache_(vol->getDataCache())
	, mChunkData(boost::make_shared<OmChunkData>(vol, this, coord))
{
	InitChunk(coord);
}

/*
 *	Initialize chunk with properties of given coordinate.
 */
void OmMipChunk::InitChunk(const OmMipChunkCoord & rMipCoord)
{
	//set coordinate
	mCoordinate = rMipCoord;

	//set parent, if any
	if (rMipCoord.Level == vol_->GetRootMipLevel()) {
		mParentCoord = OmMipChunkCoord::NULL_COORD;
	} else {
		mParentCoord = rMipCoord.ParentCoord();
	}

	//set children
	vol_->ValidMipChunkCoordChildren(rMipCoord, mChildrenCoordinates);

	//get extent from coord
	mDataExtent = vol_->MipCoordToDataBbox(rMipCoord, rMipCoord.Level);

	//set norm extent
	mNormExtent = vol_->MipCoordToNormBbox(rMipCoord);

	//set clipped norm extent
	mClippedNormExtent = vol_->MipCoordToNormBbox(rMipCoord);
	mClippedNormExtent.intersect(AxisAlignedBoundingBox<float>::UNITBOX);
}

const DataBbox& OmMipChunk::GetExtent()
{
	return mDataExtent;
}

void OmMipChunk::ReadMetaData()
{
	OmDataPath path(vol_->MipChunkMetaDataPath(mCoordinate));

	//read archive if it exists
	if (OmProjectData::GetProjectIDataReader()->dataset_exists(path)){
		OmDataArchiveMipChunk::ArchiveRead(path, this);
	}
}

void OmMipChunk::WriteMetaData()
{
	OmDataPath path(vol_->MipChunkMetaDataPath(mCoordinate));
	OmDataArchiveMipChunk::ArchiveWrite(path, this);
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
void OmMipChunk::SetVoxelValue(const DataCoord & volVoxel, uint32_t val)
{
	assert(ContainsVoxel(volVoxel));

	const DataCoord voxel = volVoxel - GetExtent().getMin();
	const uint32_t oldVal = mChunkData->SetVoxelValue(voxel, val);

	mModifiedVoxelValues.insert(oldVal);
	mModifiedVoxelValues.insert(val);
}

/*
 *	Get voxel value from the ImageData associated with this MipChunk.
 */
uint32_t OmMipChunk::GetVoxelValue(const DataCoord & volVoxel)
{
	assert(ContainsVoxel(volVoxel));

	const DataCoord voxel = volVoxel - GetExtent().getMin();

	return mChunkData->GetVoxelValue(voxel);
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

const OmMipChunkCoord & OmMipChunk::GetCoordinate()
{
	return mCoordinate;
}

const OmMipChunkCoord & OmMipChunk::GetParentCoordinate()
{
	return mParentCoord;
}

const std::set<OmMipChunkCoord>& OmMipChunk::GetChildrenCoordinates()
{
	return mChildrenCoordinates;
}

/////////////////////////////////
///////          Property Accessors

const NormBbox& OmMipChunk::GetNormExtent()
{
	return mNormExtent;
}

const NormBbox& OmMipChunk::GetClippedNormExtent()
{
	return mClippedNormExtent;
}

/////////////////////////////////
///////          MetaData Accessors

/*
 *	Returns reference to set of all values directly contained by
 *	the image data of this MipChunk
 */
const OmSegIDsSet& OmMipChunk::GetDirectDataValues()
{
	zi::guard g(mDirectDataValueLock);
	loadMetadataIfPresent();

	return mDirectlyContainedValues;
}

void OmMipChunk::loadMetadataIfPresent()
{
	if( containedValuesDataLoaded ){
		return;
	}

	if (vol_->GetChunksStoreMetaData()) {
		ReadMetaData();
	}

	/*
	  std::cout << "chunk " << mCoordinate
	  << " contains " << mDirectlyContainedValues.size()
	  << " directly contained values\n";
	*/

	containedValuesDataLoaded = true;
}

/*
 *	Analyze segmentation ImageData in the chunk associated to a MipCoord and store
 *	all values in the DataSegmentId set of the chunk.
 */
void OmMipChunk::RefreshDirectDataValues(const bool computeSizes,
										 OmSegmentCache* segCache)
{
	mDirectlyContainedValues.clear();
	mChunkData->RefreshDirectDataValues(computeSizes, segCache);
	containedValuesDataLoaded = true;
	WriteMetaData();
}

/**
 *      Returns new OmImage containing the entire extent of data needed
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
				if (!vol_->ContainsMipChunkCoord(mip_coord))
					continue;

				OmMipChunkPtr chunk;
				vol_->GetChunk(chunk, mip_coord);

				OmImage<uint32_t, 3> chunkImage = chunk->GetCopyOfChunkDataAsOmImage32();

				retImage.copyFrom(chunkImage,
								  OmExtents[z*128][y*128][x*128],
								  OmExtents[0][0][0],
								  OmExtents[lenZ][lenY][lenX]);

			}
		}
	}

	return retImage;
}

bool OmMipChunk::ContainsVoxel(const DataCoord & vox)
{
	return GetExtent().contains(vox);
}

const Vector3i OmMipChunk::GetDimensions()
{
	return GetExtent().getUnitDimensions();
}

OmDataWrapperPtr OmMipChunk::RawReadChunkDataHDF5()
{
	OmDataPath path(vol_->MipLevelInternalDataPath(GetLevel()));

	OmDataWrapperPtr data =
		OmProjectData::GetProjectIDataReader()->
		readChunk(path, GetExtent());

	return data;
}

boost::shared_ptr<uint8_t> OmMipChunk::ExtractDataSlice8bit(const ViewType plane,
															const int offset)
{
	return mChunkData->ExtractDataSlice8bit(plane, offset);
}

boost::shared_ptr<uint32_t> OmMipChunk::ExtractDataSlice32bit(const ViewType plane,
															  const int offset)
{
	return mChunkData->ExtractDataSlice32bit(plane, offset);
}

void OmMipChunk::copyInTile(const int sliceOffset, uchar* bits)
{
	mChunkData->copyInTile(sliceOffset, bits);
}

void OmMipChunk::copyDataFromHDF5toMemMap()
{
	mChunkData->copyDataFromHDF5toMemMap();
}

void OmMipChunk::copyInChunkData(OmDataWrapperPtr hdf5)
{
	mChunkData->copyDataFromHDF5toMemMap(hdf5);
}

OmImage<uint32_t, 3> OmMipChunk::GetCopyOfChunkDataAsOmImage32()
{
	return mChunkData->GetCopyOfChunkDataAsOmImage32();
}

bool OmMipChunk::compare(OmMipChunkPtr other)
{
	if(GetCoordinate().Level != other->GetCoordinate().Level){
		printf("Chunks differ: Different mip levels.\n");
		return false;
	}

 	if(GetDimensions() != other->GetDimensions()){
		printf("Chunks differ: Different dimensions.\n");
		return false;
	}

	return mChunkData->compare(other->mChunkData);
}

int OmMipChunk::GetNumberOfVoxelsInChunk() const {
	const int sideDim = vol_->GetChunkDimension();
	return sideDim*sideDim*sideDim;
}

double OmMipChunk::GetMinValue()
{
	return mChunkData->GetMinValue();
}

double OmMipChunk::GetMaxValue()
{
	return mChunkData->GetMaxValue();
}

