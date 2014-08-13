#pragma once
#include "precomp.h"

#include "common/common.h"
#include "system/omGenericManager.hpp"
#include "volume/omSegmentation.h"
#include "datalayer/archive/segmentation.h"

class OmProjectVolumes;

class OmSegmentationManager {
 private:
  OmProjectVolumes* const volumes_;

 public:
  OmSegmentationManager(OmProjectVolumes* volumes) : volumes_(volumes) {}

  OmSegmentation* GetSegmentation(const om::common::ID id);
  OmSegmentation& AddSegmentation();
  void RemoveSegmentation(const om::common::ID id);
  bool IsSegmentationValid(const om::common::ID id);
  const om::common::IDSet& GetValidSegmentationIds();
  bool IsSegmentationEnabled(const om::common::ID id);
  void SetSegmentationEnabled(const om::common::ID id, const bool enable);
  const std::vector<OmSegmentation*> GetPtrVec() const;

 private:
  OmGenericManager<OmSegmentation> manager_;

  friend class YAML::convert<OmSegmentationManager>;
  friend QDataStream& operator<<(QDataStream& out,
                                 const OmSegmentationManager&);
  friend QDataStream& operator>>(QDataStream& in, OmSegmentationManager&);
};
