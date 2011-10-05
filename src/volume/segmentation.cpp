#include "annotation/annotation.h"
#include "chunks/chunkCache.hpp"
#include "chunks/segChunk.h"
#include "chunks/uniqueValues/chunkUniqueValuesManager.hpp"
#include "common/common.h"
#include "common/omDebug.h"
#include "datalayer/dataPaths.h"
#include "mesh/drawer/meshDrawer.h"
#include "mesh/meshManagers.hpp"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/validGroupNum.hpp"
#include "segment/listssegmentLists.h"
#include "segment/segments.h"
#include "system/omGroups.h"
#include "tiles/cache/tileCacheSegmentation.hpp"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/io/volumeData.h"
#include "volume/segmentation.h"
#include "volume/segmentationLoader.h"
#include "volume/omUpdateBoundingBoxes.h"

// used by dataArchiveProject
segmentation::segmentation()
    : loader_(new om::segmentation::loader(this))
    , uniqueChunkValues_(new chunkUniqueValuesManager(this))
    , groups_(new OmGroups(this))
    , mst_(new OmMST(this))
    , meshDrawer_(new meshDrawer(this))
    , meshManagers_(new meshManagers(this))
    , chunkCache_(new chunkCache<segmentation, segChunk>(this))
    , segments_(new segments(this))
    , segmentLists_(new segmentLists())
    , mstUserEdges_(new OmUserEdges(this))
    , validGroupNum_(new OmValidGroupNum(this))
    , volData_(new volumeData())
    , volSliceCache_(new OmRawSegTileCache(this))
    , tileCache_(new tileCacheSegmentation())
    , annotations_(new om::annotation::manager(this))
{}

// used by genericManager
segmentation::segmentation(OmID id)
    : OmManageableObject(id)
    , loader_(new om::segmentation::loader(this))
    , uniqueChunkValues_(new chunkUniqueValuesManager(this))
    , groups_(new OmGroups(this))
    , mst_(new OmMST(this))
    , meshDrawer_(new meshDrawer(this))
    , meshManagers_(new meshManagers(this))
    , chunkCache_(new chunkCache<segmentation, segChunk>(this))
    , segments_(new segments(this))
    , segmentLists_(new segmentLists())
    , mstUserEdges_(new OmUserEdges(this))
    , validGroupNum_(new OmValidGroupNum(this))
    , volData_(new volumeData())
    , volSliceCache_(new OmRawSegTileCache(this))
    , tileCache_(new tileCacheSegmentation())
    , annotations_(new om::annotation::manager(this))
{
    LoadPath();

    segments_->StartCaches();
    segments_->refreshTree();
}

segmentation::~segmentation()
{}

void segmentation::LoadPath(){
    folder_.reset(new om::segmentation::folder(this));
}

bool segmentation::LoadVolData()
{
    if(IsBuilt())
    {
        UpdateFromVolResize();
        volData_->load(this);
        volSliceCache_->Load();
        tileCache_->Load(this);
        annotations_->Load();
        return true;
    }

    return false;
}

std::string segmentation::GetName(){
    return "segmentation" + om::string::num(GetID());
}

std::string segmentation::GetNameHyphen(){
    return "segmentation-" + om::string::num(GetID());
}

void segmentation::SetDendThreshold(const double t){
    mst_->SetUserThreshold(t);
}

void segmentation::SetSizeThreshold(const double t){
    mst_->SetUserSizeThreshold(t);
}

void segmentation::CloseDownThreads() {
    meshManagers_->CloseDownThreads();
}

bool segmentation::LoadVolDataIfFoldersExist()
{
    //assume level 0 data always present
    const QString path = OmFileNames::GetVolDataFolderPath(this, 0);

    if(QDir(path).exists()){
        return LoadVolData();
    }

    return false;
}

double segmentation::GetDendThreshold() {
    return mst_->UserThreshold();
}

double segmentation::GetSizeThreshold() {
    return mst_->UserSizeThreshold();
}

void segmentation::UpdateVoxelBoundingData()
{
    OmUpdateBoundingBoxes updater(this);
    updater.Update();
}

int segmentation::GetBytesPerVoxel() const {
    return volData_->GetBytesPerVoxel();
}

int segmentation::GetBytesPerSlice() const {
    return GetBytesPerVoxel()*coords_.GetChunkDimension()*coords_.GetChunkDimension();
}

void segmentation::SetVolDataType(const OmVolDataType type)
{
    mVolDataType = type;
    volData_->SetDataType(this);
}

SegmentationDataWrapper segmentation::GetSDW() const {
    return SegmentationDataWrapper(GetID());
}

void segmentation::Flush()
{
    folder_->MakeUserSegmentsFolder();

    segments_->Flush();
    mst_->Flush();
    validGroupNum_->Save();
    mstUserEdges_->Save();
    annotations_->Save();
}

//TODO: make OmVolumeBuildBlank class, and move this in there (purcaro)
void segmentation::BuildBlankVolume(const Vector3i& dims)
{
    SetBuildState(MIPVOL_BUILDING);

    Coords().SetDataDimensions(dims);
    Coords().UpdateRootLevel();

    OmVolumeAllocater::AllocateData(this, OmVolDataType::UINT32);

    SetBuildState(MIPVOL_BUILT);
}

meshManager* segmentation::MeshManager(const double threshold){
    return meshManagers_->GetManager(threshold);
}

quint32 segmentation::GetVoxelValue(const om::globalCoord & vox)
{
    if(!ContainsVoxel(vox)){
        return 0;
    }

    //find mip_coord and offset
    const om::chunkCoord mip0coord = vox.toChunkCoord(this, 0);

    segChunk* chunk = GetChunk(mip0coord);

    //get voxel data
    return chunk->GetVoxelValue(vox.toDataCoord(this, 0));
}

void segmentation::SetVoxelValue(const om::globalCoord& vox, const uint32_t val)
{
    if (!ContainsVoxel(vox))
        return;
    
    for(int level = 0; level <= coords_.GetRootMipLevel(); level++)
    {
        om::chunkCoord leaf_mip_coord = vox.toChunkCoord(this, level);
        segChunk* chunk = GetChunk(leaf_mip_coord);
        chunk->SetVoxelValue(vox.toDataCoord(this, level), val);
    }
}

bool segmentation::SetVoxelValueIfSelected(const om::globalCoord& vox, const uint32_t val)
{
    const segIdsSet selection = Segments()->GetSelectedSegmentIDs();
    if(selection.size() > 0)
    {
        om::chunkCoord leaf_mip_coord = vox.toChunkCoord(this, 0);
        segChunk* chunk = GetChunk(leaf_mip_coord);
        segId target = Segments()->findRootID(
            chunk->GetVoxelValue(vox.toDataCoord(this, 0)));
        
        if(selection.count(target) == 0) {
            return false;  
        }
    }
    
    SetVoxelValue(vox, val);
    return true;
}

segChunk* segmentation::GetChunk(const om::chunkCoord& coord){
    return chunkCache_->GetChunk(coord);
}

void segmentation::UpdateFromVolResize()
{
    chunkCache_->UpdateFromVolResize();
    uniqueChunkValues_->UpdateFromVolResize();
}

std::string segmentation::GetDirectoryPath() const {
    return folder_->RelativeVolPath().toStdString();
}
