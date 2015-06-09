#include "volume/omChannel.h"
#include "volume/io/omVolumeData.h"
#include "chunk/cachedDataSource.hpp"
#include "tile/cachedDataSource.hpp"


/////////////////// Public methods
OmChannel::OmChannel() : volData_(new OmVolumeData()), tileCache_(new OmTileCacheChannel()) {}

OmChannel::OmChannel(om::common::ID id)  : OmManageableObject(id),
      volData_(new OmVolumeData()),
      tileCache_(new OmTileCacheChannel()) {
  filterManager_.AddFilter();
  paths_ = OmProject::Paths().Channel(id);
}

OmChannel::~OmChannel() {}

std::string OmChannel::GetName() const {
  return "channel" + om::string::num(GetID());
}

std::string OmChannel::GetNameHyphen() const {
  return "channel-" + om::string::num(GetID());
}


void OmChannel::LoadPath() {
  chunkDS_.reset(
      new om::chunk::CachedDataSource(paths_, getVolDataType(), coords_));
  tileDS_.reset(new om::tile::CachedDataSource(*chunkDS_, coords_));
}

void OmChannel::SetVolDataType(const om::common::DataType type) {
  mVolDataType = type;
  volData_->SetDataType(this);
}

bool OmChannel::LoadVolData() {
  if (IsBuilt()) {

    volData_->load(this);
    tileCache_->Load(this);
    return true;
  }

  return false;
}


bool OmChannel::LoadVolDataIfFoldersExist() {
  if (om::file::exists(paths_)) {
    return LoadVolData();
  }

  return false;
}


unsigned int OmChannel::GetBytesPerVoxel() const {
  return volData_->GetBytesPerVoxel();
}

unsigned int OmChannel::GetBytesPerSlice() const {
  return GetBytesPerVoxel() * 128 * 128;
}


std::shared_ptr<om::chunk::ChunkVar> OmChannel::GetChunk(
    const om::coords::Chunk& coord) {
  return chunkDS_->Get(coord);
}

std::shared_ptr<om::tile::TileVar> OmChannel::GetTile(
    const om::coords::Tile& coord) {
  return tileDS_->Get(coord);
}

om::chunk::ChunkDS& OmChannel::ChunkDS() const { return *chunkDS_; }
om::tile::TileDS& OmChannel::TileDS() const { return *tileDS_; }
