#pragma once
#include "precomp.h"

#include "common/enums.hpp"
#include "mesh/omMeshTypes.h"

class OmMeshCache;
class OmMeshConvertV1toV2;
class OmMeshFilePtrCache;
class OmMeshMetadata;
class OmMeshReaderV2;
class OmMesh;
class OmSegmentation;

class OmMeshManager {
 public:
  OmMeshManager(OmSegmentation* segmentation, const double threshold);
  ~OmMeshManager();

  void Create();
  void Load();

  OmSegmentation* GetSegmentation() const { return segmentation_; }

  double Threshold() const { return threshold_; }

  void CloseDownThreads();

  OmMeshPtr Produce(const om::coords::Mesh&);

  void GetMesh(OmMeshPtr&, const om::coords::Mesh&, const om::common::Blocking);

  void UncacheMesh(const om::coords::Mesh& coord);

  void ClearCache();

  OmMeshReaderV2* Reader() { return reader_.get(); }

  OmMeshMetadata* Metadata() { return metadata_.get(); }

  void ActivateConversionFromV1ToV2();

  OmMeshConvertV1toV2* Converter() { return converter_.get(); }

  OmMeshFilePtrCache* FilePtrCache() { return filePtrCache_.get(); }

 private:
  OmSegmentation* const segmentation_;
  const double threshold_;

  const std::unique_ptr<OmMeshCache> dataCache_;
  const std::unique_ptr<OmMeshFilePtrCache> filePtrCache_;
  const std::unique_ptr<OmMeshMetadata> metadata_;

  std::unique_ptr<OmMeshReaderV2> reader_;
  std::unique_ptr<OmMeshConvertV1toV2> converter_;

  void HandleFetchUpdate();

  void inferMeshMetadata();
  void loadThreadhold1();
  void loadThreadholdNon1();
  void moveOldMetadataFile();

  friend QDataStream& operator<<(QDataStream&, const OmMeshManager&);
  friend QDataStream& operator>>(QDataStream&, OmMeshManager&);
};
