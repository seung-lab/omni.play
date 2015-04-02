#pragma once
#include "precomp.h"

#include "datalayer/omDataWrapper.h"
#include "datalayer/paths.hpp"
#include "mesh/omMeshTypes.h"
#include "system/omManageableObject.h"
#include "volume/omMipVolume.h"
#include "datalayer/archive/segmentation.h"
#include "segment/dataSources.hpp"
#include "chunk/dataSources.hpp"
#include "tile/dataSources.hpp"
#include "volume/iterators.hpp"

class OmChunk;
class OmMeshDrawer;
class OmMeshManager;
class OmMeshManagers;
class OmRawSegTileCache;
class OmSegChunk;
class OmSegment;
class OmSegments;
class OmTileCacheSegmentation;
class OmValidGroupNum;
class OmViewGroupState;
class OmVolumeCuller;
class OmVolumeData;
class SegmentationDataWrapper;

namespace om {
namespace volume {
class MetadataManager;
class MetadataDataSource;
}
namespace segmentation {
class folder;
}
namespace annotation {
class manager;
}
namespace segment {
class FileDataSource;
class ListTypeFileDataSource;
class UserEdgeVector;
}
namespace chunk {
class CachedDataSource;
class CachedUniqueValuesDataSource;
}
namespace tile {
class CachedDataSource;
}
}

class OmSegmentation : public OmMipVolume, public OmManageableObject {
 public:
  OmSegmentation();
  OmSegmentation(om::common::ID id);
  virtual ~OmSegmentation();

  std::string GetName() const;
  std::string GetNameHyphen() const;

  const om::file::Paths::Vol& VolPaths() const { return paths_; }
  const om::file::Paths::Seg& SegPaths() const { return paths_; }
  void LoadPath();
  void LoadEmptyPath();
  void LoadChunks();
  bool LoadVolData();
  bool LoadVolDataIfFoldersExist();
  void UpdateFromVolResize();

  inline om::common::ObjectType getVolumeType() const {
    return om::common::SEGMENTATION;
  }

  inline om::common::ID GetID() const { return id_; }
  inline om::common::ID id() const { return id_; }

  virtual int GetBytesPerVoxel() const;
  virtual int GetBytesPerSlice() const;

  SegmentationDataWrapper GetSDW() const;

  void CloseDownThreads();

  void Flush();

  void SetDendThreshold(double t);
  double GetDendThreshold();

  void SetSizeThreshold(double t);
  double GetSizeThreshold();

  OmMeshManager* MeshManager(const double threshold);

  void UpdateVoxelBoundingData();

  void SetVolDataType(const om::common::DataType);

  void BuildBlankVolume(const Vector3i& dims);

  std::shared_ptr<om::chunk::ChunkVar> GetChunk(const om::coords::Chunk& coord);
  std::shared_ptr<om::tile::TileVar> GetTile(const om::coords::Tile& coord);
  void InvalidateTiles(const om::coords::Chunk& coord);

  uint32_t GetVoxelValue(const om::coords::Global& vox);
  void SetVoxelValue(const om::coords::Global& vox, const uint32_t value);
  bool SetVoxelValueIfSelected(const om::coords::Global& vox,
                               const uint32_t value);

  void RebuildSegments();

  template <typename T>
  struct filtered_iterable_volume {
    typedef om::volume::segment_filtered_dataval_iterator<T> iterator;
    iterator begin() {
      return om::volume::make_segment_filtered_dataval_iterator(
          bounds, vol.ChunkDS(), vol.UniqueValuesDS(), id);
    }
    iterator end() { return iterator(); }

    const OmSegmentation& vol;
    om::coords::DataBbox bounds;
    T id;
  };

  template <typename T>
  filtered_iterable_volume<T> SegIterate(T id) const {
    return OmSegmentation::filtered_iterable_volume<T>{*this, Coords().Bounds(),
                                                       id};
  }
  template <typename T>
  filtered_iterable_volume<T> SegIterate(T id,
                                         om::coords::GlobalBbox bounds) const {
    return OmSegmentation::filtered_iterable_volume<T>{
        *this, bounds.ToDataBbox(Coords(), 0), id};
  }
  template <typename T>
  filtered_iterable_volume<T> SegIterate(T id,
                                         om::coords::DataBbox bounds) const {
    return OmSegmentation::filtered_iterable_volume<T>{*this, bounds, id};
  }

  template <typename T>
  struct filtered_set_iterable_volume {
    typedef om::volume::segment_filtered_dataval_iterator<T> iterator;
    iterator begin() {
      return om::volume::make_segment_filtered_dataval_iterator(
          bounds, vol.ChunkDS(), vol.UniqueValuesDS(), ids);
    }
    iterator end() { return iterator(); }

    const OmSegmentation& vol;
    om::coords::DataBbox bounds;
    std::set<T> ids;
  };

  template <typename T>
  filtered_set_iterable_volume<T> SegIterate(std::set<T> ids) const {
    return OmSegmentation::filtered_set_iterable_volume<T>{
        *this, Coords().Bounds(), ids};
  }
  template <typename T>
  filtered_set_iterable_volume<T> SegIterate(
      std::set<T> ids, om::coords::GlobalBbox bounds) const {
    return OmSegmentation::filtered_set_iterable_volume<T>{
        *this, bounds.ToDataBbox(Coords(), 0), ids};
  }
  template <typename T>
  filtered_set_iterable_volume<T> SegIterate(
      std::set<T> ids, om::coords::DataBbox bounds) const {
    return OmSegmentation::filtered_set_iterable_volume<T>{*this, bounds, ids};
  }

 public:
  inline OmMeshManagers& MeshManagers() { return *meshManagers_; }
  inline OmSegments& Segments() { return *segments_; }
  inline OmValidGroupNum& ValidGroupNum() { return *validGroupNum_; }
  inline OmVolumeData& VolData() { return *volData_; }
  inline OmRawSegTileCache& SliceCache() { return *volSliceCache_; }
  inline OmTileCacheSegmentation& TileCache() { return *tileCache_; }
  inline om::annotation::manager& Annotations() const { return *annotations_; }
  om::chunk::ChunkDS& ChunkDS() const override;
  om::tile::TileDS& TileDS() const override;
  om::chunk::UniqueValuesDS& UniqueValuesDS() const;

  om::segment::SegDataVector& SegData() const { return *segData_; }
  om::segment::SegListDataVector& SegListData() const { return *segListData_; }
  om::segment::EdgeVector& MST() { return *mst_; }
  om::segment::UserEdgeVector& UserEdges() { return *userEdges_; }
  om::volume::MetadataManager& Metadata() const { return *metaManager_; }

  void ClearUserChangesAndSave();

 private:
  std::unique_ptr<OmMeshManagers> meshManagers_;
  std::unique_ptr<OmSegments> segments_;
  std::unique_ptr<OmValidGroupNum> validGroupNum_;
  std::unique_ptr<OmVolumeData> volData_;
  std::unique_ptr<OmRawSegTileCache> volSliceCache_;
  std::unique_ptr<OmTileCacheSegmentation> tileCache_;
  std::unique_ptr<om::annotation::manager> annotations_;

  std::unique_ptr<om::volume::MetadataDataSource> metaDS_;
  std::unique_ptr<om::volume::MetadataManager> metaManager_;
  std::unique_ptr<om::segment::FileDataSource> segDataDS_;
  std::unique_ptr<om::segment::SegDataVector> segData_;
  std::unique_ptr<om::segment::ListTypeFileDataSource> segListDataDS_;
  std::unique_ptr<om::segment::SegListDataVector> segListData_;
  std::unique_ptr<om::segment::EdgeVector> mst_;
  std::unique_ptr<om::segment::UserEdgeVector> userEdges_;
  std::unique_ptr<om::chunk::CachedDataSource> chunkDS_;
  std::unique_ptr<om::tile::CachedDataSource> tileDS_;
  std::unique_ptr<om::chunk::CachedUniqueValuesDataSource> uniqueChunkValues_;

  om::file::Paths::Seg paths_;

  template <class T>
  friend class OmVolumeBuilder;
  template <class T>
  friend class OmVolumeBuilderHdf5;
  template <class T>
  friend class OmVolumeBuilderImages;
  template <class T>
  friend class OmVolumeImporter;

  friend class OmSegmentsImpl;
  friend class OmSegmentsImplLowLevel;
  friend class OmSegmentIterator;
  friend class OmSegmentationChunkBuildTask;
  friend class SegmentTests1;

  friend class OmDataArchiveProjectImpl;
  friend class YAML::convert<OmSegmentation>;
  friend QDataStream& operator>>(QDataStream& in, OmSegmentation&);
  friend QDataStream& operator<<(QDataStream& out, const OmSegmentation&);
};
