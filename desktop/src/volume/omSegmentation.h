#pragma once
#include "precomp.h"

#include "datalayer/omDataWrapper.h"
#include "mesh/omMeshTypes.h"
#include "system/omManageableObject.h"
#include "volume/omMipVolume.h"
#include "datalayer/archive/segmentation.h"
#include "segment/dataSources.hpp"

class OmChunk;
class OmChunkUniqueValuesManager;
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
template <typename, typename>
class OmChunkCache;

namespace om {
namespace volume {
class MetadataManager;
class MetadataDataSource;
}
namespace segmentation {
class folder;
class loader;
}
namespace annotation {
class manager;
}
namespace segment {
class FileDataSource;
class ListTypeFileDataSource;
class UserEdgeVector;
}
}

class OmSegmentation : public OmMipVolume, public OmManageableObject {
 public:
  OmSegmentation();
  OmSegmentation(om::common::ID id);
  virtual ~OmSegmentation();

  std::string GetName();
  std::string GetNameHyphen();

  std::string GetDirectoryPath() const;
  void LoadPath();

  bool LoadVolData();
  bool LoadVolDataIfFoldersExist();
  void UpdateFromVolResize();

  inline om::common::ObjectType getVolumeType() const {
    return om::common::SEGMENTATION;
  }

  inline om::common::ID getID() const { return GetID(); }
  inline om::common::ID id() const { return GetID(); }

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

  void SetVolDataType(const OmVolDataType);

  void BuildBlankVolume(const Vector3i& dims);

  OmSegChunk* GetChunk(const om::coords::Chunk& coord);

  uint32_t GetVoxelValue(const om::coords::Global& vox);
  void SetVoxelValue(const om::coords::Global& vox, const uint32_t value);
  bool SetVoxelValueIfSelected(const om::coords::Global& vox,
                               const uint32_t value);

  void RebuildSegments();

 public:
  inline OmChunkUniqueValuesManager& UniqueValuesDS() {
    return *uniqueChunkValues_;
  }
  inline OmMeshDrawer& MeshDrawer() { return *meshDrawer_; }
  inline OmMeshManagers& MeshManagers() { return *meshManagers_; }
  inline OmSegments& Segments() { return *segments_; }
  inline OmValidGroupNum& ValidGroupNum() { return *validGroupNum_; }
  inline OmVolumeData& VolData() { return *volData_; }
  inline OmRawSegTileCache& SliceCache() { return *volSliceCache_; }
  inline OmChunkCache<OmSegmentation, OmSegChunk>& ChunkCache() {
    return *chunkCache_;
  }
  inline OmTileCacheSegmentation& TileCache() { return *tileCache_; }
  inline om::segmentation::folder& Folder() const { return *folder_; }
  inline om::annotation::manager& Annotations() const { return *annotations_; }

  om::segment::SegDataVector& SegData() const { return *segData_; }
  om::segment::SegListDataVector& SegListData() const { return *segListData_; }
  om::segment::EdgeVector& MST() { return *mst_; }
  om::segment::UserEdgeVector& UserEdges() { return *userEdges_; }
  om::volume::MetadataManager& Metadata() const { return *metaManager_; }

  void ClearUserChangesAndSave();

 private:
  std::unique_ptr<om::segmentation::folder> folder_;
  std::unique_ptr<OmChunkUniqueValuesManager> uniqueChunkValues_;
  std::unique_ptr<OmMeshDrawer> meshDrawer_;
  std::unique_ptr<OmMeshManagers> meshManagers_;
  std::unique_ptr<OmChunkCache<OmSegmentation, OmSegChunk> > chunkCache_;
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
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmSegmentation&);
  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                               const OmSegmentation&);
  friend QDataStream& operator>>(QDataStream& in, OmSegmentation&);
  friend QDataStream& operator<<(QDataStream& out, const OmSegmentation&);
};
