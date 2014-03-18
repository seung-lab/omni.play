#pragma once
#include "precomp.h"

#include "common/common.h"
#include "datalayer/file.h"
#include "common/genericManager.hpp"
#include "volume/omSegmentation.h"

namespace om {
namespace volume {
class MetadataDataSource;
}
}

class OmProjectVolumes;

extern template class om::common::GenericManager<OmSegmentation>;

class OmSegmentationManager {
 private:
  OmProjectVolumes& volumes_;

 public:
  OmSegmentationManager(OmProjectVolumes& volumes) : volumes_(volumes) {}

  void Load();
  OmSegmentation* GetSegmentation(const om::common::ID id);
  OmSegmentation& AddSegmentation();
  void RemoveSegmentation(const om::common::ID id);
  bool IsSegmentationValid(const om::common::ID id);
  const om::common::IDSet& GetValidSegmentationIds();
  bool IsSegmentationEnabled(const om::common::ID id);
  void SetSegmentationEnabled(const om::common::ID id, const bool enable);
  const std::vector<OmSegmentation*> GetPtrVec() const;

 private:
  static om::file::path path();

  om::common::GenericManager<OmSegmentation> manager_;
};
