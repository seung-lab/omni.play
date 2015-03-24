#include "tiles/cache/omTileCacheChannel.hpp"
#include "actions/omActions.h"
#include "common/common.h"
#include "common/logging.h"

#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "threads/taskManager.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omChannelImpl.h"
#include "volume/omFilter2d.h"
#include "chunk/cachedDataSource.hpp"
#include "tile/cachedDataSource.hpp"

OmChannelImpl::OmChannelImpl()
    : volData_(new OmVolumeData()), tileCache_(new OmTileCacheChannel()) {}

OmChannelImpl::OmChannelImpl(om::common::ID id)
    : OmManageableObject(id),
      volData_(new OmVolumeData()),
      tileCache_(new OmTileCacheChannel()) {
  filterManager_.AddFilter();
  paths_ = OmProject::Paths().Channel(id);
}

OmChannelImpl::~OmChannelImpl() {}

void OmChannelImpl::LoadPath() {
  chunkDS_.reset(
      new om::chunk::CachedDataSource(paths_, getVolDataType(), coords_));
  tileDS_.reset(new om::tile::CachedDataSource(*chunkDS_, coords_));
}

std::string OmChannelImpl::GetName() const {
  return "channel" + om::string::num(GetID());
}

std::string OmChannelImpl::GetNameHyphen() const {
  return "channel-" + om::string::num(GetID());
}

void OmChannelImpl::CloseDownThreads() {}

bool OmChannelImpl::LoadVolData() {
  if (IsBuilt()) {
    UpdateFromVolResize();
    volData_->load(this);
    tileCache_->Load(this);
    return true;
  }

  return false;
}

bool OmChannelImpl::LoadVolDataIfFoldersExist() {
  if (om::file::exists(paths_)) {
    return LoadVolData();
  }

  return false;
}

int OmChannelImpl::GetBytesPerVoxel() const {
  return volData_->GetBytesPerVoxel();
}

int OmChannelImpl::GetBytesPerSlice() const {
  return GetBytesPerVoxel() * 128 * 128;
}

void OmChannelImpl::SetVolDataType(const om::common::DataType type) {
  mVolDataType = type;
  volData_->SetDataType(this);
}

std::shared_ptr<om::chunk::ChunkVar> OmChannelImpl::GetChunk(
    const om::coords::Chunk& coord) {
  return chunkDS_->Get(coord);
}

std::shared_ptr<om::tile::TileVar> OmChannelImpl::GetTile(
    const om::coords::Tile& coord) {
  return tileDS_->Get(coord);
}

void OmChannelImpl::UpdateFromVolResize() {}

om::chunk::ChunkDS& OmChannelImpl::ChunkDS() const { return *chunkDS_; }
om::tile::TileDS& OmChannelImpl::TileDS() const { return *tileDS_; }
