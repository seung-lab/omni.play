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

class OmTileCacheChannel;
class OmVolumeData;
template <typename, typename>
class OmChunkCache;

namespace om {
namespace channel {
class folder;
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

  OmChunk* GetChunk(const om::coords::Chunk& coord);

  inline std::vector<OmFilter2d*> GetFilters() const {
    return filterManager_.GetFilters();
  }

  OmChunkCache<OmChannelImpl, OmChunk>* ChunkCache() {
    return chunkCache_.get();
  }

  inline OmTileCacheChannel& TileCache() { return *tileCache_; }

 protected:
  // protected copy constructor and assignment operator to prevent copy
  OmChannelImpl(const OmChannelImpl&);
  OmChannelImpl& operator=(const OmChannelImpl&);

  std::unique_ptr<OmChunkCache<OmChannelImpl, OmChunk> > chunkCache_;
  std::unique_ptr<OmVolumeData> volData_;
  std::unique_ptr<OmTileCacheChannel> tileCache_;

  om::file::Paths::Vol paths_;

  OmFilter2dManager filterManager_;

 private:
  friend class OmChannelImplChunkBuildTask;
  friend class OmDataArchiveProjectImpl;
};
