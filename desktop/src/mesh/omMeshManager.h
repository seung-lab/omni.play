#pragma once

#include "common/om.hpp"
#include "mesh/omMeshTypes.h"

#include <boost/scoped_ptr.hpp>

class OmMeshCache;
class OmMeshConvertV1toV2;
class OmMeshFilePtrCache;
class OmMeshMetadata;
class OmMeshReaderV2;
class OmMesh;
class OmMeshCoord;
class OmSegmentation;

#include <QDataStream>

class OmMeshManager {
 public:
  OmMeshManager(OmSegmentation* segmentation, const double threshold);
  ~OmMeshManager();

  void Create();
  void Load();

  OmSegmentation* GetSegmentation() const { return segmentation_; }

  double Threshold() const { return threshold_; }

  void CloseDownThreads();

  OmMeshPtr Produce(const OmMeshCoord&);

  void GetMesh(OmMeshPtr&, const OmMeshCoord&, const om::Blocking);

  void UncacheMesh(const OmMeshCoord& coord);

  void ClearCache();

  OmMeshReaderV2* Reader() { return reader_.get(); }

  OmMeshMetadata* Metadata() { return metadata_.get(); }

  void ActivateConversionFromV1ToV2();

  OmMeshConvertV1toV2* Converter() { return converter_.get(); }

  OmMeshFilePtrCache* FilePtrCache() { return filePtrCache_.get(); }

 private:
  OmSegmentation* const segmentation_;
  const double threshold_;

  const boost::scoped_ptr<OmMeshCache> dataCache_;
  const boost::scoped_ptr<OmMeshFilePtrCache> filePtrCache_;
  const boost::scoped_ptr<OmMeshMetadata> metadata_;

  boost::scoped_ptr<OmMeshReaderV2> reader_;
  boost::scoped_ptr<OmMeshConvertV1toV2> converter_;

  void HandleFetchUpdate();

  void inferMeshMetadata();
  void loadThreadhold1();
  void loadThreadholdNon1();
  void moveOldMetadataFile();

  friend QDataStream& operator<<(QDataStream&, const OmMeshManager&);
  friend QDataStream& operator>>(QDataStream&, OmMeshManager&);
};
