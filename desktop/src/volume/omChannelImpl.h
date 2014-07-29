#pragma once
#include "precomp.h"

/*
 * OmChannel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "system/omManageableObject.h"
#include "volume/omFilter2dManager.h"
#include "volume/omMipVolume.h"
#include "chunk/dataSources.hpp"
#include "tile/dataSources.hpp"
#include "volume/iterators.hpp"

class OmTileCacheChannel;
class OmVolumeData;

namespace om {
namespace channel {
class folder;
}
namespace chunk {
class CachedDataSource;
}
namespace tile {
class CachedDataSource;
}
}

class OmChannelImpl : public OmMipVolume, public OmManageableObject {

 public:
  OmChannelImpl();
  OmChannelImpl(om::common::ID id);
  virtual ~OmChannelImpl();

  virtual QString GetDefaultHDF5DatasetName() = 0;

  OmVolumeData& VolData() { return *volData_; }

  std::string GetName();
  std::string GetNameHyphen();
  const om::file::Paths::Vol& VolPaths() const { return paths_; }
  void LoadPath();

  bool LoadVolData();
  bool LoadVolDataIfFoldersExist();
  void UpdateFromVolResize();

  om::common::ObjectType getVolumeType() const { return om::common::CHANNEL; }

  om::common::ID GetID() const { return id_; }

  virtual int GetBytesPerVoxel() const;
  virtual int GetBytesPerSlice() const;

  void CloseDownThreads();

  OmFilter2dManager& FilterManager() { return filterManager_; }

  void SetVolDataType(const om::common::DataType);

  om::chunk::ChunkDS& ChunkDS() const override;
  om::tile::TileDS& TileDS() const override;
  std::shared_ptr<om::chunk::ChunkVar> GetChunk(const om::coords::Chunk& coord);
  std::shared_ptr<om::tile::TileVar> GetTile(const om::coords::Tile& coord);

  inline std::vector<OmFilter2d*> GetFilters() const {
    return filterManager_.GetFilters();
  }

  inline OmTileCacheChannel& TileCache() { return *tileCache_; }

 protected:
  // protected copy constructor and assignment operator to prevent copy
  OmChannelImpl(const OmChannelImpl&);
  OmChannelImpl& operator=(const OmChannelImpl&);

  std::unique_ptr<OmVolumeData> volData_;
  std::unique_ptr<OmTileCacheChannel> tileCache_;
  std::unique_ptr<om::chunk::CachedDataSource> chunkDS_;
  std::unique_ptr<om::tile::CachedDataSource> tileDS_;

  om::file::Paths::Vol paths_;

  OmFilter2dManager filterManager_;

 private:
  friend class OmChannelImplChunkBuildTask;
  friend class OmDataArchiveProjectImpl;
};
