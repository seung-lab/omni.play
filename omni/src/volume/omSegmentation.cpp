#include "chunks/omChunkCache.hpp"
#include "chunks/omSegChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPaths.h"
#include "mesh/drawer/omMeshDrawer.h"
#include "mesh/omMeshManagers.hpp"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "system/omGroups.h"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "volume/omUpdateBoundingBoxes.h"

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
    : uniqueChunkValues_(new OmChunkUniqueValuesManager(this))
    , groups_(new OmGroups(this))
    , mst_(new OmMST(this))
    , meshDrawer_(new OmMeshDrawer(this))
    , meshManagers_(new OmMeshManagers(this))
    , chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this))
    , segments_(new OmSegments(this))
    , segmentLists_(new OmSegmentLists())
    , mstUserEdges_(new OmUserEdges(this))
    , validGroupNum_(new OmValidGroupNum(this))
    , volData_(new OmVolumeData())
    , volSliceCache_(new OmRawSegTileCache(this))
{}

// used by OmGenericManager
OmSegmentation::OmSegmentation(OmID id)
    : OmManageableObject(id)
    , uniqueChunkValues_(new OmChunkUniqueValuesManager(this))
    , groups_(new OmGroups(this))
    , mst_(new OmMST(this))
    , meshDrawer_(new OmMeshDrawer(this))
    , meshManagers_(new OmMeshManagers(this))
    , chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this))
    , segments_(new OmSegments(this))
    , segmentLists_(new OmSegmentLists())
    , mstUserEdges_(new OmUserEdges(this))
    , validGroupNum_(new OmValidGroupNum(this))
    , volData_(new OmVolumeData())
    , volSliceCache_(new OmRawSegTileCache(this))
{
    segments_->StartCaches();
    segments_->refreshTree();
}

OmSegmentation::~OmSegmentation()
{}

void OmSegmentation::loadVolData()
{
    if(IsBuilt())
    {
        UpdateFromVolResize();
        volData_->load(this);
        volSliceCache_->Load();
    }
}

std::string OmSegmentation::GetName(){
    return "segmentation" + om::string::num(GetID());
}

std::string OmSegmentation::GetNameHyphen(){
    return "segmentation-" + om::string::num(GetID());
}

std::string OmSegmentation::GetDirectoryPath(){
    return OmDataPaths::getDirectoryPath(this);
}

void OmSegmentation::SetDendThreshold(const double t){
    mst_->SetUserThreshold(t);
}

void OmSegmentation::CloseDownThreads() {
    meshManagers_->CloseDownThreads();
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

int OmSegmentation::GetBytesPerSlice() const {
    return GetBytesPerVoxel()*128*128;
}

void OmSegmentation::SetVolDataType(const OmVolDataType type)
{
    mVolDataType = type;
    volData_->SetDataType(this);
}

SegmentationDataWrapper OmSegmentation::GetSDW() const {
    return SegmentationDataWrapper(GetID());
}

void OmSegmentation::Flush()
{
    segments_->Flush();
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

OmMeshManager* OmSegmentation::MeshManager(const double threshold){
    return meshManagers_->GetManager(threshold);
}

quint32 OmSegmentation::GetVoxelValue(const DataCoord & vox)
{
    if(!ContainsVoxel(vox)){
        return 0;
    }

    //find mip_coord and offset
    const OmChunkCoord mip0coord = coords_.DataToMipCoord(vox, 0);

    OmSegChunk* chunk = GetChunk(mip0coord);

    //get voxel data
    return chunk->GetVoxelValue(vox);
}

void OmSegmentation::SetVoxelValue(const DataCoord& vox, const uint32_t val)
{
    if (!ContainsVoxel(vox))
        return;

    //find mip_coord and offset
    OmChunkCoord leaf_mip_coord = coords_.DataToMipCoord(vox, 0);

    OmSegChunk* chunk = GetChunk(leaf_mip_coord);

    chunk->SetVoxelValue(vox, val);
}

OmSegChunk* OmSegmentation::GetChunk(const OmChunkCoord& coord){
    return chunkCache_->GetChunk(coord);
}

void OmSegmentation::UpdateFromVolResize()
{
    chunkCache_->UpdateFromVolResize();
    uniqueChunkValues_->UpdateFromVolResize();
}
