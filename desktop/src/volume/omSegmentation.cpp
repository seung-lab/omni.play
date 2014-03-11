#include "annotation/annotation.h"
#include "chunks/omChunkCache.hpp"
#include "chunks/omSegChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "common/common.h"
#include "common/logging.h"
#include "datalayer/omDataPaths.h"
#include "mesh/drawer/omMeshDrawer.h"
#include "mesh/omMeshManagers.hpp"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "tiles/cache/omTileCacheSegmentation.hpp"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationLoader.h"
#include "volume/omUpdateBoundingBoxes.h"
#include "actions/omActions.h"
#include "volume/segmentation.h"

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
    : loader_(new om::segmentation::loader(this)),
      uniqueChunkValues_(new OmChunkUniqueValuesManager(this)),
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
      annotations_(new om::annotation::manager(this)) {}

// used by OmGenericManager
OmSegmentation::OmSegmentation(om::common::ID id)
    : OmManageableObject(id),
      loader_(new om::segmentation::loader(this)),
      uniqueChunkValues_(new OmChunkUniqueValuesManager(this)),
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
      annotations_(new om::annotation::manager(this)) {
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

om::segment::EdgeVector* OmSegmentation::MST() {
  if (!mst_) {
    mst_.reset(new om::segment::EdgeVector(OmProject::Paths().UserMST(
        OmProject::Globals().Users().CurrentUser(), GetID())));
  }
  return mst_.get();
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
  // assume level 0 data always present
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
  return GetBytesPerVoxel() * coords_.ChunkDimensions().x *
         coords_.ChunkDimensions().y;
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

// TODO: make OmVolumeBuildBlank class, and move this in there (purcaro)
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

quint32 OmSegmentation::GetVoxelValue(const om::coords::Global& vox) {
  if (!ContainsVoxel(vox)) {
    return 0;
  }

  // find mip_coord and offset
  const om::coords::Chunk mip0coord = vox.ToChunk(Coords(), 0);

  OmSegChunk* chunk = GetChunk(mip0coord);

  // get voxel data
  return chunk->GetVoxelValue(vox.ToData(Coords(), 0));
}

void OmSegmentation::SetVoxelValue(const om::coords::Global& vox,
                                   const uint32_t val) {
  if (!ContainsVoxel(vox)) return;

  for (int level = 0; level <= coords_.RootMipLevel(); level++) {
    om::coords::Chunk leaf_mip_coord = vox.ToChunk(Coords(), level);
    OmSegChunk* chunk = GetChunk(leaf_mip_coord);
    chunk->SetVoxelValue(vox.ToData(Coords(), level), val);
  }
}

bool OmSegmentation::SetVoxelValueIfSelected(const om::coords::Global& vox,
                                             const uint32_t val) {
  const om::common::SegIDSet selection = Segments()->GetSelectedSegmentIDs();
  if (selection.size() > 0) {
    om::coords::Chunk leaf_mip_coord = vox.ToChunk(Coords(), 0);
    OmSegChunk* chunk = GetChunk(leaf_mip_coord);
    om::common::SegID target =
        Segments()->findRootID(chunk->GetVoxelValue(vox.ToData(Coords(), 0)));

    if (selection.count(target) == 0) {
      return false;
    }
  }

  SetVoxelValue(vox, val);
  return true;
}

OmSegChunk* OmSegmentation::GetChunk(const om::coords::Chunk& coord) {
  return chunkCache_->GetChunk(coord);
}

void OmSegmentation::UpdateFromVolResize() {
  chunkCache_->UpdateFromVolResize();
  uniqueChunkValues_->UpdateFromVolResize();
}

std::string OmSegmentation::GetDirectoryPath() const {
  return folder_->RelativeVolPath().toStdString();
}

void OmSegmentation::ClearUserChangesAndSave() {
  OmMSTEdge* edges = MST()->Edges();
  for (uint32_t i = 0; i < MST()->NumEdges(); ++i) {
    edges[i].userSplit = 0;
    edges[i].userJoin = 0;
  }

  segments_->ClearUserEdges();

  OmSegments* segments = Segments();
  for (om::common::SegID i = 1; i <= segments->getMaxValue(); ++i) {
    OmSegment* seg = segments->GetSegment(i);
    if (!seg) {
      continue;
    }
    seg->RandomizeColor();
    seg->SetListType(om::common::SegListType::WORKING);
  }
  ValidGroupNum()->Clear();

  SetDendThreshold(OmMST::DefaultThreshold);
  om::event::RefreshMSTthreshold();
  om::event::SegmentModified();

  OmCacheManager::TouchFreshness();
  OmProject::Save();
}
