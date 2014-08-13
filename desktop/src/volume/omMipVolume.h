#pragma once
#include "precomp.h"

/*
 *  OmMipVolume is composed of sub-volumes cubes call OmChunks.
 *
 *  Brett Warne - bwarne@mit.edu - 7/19/09
 */

#include "common/common.h"
#include "datalayer/paths.hpp"
#include "coordinates/chunk.h"
#include "coordinates/volumeSystem.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataWrapper.h"
#include "chunk/dataSources.hpp"
#include "tile/dataSources.hpp"
#include "volume/iterators.hpp"

class OmDataPath;
class OmHdf5;
class OmChunk;
class OmVolume;
class OmVolumeData;

namespace om {
namespace rebuilder {
class segmentation;
}
namespace data {
namespace archive {
template <typename VOL>
class mipVolume;
}
}
}

enum MipVolumeBuildState {
  MIPVOL_UNBUILT = 0,
  MIPVOL_BUILT,
  MIPVOL_BUILDING
};

class OmMipVolume {
 public:
  OmMipVolume();
  virtual ~OmMipVolume() {}

  std::string GetDirectoryPath() const {
    return ((om::file::path)VolPaths()).string();
  };

  void MakeVolFolder() const { om::file::MkDir(VolPaths()); }
  virtual const om::file::Paths::Vol& VolPaths() const = 0;
  virtual std::string GetName() const = 0;
  virtual om::common::ObjectType getVolumeType() const = 0;
  virtual om::common::ID GetID() const = 0;

  inline bool IsBuilt() { return MIPVOL_BUILT == mBuildState; }
  inline bool built() { return MIPVOL_BUILT == mBuildState; }

  std::shared_ptr<std::deque<om::coords::Chunk> > GetMipChunkCoords() const;
  std::shared_ptr<std::deque<om::coords::Chunk> > GetMipChunkCoords(
      const int mipLevel) const;

  // mip data accessors
  bool ContainsVoxel(const om::coords::Global& vox);

  om::common::DataType getVolDataType() { return mVolDataType; }

  std::string getVolDataTypeAsStr() {
    return OmVolumeTypeHelpers::GetTypeAsString(mVolDataType);
  }

  virtual void SetVolDataType(const om::common::DataType) = 0;

  virtual int GetBytesPerVoxel() const = 0;
  virtual int GetBytesPerSlice() const = 0;

  inline om::coords::VolumeSystem& Coords() { return coords_; }
  inline const om::coords::VolumeSystem& Coords() const { return coords_; }

  virtual om::chunk::ChunkDS& ChunkDS() const = 0;
  virtual om::tile::TileDS& TileDS() const = 0;

  template <typename T>
  struct iterable_volume {
    typedef om::volume::all_dataval_iterator<T> iterator;
    iterator begin() {
      return om::volume::make_all_dataval_iterator<T>(bounds, vol.ChunkDS());
    }
    iterator end() { return iterator(); }

    const OmMipVolume& vol;
    om::coords::DataBbox bounds;
  };

  template <typename T>
  iterable_volume<T> Iterate() const {
    return OmMipVolume::iterable_volume<T>{*this, Coords().Bounds()};
  }
  template <typename T>
  iterable_volume<T> Iterate(om::coords::DataBbox bounds) const {
    return OmMipVolume::iterable_volume<T>{*this, bounds};
  }
  template <typename T>
  iterable_volume<T> Iterate(om::coords::GlobalBbox bounds) const {
    return OmMipVolume::iterable_volume<T>{*this,
                                           bounds.ToDataBbox(Coords(), 0)};
  }

 protected:
  om::common::DataType mVolDataType;

  om::coords::VolumeSystem coords_;

  int mBuildState;
  void SetBuildState(const MipVolumeBuildState s) { mBuildState = s; }

  void addChunkCoordsForLevel(const int mipLevel,
                              std::deque<om::coords::Chunk>* coords) const;

 private:
  template <class T>
  friend class OmVolumeBuilderBase;
  template <class T>
  friend class OmVolumeBuilderHdf5;
  template <class T>
  friend class OmVolumeBuilderImages;
  template <class T>
  friend class OmMipVolumeArchive;
  template <class T>
  friend class om::data::archive::mipVolume;

  friend class OmMipVolumeArchiveOld;
  friend class OmMemMappedVolume;
  friend class OmUpgradeTo14;
  friend class OmWatershedImporter;
  friend class om::rebuilder::segmentation;
};
