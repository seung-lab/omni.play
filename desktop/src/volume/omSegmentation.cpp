#include "annotation/annotation.h"
#include "common/common.h"
#include "common/logging.h"
#include "datalayer/omDataPaths.h"
#include "view3d/mesh/omMeshManagers.hpp"
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
#include "chunk/cachedDataSource.hpp"
#include "chunk/cachedUniqueValuesDataSource.hpp"
#include "tile/cachedDataSource.hpp"
#include "chunk/voxelGetter.hpp"

using namespace om;

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
    : meshManagers_(new OmMeshManagers(this)),
      volData_(new OmVolumeData()),
      volSliceCache_(new OmRawSegTileCache(this)),
      tileCache_(new OmTileCacheSegmentation()),
      annotations_(new annotation::manager(this)) {}

// used by OmGenericManager
OmSegmentation::OmSegmentation(common::ID id)
    : OmManageableObject(id),
      meshManagers_(new OmMeshManagers(this)),
      volData_(new OmVolumeData()),
      volSliceCache_(new OmRawSegTileCache(this)),
      tileCache_(new OmTileCacheSegmentation()),
      annotations_(new annotation::manager(this)) {
}

OmSegmentation::~OmSegmentation() {}

void OmSegmentation::LoadPath(const bool newSegmentation) {
  log_debugs << "Loading segmentation " << id_;
  const auto& p = OmProject::Paths();
  const auto& username = OmProject::Globals().Users().CurrentUser();
  paths_ = p.SegmentationPaths(id_);
  auto userPaths = p.UserPaths(username);

  metaDS_.reset( new om::volume::MetadataDataSource());
  metaManager_.reset( new om::volume::MetadataManager(*metaDS_, p.Segmentation(id_)) );

  if(newSegmentation) {
    Metadata().New();
    Metadata().set_maxSegments(0);
    Metadata().set_numSegments(0);

    om::file::MkDir(p.SegmentationPaths(id_));
  }
  validGroupNum_.reset(new OmValidGroupNum(userPaths.ValidGroupNum(id_),
                                           Metadata().maxSegments()));

  segDataDS_.reset(new segment::FileDataSource(userPaths.Segments(id_)));
  segListDataDS_.reset(new segment::ListTypeFileDataSource(userPaths.Segments(id_)));

  if(newSegmentation){
    segData_.reset(new segment::SegDataVector(*segDataDS_, segment::PageSize, 0));
    segListData_.reset(new segment::SegListDataVector(*segListDataDS_, segment::PageSize, 0));
  }else{
    segData_.reset(new segment::SegDataVector(*segDataDS_, segment::PageSize, Metadata().maxSegments()+1));
    segListData_.reset(new segment::SegListDataVector(*segListDataDS_, segment::PageSize, Metadata().maxSegments()+1));
  }

  chunkDS_.reset(new chunk::CachedDataSource(paths_, getVolDataType(), coords_));

  tileDS_.reset(new om::tile::CachedDataSource(*chunkDS_, coords_));
  uniqueChunkValues_.reset(new chunk::CachedUniqueValuesDataSource(paths_));

  mst_.reset(new segment::EdgeVector(userPaths.MST(id_)));
  for (auto& edge : *mst_) {
    edge.wasJoined = 0;
  }
  userEdges_.reset(new segment::UserEdgeVector(userPaths.UserEdges(id_)));
  segments_.reset(new OmSegments(this));
}

void OmSegmentation::LoadChunks()
{
  chunkDS_.reset(
        new chunk::CachedDataSource(paths_, getVolDataType(), coords_));
}

bool OmSegmentation::LoadVolData() {
  if (IsBuilt()) {
    UpdateFromVolResize();
    volSliceCache_->Load();
    tileCache_->Load(this);
    annotations_->Load();
    return true;
  }

  return false;
}

std::string OmSegmentation::GetName() const {
  return "segmentation" + string::num(GetID());
}

std::string OmSegmentation::GetNameHyphen() const {
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

void OmSegmentation::SetASThreshold(const double t){
  // TODO: lock
  OmProject::Globals().Users().UserSettings().setASThreshold(t);
  segments_->refreshTree();
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

double OmSegmentation::GetASThreshold() {
  return OmProject::Globals().Users().UserSettings().getASThreshold();
}

void OmSegmentation::UpdateVoxelBoundingData() {
  OmUpdateBoundingBoxes updater(this);
  updater.Update();
}

unsigned int OmSegmentation::GetBytesPerVoxel() const {
  return volData_->GetBytesPerVoxel();
}

unsigned int OmSegmentation::GetBytesPerSlice() const {
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

uint32_t OmSegmentation::GetVoxelValue(const coords::Global& vox) {
  om::chunk::Voxels<uint32_t> v(*chunkDS_, Coords());
  return v.GetValue(vox);
}

void OmSegmentation::SetVoxelValue(const coords::Global& vox,
                                   const uint32_t val) {
  if (!ContainsVoxel(vox)) return;

  for (int level = 0; level <= coords_.RootMipLevel(); level++) {
    coords::Chunk mipCoord = vox.ToChunk(Coords(), level);
    auto chunk = GetChunk(mipCoord);
    auto typedChunk = boost::get<om::chunk::Chunk<uint32_t>>(chunk.get());
    if (!typedChunk) {
      continue;
    }
    auto coord = vox.ToData(Coords(), level);
    auto idx = coord.ToChunkOffset();
    (*typedChunk)[idx] = val;
    chunkDS_->Put(mipCoord, chunk);
  }
}

bool OmSegmentation::SetVoxelValueIfSelected(const coords::Global& vox,
                                             const uint32_t val) {
  const common::SegIDSet selection =
      Segments().Selection().GetSelectedSegmentIDs();
  if (selection.count(GetVoxelValue(vox)) == 0) {
    return false;
  }

  SetVoxelValue(vox, val);
  return true;
}

std::shared_ptr<om::chunk::ChunkVar> OmSegmentation::GetChunk(
    const coords::Chunk& coord) {
  return chunkDS_->Get(coord);
}

std::shared_ptr<om::tile::TileVar> OmSegmentation::GetTile(
    const coords::Tile& coord) {
  return tileDS_->Get(coord);
}

void OmSegmentation::InvalidateTiles(const om::coords::Chunk& coord) {
  tileDS_->Invalidate(coord);
}

void OmSegmentation::UpdateFromVolResize() {}

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

om::chunk::ChunkDS& OmSegmentation::ChunkDS() const { return *chunkDS_; }
om::tile::TileDS& OmSegmentation::TileDS() const { return *tileDS_; }
om::chunk::UniqueValuesDS& OmSegmentation::UniqueValuesDS() const {
  return *uniqueChunkValues_;
}
