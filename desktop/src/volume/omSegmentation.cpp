#include "annotation/annotation.h"
#include "chunks/omChunkCache.hpp"
#include "chunks/omSegChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "common/common.h"
#include "common/logging.h"
#include "datalayer/omDataPaths.h"
#include "mesh/drawer/omMeshDrawer.h"
#include "mesh/omMeshManagers.hpp"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "system/omGroups.h"
#include "tiles/cache/omTileCacheSegmentation.hpp"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationLoader.h"
#include "volume/omUpdateBoundingBoxes.h"
#include "chunks/uniqueValuesDSH.hpp"

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
    : loader_(new om::segmentation::loader(this)),
      uniqueChunkValues_(new OmChunkUniqueValuesManager(this)),
      groups_(new OmGroups(this)),
      mst_(new OmMST(this)),
      meshDrawer_(new OmMeshDrawer(this)),
      meshManagers_(new OmMeshManagers(this)),
      chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this)),
      segments_(new OmSegments(this)),
      segmentLists_(new OmSegmentLists()),
      mstUserEdges_(new OmUserEdges(this)),
      validGroupNum_(new OmValidGroupNum(this)),
      volData_(new OmVolumeData()),
      volSliceCache_(new OmRawSegTileCache(this)),
      tileCache_(new OmTileCacheSegmentation()),
      annotations_(new om::annotation::manager(this)),
      uniqueValuesDS_(new om::chunk::CachedUniqueValuesDataSource(this))
 {}

// used by OmGenericManager
OmSegmentation::OmSegmentation(om::common::ID id)
    : OmManageableObject(id),
      loader_(new om::segmentation::loader(this)),
      uniqueChunkValues_(new OmChunkUniqueValuesManager(this)),
      groups_(new OmGroups(this)),
      mst_(new OmMST(this)),
      meshDrawer_(new OmMeshDrawer(this)),
      meshManagers_(new OmMeshManagers(this)),
      chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this)),
      segments_(new OmSegments(this)),
      segmentLists_(new OmSegmentLists()),
      mstUserEdges_(new OmUserEdges(this)),
      validGroupNum_(new OmValidGroupNum(this)),
      volData_(new OmVolumeData()),
      volSliceCache_(new OmRawSegTileCache(this)),
      tileCache_(new OmTileCacheSegmentation()),
      annotations_(new om::annotation::manager(this)),
      uniqueValuesDS_(new om::chunk::CachedUniqueValuesDataSource(this))
 {
  LoadPath();

  segments_->StartCaches();
  segments_->refreshTree();
}

OmSegmentation::~OmSegmentation() {}

void OmSegmentation::LoadPath() {
  folder_.reset(new om::segmentation::folder(this));
}

bool OmSegmentation::LoadVolData() {
  if (IsBuilt()) {
    UpdateFromVolResize();
    volData_->load(this);
    volSliceCache_->Load();
    tileCache_->Load(this);
    annotations_->Load();
    return true;
  }

  return false;
}

std::string OmSegmentation::GetName() {
  return "segmentation" + om::string::num(GetID());
}

std::string OmSegmentation::GetNameHyphen() {
  return "segmentation-" + om::string::num(GetID());
}

void OmSegmentation::SetDendThreshold(const double t) {
  mst_->SetUserThreshold(t);
}

void OmSegmentation::SetSizeThreshold(const double t) {
  mst_->SetUserSizeThreshold(t);
}

void OmSegmentation::CloseDownThreads() { meshManagers_->CloseDownThreads(); }

bool OmSegmentation::LoadVolDataIfFoldersExist() {
  //assume level 0 data always present
  const QString path = OmFileNames::GetVolDataFolderPath(this, 0);

  if (QDir(path).exists()) {
    return LoadVolData();
  }

  return false;
}

double OmSegmentation::GetDendThreshold() { return mst_->UserThreshold(); }

double OmSegmentation::GetSizeThreshold() { return mst_->UserSizeThreshold(); }

void OmSegmentation::UpdateVoxelBoundingData() {
  OmUpdateBoundingBoxes updater(this);
  updater.Update();
}

int OmSegmentation::GetBytesPerVoxel() const {
  return volData_->GetBytesPerVoxel();
}

int OmSegmentation::GetBytesPerSlice() const {
  return GetBytesPerVoxel() * coords_.GetChunkDimension() *
         coords_.GetChunkDimension();
}

void OmSegmentation::SetVolDataType(const OmVolDataType type) {
  mVolDataType = type;
  volData_->SetDataType(this);
}

SegmentationDataWrapper OmSegmentation::GetSDW() const {
  return SegmentationDataWrapper(GetID());
}

void OmSegmentation::Flush() {
  folder_->MakeUserSegmentsFolder();

  segments_->Flush();
  mst_->Flush();
  validGroupNum_->Save();
  mstUserEdges_->Save();
  annotations_->Save();
}

//TODO: make OmVolumeBuildBlank class, and move this in there (purcaro)
void OmSegmentation::BuildBlankVolume(const Vector3i& dims) {
  SetBuildState(MIPVOL_BUILDING);

  Coords().SetDataDimensions(dims);
  Coords().UpdateRootLevel();

  OmVolumeAllocater::AllocateData(this, OmVolDataType::UINT32);

  SetBuildState(MIPVOL_BUILT);
}

OmMeshManager* OmSegmentation::MeshManager(const double threshold) {
  return meshManagers_->GetManager(threshold);
}

quint32 OmSegmentation::GetVoxelValue(const om::globalCoord& vox) {
  if (!ContainsVoxel(vox)) {
    return 0;
  }

  //find mip_coord and offset
  const om::chunkCoord mip0coord = vox.toChunkCoord(this, 0);

  OmSegChunk* chunk = GetChunk(mip0coord);

  //get voxel data
  return chunk->GetVoxelValue(vox.toDataCoord(this, 0));
}

void OmSegmentation::SetVoxelValue(const om::globalCoord& vox,
                                   const uint32_t val) {
  if (!ContainsVoxel(vox)) return;

  for (int level = 0; level <= coords_.GetRootMipLevel(); level++) {
    om::chunkCoord leaf_mip_coord = vox.toChunkCoord(this, level);
    OmSegChunk* chunk = GetChunk(leaf_mip_coord);
    chunk->SetVoxelValue(vox.toDataCoord(this, level), val);
  }
}

bool OmSegmentation::SetVoxelValueIfSelected(const om::globalCoord& vox,
                                             const uint32_t val) {
  const om::common::SegIDSet selection = Segments()->GetSelectedSegmentIDs();
  if (selection.size() > 0) {
    om::chunkCoord leaf_mip_coord = vox.toChunkCoord(this, 0);
    OmSegChunk* chunk = GetChunk(leaf_mip_coord);
    om::common::SegID target =
        Segments()->findRootID(chunk->GetVoxelValue(vox.toDataCoord(this, 0)));

    if (selection.count(target) == 0) {
      return false;
    }
  }

  SetVoxelValue(vox, val);
  return true;
}

OmSegChunk* OmSegmentation::GetChunk(const om::chunkCoord& coord) {
  return chunkCache_->GetChunk(coord);
}

void OmSegmentation::UpdateFromVolResize() {
  chunkCache_->UpdateFromVolResize();
  uniqueChunkValues_->UpdateFromVolResize();
}

std::string OmSegmentation::GetDirectoryPath() const {
  return folder_->RelativeVolPath().toStdString();
}
