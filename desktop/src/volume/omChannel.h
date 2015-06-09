#pragma once
#include "precomp.h"

/*
 * OmChannel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "volume/omMipVolume.h"
#include "system/omManageableObject.h"
#include "datalayer/archive/channel.h"
#include "volume/omFilter2dManager.h"
#include "chunk/dataSources.hpp"
#include "tile/dataSources.hpp"
#include "volume/iterators.hpp"
#include "tiles/cache/omTileCacheChannel.hpp"
#include "actions/omActions.h"
#include "common/common.h"
#include "common/logging.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "threads/taskManager.hpp"

class OmFilter2d;
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


class OmChannel : public OmMipVolume, public OmManageableObject {

 public:
  OmChannel();
  OmChannel(om::common::ID id);
  virtual ~OmChannel();

  virtual QString GetDefaultHDF5DatasetName() { return "chanSingle"; }
  std::string GetName() const;
  std::string GetNameHyphen() const;
  om::common::ObjectType getVolumeType() const { return om::common::CHANNEL; }
  om::common::ID GetID() const { return id_; }

  const om::file::Paths::Vol& VolPaths() const { return paths_; }
  OmVolumeData& VolData() { return *volData_; }
  void SetVolDataType(const om::common::DataType);


  void LoadPath();
  bool LoadVolData();
  bool LoadVolDataIfFoldersExist();

  unsigned int GetBytesPerVoxel() const;
  unsigned int GetBytesPerSlice() const;


  om::chunk::ChunkDS& ChunkDS() const override;
  om::tile::TileDS& TileDS() const override;
  std::shared_ptr<om::chunk::ChunkVar> GetChunk(const om::coords::Chunk& coord);
  std::shared_ptr<om::tile::TileVar> GetTile(const om::coords::Tile& coord);


  OmFilter2dManager& FilterManager() { return filterManager_; }
  inline std::vector<OmFilter2d*> GetFilters() const {
    return filterManager_.GetFilters();
  }
  inline OmTileCacheChannel& TileCache() { return *tileCache_; }

 protected:
  // protected copy constructor and assignment operator to prevent copy
  OmChannel(const OmChannel&);
  OmChannel& operator=(const OmChannel&);

  std::unique_ptr<OmVolumeData> volData_;
  std::unique_ptr<OmTileCacheChannel> tileCache_;
  std::unique_ptr<om::chunk::CachedDataSource> chunkDS_;
  std::unique_ptr<om::tile::CachedDataSource> tileDS_;

  om::file::Paths::Vol paths_;

  OmFilter2dManager filterManager_;

 private:
  friend class OmChannelImplChunkBuildTask;
  friend class OmDataArchiveProjectImpl;


 private:
  friend class OmBuildChannel;
  template <class T>
  friend class OmVolumeBuilder;
  template <class T>
  friend class OmVolumeBuilderHdf5;
  template <class T>
  friend class OmVolumeBuilderImages;
  template <class T>
  friend class OmVolumeImporter;

  friend QDataStream& operator<<(QDataStream& out, const OmChannel&);
  friend QDataStream& operator>>(QDataStream& in, OmChannel&);
  friend class YAML::convert<OmChannel>;
};
