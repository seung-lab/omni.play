#include "volume/omUpdateBoundingBoxes.h"
#include "chunks/omSegChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPaths.h"
#include "mesh/drawer/omMeshDrawer.h"
#include "mesh/omMipMeshManagers.hpp"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentLists.hpp"
#include "system/omGroups.h"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
	: uniqueChunkValues_(new OmChunkUniqueValuesManager(this))
	, groups_(new OmGroups(this))
	, mst_(new OmMST(this))
	, meshDrawer_(new OmMeshDrawer(this))
	, meshManagers_(new OmMipMeshManagers(this))
	, chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this))
	, segmentCache_(new OmSegmentCache(this))
	, segmentLists_(new OmSegmentLists())
	, mstUserEdges_(new OmUserEdges(this))
	, validGroupNum_(new OmValidGroupNum(this))
	, volData_(new OmVolumeData())
{}

// used by OmGenericManager
OmSegmentation::OmSegmentation(OmID id)
	: OmManageableObject(id)
	, uniqueChunkValues_(new OmChunkUniqueValuesManager(this))
	, groups_(new OmGroups(this))
	, mst_(new OmMST(this))
	, meshDrawer_(new OmMeshDrawer(this))
	, meshManagers_(new OmMipMeshManagers(this))
	, chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this))
	, segmentCache_(new OmSegmentCache(this))
	, segmentLists_(new OmSegmentLists())
	, mstUserEdges_(new OmUserEdges(this))
	, validGroupNum_(new OmValidGroupNum(this))
	, volData_(new OmVolumeData())
{
	segmentCache_->refreshTree();
}

OmSegmentation::~OmSegmentation()
{}

std::string OmSegmentation::GetName(){
	return "segmentation" + om::NumToStr(GetID());
}

std::string OmSegmentation::GetDirectoryPath(){
	return OmDataPaths::getDirectoryPath(this);
}

void OmSegmentation::SetDendThreshold(const double t)
{
	static zi::rwmutex lock;
	zi::rwmutex::write_guard g(lock);

	if(qFuzzyCompare(t, mst_->UserThreshold())){
		return;
	}
	mst_->SetUserThreshold(t);
	segmentCache_->refreshTree();
}

void OmSegmentation::CloseDownThreads() {
	meshManagers_->CloseDownThreads();
}

void OmSegmentation::loadVolData()
{
	if(IsBuilt()){
		volData_->load(this);
	}
}

void OmSegmentation::loadVolDataIfFoldersExist()
{
	//assume level 0 data always present
	const QString path = OmFileNames::GetVolDataFolderPath(this, 0);

	if(QDir(path).exists()){
		loadVolData();
	}
}

double OmSegmentation::GetDendThreshold() {
	return mst_->UserThreshold();
}

void OmSegmentation::UpdateVoxelBoundingData()
{
	OmUpdateBoundingBoxes updater(this);
	updater.Update();
}

int OmSegmentation::GetBytesPerVoxel() const {
	return volData_->GetBytesPerVoxel();
}

void OmSegmentation::SetVolDataType(const OmVolDataType type)
{
	mVolDataType = type;
	volData_->SetDataType(this);
}

SegmentationDataWrapper OmSegmentation::getSDW() const {
	return SegmentationDataWrapper(GetID());
}

void OmSegmentation::Flush()
{
	segmentCache_->Flush();
	mst_->Flush();
	validGroupNum_->Save();
	mstUserEdges_->Save();
}

//TODO: make OmVolumeBuildBlank class, and move this in there (purcaro)
void OmSegmentation::BuildBlankVolume(const Vector3i& dims)
{
	SetBuildState(MIPVOL_BUILDING);

	Coords().SetDataDimensions(dims);
	Coords().UpdateRootLevel();

	OmVolumeAllocater::AllocateData(this, OmVolDataType::UINT32);

	SetBuildState(MIPVOL_BUILT);
}

OmMipMeshManager* OmSegmentation::MeshManager(const double threshold){
	return meshManagers_->GetManager(threshold);
}

quint32 OmSegmentation::GetVoxelValue(const DataCoord & vox)
{
	if (!ContainsVoxel(vox))
		return 0;

	//find mip_coord and offset
	OmChunkCoord leaf_mip_coord = coords_.DataToMipCoord(vox, 0);

	OmSegChunkPtr chunk;
	GetChunk(chunk, leaf_mip_coord);

	//get voxel data
	return chunk->GetVoxelValue(vox);
}

void OmSegmentation::SetVoxelValue(const DataCoord& vox, const uint32_t val)
{
	if (!ContainsVoxel(vox))
		return;

	//find mip_coord and offset
	OmChunkCoord leaf_mip_coord = coords_.DataToMipCoord(vox, 0);

	OmSegChunkPtr chunk;
	GetChunk(chunk, leaf_mip_coord);

	chunk->SetVoxelValue(vox, val);
}

void OmSegmentation::GetChunk(OmChunkPtr& ptr, const OmChunkCoord& coord)
{
	OmSegChunkPtr seg;
	chunkCache_->Get(seg, coord);
	ptr = seg;
}

void OmSegmentation::GetChunk(OmSegChunkPtr& ptr, const OmChunkCoord& coord){
	chunkCache_->Get(ptr, coord);
}
