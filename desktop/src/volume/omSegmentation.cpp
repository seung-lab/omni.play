#include "annotation/annotation.h"
#include "chunks/omChunkCache.hpp"
#include "chunks/omSegChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "common/common.h"
#include "common/logging.h"
#include "datalayer/omDataPaths.h"
#include "mesh/drawer/omMeshDrawer.h"
#include "mesh/omMeshManagers.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "tiles/cache/omTileCacheSegmentation.hpp"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "volume/omUpdateBoundingBoxes.h"
#include "actions/omActions.h"
#include "volume/metadataDataSource.hpp"
#include "volume/metadataManager.h"
#include "segment/fileDataSource.hpp"
#include "segment/dataSources.hpp"
#include "segment/userEdgeVector.hpp"
#include "segment/selection.hpp"
#include "users/omUsers.h"
using namespace om;

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
    : uniqueChunkValues_(new OmChunkUniqueValuesManager(this)),
      meshDrawer_(new OmMeshDrawer(this)),
      meshManagers_(new OmMeshManagers(this)),
      chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this)),
      volData_(new OmVolumeData()),
      volSliceCache_(new OmRawSegTileCache(this)),
      tileCache_(new OmTileCacheSegmentation()),
      annotations_(new annotation::manager(this)) {}

// used by OmGenericManager
OmSegmentation::OmSegmentation(common::ID id)
    : OmManageableObject(id),
      uniqueChunkValues_(new OmChunkUniqueValuesManager(this)),
      meshDrawer_(new OmMeshDrawer(this)),
      meshManagers_(new OmMeshManagers(this)),
      chunkCache_(new OmChunkCache<OmSegmentation, OmSegChunk>(this)),
      volData_(new OmVolumeData()),
      volSliceCache_(new OmRawSegTileCache(this)),
      tileCache_(new OmTileCacheSegmentation()),
      annotations_(new annotation::manager(this)) {}

OmSegmentation::~OmSegmentation() {}

void OmSegmentation::LoadPath() {
  log_debugs << "Loading segmentation " << id_;
  const auto& p = OmProject::Paths();
  const auto& username = OmProject::Globals().Users().CurrentUser();
  paths_ = p.SegmentationPaths(id_);
  auto userPaths = p.UserPaths(username);

  validGroupNum_.reset(new OmValidGroupNum(paths_.ValidGroupNum()));

  metaDS_.reset(new om::volume::MetadataDataSource());
  metaManager_.reset(
      new om::volume::MetadataManager(*metaDS_, p.Segmentation(id_)));

  segDataDS_.reset(new segment::FileDataSource(userPaths.Segments(id_)));
  segData_.reset(new segment::SegDataVector(*segDataDS_, segment::PageSize,
                                            Metadata().numSegments() + 1));

  segListDataDS_.reset(
      new segment::ListTypeFileDataSource(userPaths.Segments(id_)));
  segListData_.reset(new segment::SegListDataVector(
      *segListDataDS_, segment::PageSize, Metadata().numSegments() + 1));

  mst_.reset(new segment::EdgeVector(userPaths.MST(id_)));
  userEdges_.reset(new segment::UserEdgeVector(userPaths.UserEdges(id_)));
  segments_.reset(new OmSegments(this));
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
  return "segmentation" + string::num(GetID());
}

std::string OmSegmentation::GetNameHyphen() {
  return "segmentation-" + string::num(GetID());
}

void OmSegmentation::SetDendThreshold(const double t) {
  // TODO: lock
  OmProject::Globals().Users().UserSettings().setThreshold(t);
  segments_->refreshTree();
}

void OmSegmentation::SetSizeThreshold(const double t) {
  // TODO: lock
  OmProject::Globals().Users().UserSettings().setSizeThreshold(t);
  segments_->refreshTree();
}

double OmSegmentation::GetDendThreshold() {
  return OmProject::Globals().Users().UserSettings().getThreshold();
}

double OmSegmentation::GetSizeThreshold() {
  return OmProject::Globals().Users().UserSettings().getSizeThreshold();
}

void OmSegmentation::CloseDownThreads() { meshManagers_->CloseDownThreads(); }

bool OmSegmentation::LoadVolDataIfFoldersExist() {
  if (om::file::exists(paths_.Data(0, getVolDataType()))) {
    return LoadVolData();
  }

  return false;
}

void OmSegmentation::UpdateVoxelBoundingData() {
  OmUpdateBoundingBoxes updater(this);
  updater.Update();
}

int OmSegmentation::GetBytesPerVoxel() const {
  return volData_->GetBytesPerVoxel();
}

int OmSegmentation::GetBytesPerSlice() const {
  return GetBytesPerVoxel() * coords_.ChunkDimensions().x *
         coords_.ChunkDimensions().y;
}

void OmSegmentation::SetVolDataType(const om::common::DataType type) {
  mVolDataType = type;
  volData_->SetDataType(this);
}

SegmentationDataWrapper OmSegmentation::GetSDW() const {
  return SegmentationDataWrapper(GetID());
}

void OmSegmentation::Flush() {

  segments_->Flush();
  mst_->flush();
  validGroupNum_->Save();
  userEdges_->flush();
  annotations_->Save();
}

// TODO: make OmVolumeBuildBlank class, and move this in there (purcaro)
void OmSegmentation::BuildBlankVolume(const Vector3i& dims) {
  SetBuildState(MIPVOL_BUILDING);

  Coords().SetDataDimensions(dims);
  Coords().UpdateRootLevel();

  OmVolumeAllocater::AllocateData(this, om::common::DataType::UINT32);

  SetBuildState(MIPVOL_BUILT);
}

OmMeshManager* OmSegmentation::MeshManager(const double threshold) {
  return meshManagers_->GetManager(threshold);
}

quint32 OmSegmentation::GetVoxelValue(const coords::Global& vox) {
  if (!ContainsVoxel(vox)) {
    return 0;
  }

  // find mip_coord and offset
  const coords::Chunk mip0coord = vox.ToChunk(Coords(), 0);

  OmSegChunk* chunk = GetChunk(mip0coord);

  // get voxel data
  return chunk->GetVoxelValue(vox.ToData(Coords(), 0));
}

void OmSegmentation::SetVoxelValue(const coords::Global& vox,
                                   const uint32_t val) {
  if (!ContainsVoxel(vox)) return;

  for (int level = 0; level <= coords_.RootMipLevel(); level++) {
    coords::Chunk leaf_mip_coord = vox.ToChunk(Coords(), level);
    OmSegChunk* chunk = GetChunk(leaf_mip_coord);
    chunk->SetVoxelValue(vox.ToData(Coords(), level), val);
  }
}

bool OmSegmentation::SetVoxelValueIfSelected(const coords::Global& vox,
                                             const uint32_t val) {
  const common::SegIDSet selection =
      Segments().Selection().GetSelectedSegmentIDs();
  if (selection.size() > 0) {
    coords::Chunk leaf_mip_coord = vox.ToChunk(Coords(), 0);
    OmSegChunk* chunk = GetChunk(leaf_mip_coord);
    common::SegID target =
        Segments().FindRootID(chunk->GetVoxelValue(vox.ToData(Coords(), 0)));

    if (selection.count(target) == 0) {
      return false;
    }
  }

  SetVoxelValue(vox, val);
  return true;
}

OmSegChunk* OmSegmentation::GetChunk(const coords::Chunk& coord) {
  return chunkCache_->GetChunk(coord);
}

void OmSegmentation::UpdateFromVolResize() {
  chunkCache_->UpdateFromVolResize();
  uniqueChunkValues_->UpdateFromVolResize();
}

void OmSegmentation::ClearUserChangesAndSave() {
  for (uint32_t i = 0; i < MST().size(); ++i) {
    MST()[i].userSplit = 0;
    MST()[i].userJoin = 0;
  }

  UserEdges().clear();

  for (common::SegID i = 1; i <= Segments().maxValue(); ++i) {
    OmSegment* seg = Segments().GetSegment(i);
    if (!seg) {
      continue;
    }
    seg->RandomizeColor();
    seg->SetListType(common::SegListType::WORKING);
  }
  ValidGroupNum().Clear();

  SetDendThreshold(1);
  event::RefreshMSTthreshold();
  event::SegmentModified();

  OmCacheManager::TouchFreshness();
  OmProject::Save();
}
