#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "volume/omAffinityGraph.h"

extern template class om::common::GenericManager<OmAffinityGraph>;

class OmAffinityGraphManager {
 public:
  OmAffinityGraph& Get(const om::common::ID id);
  OmAffinityGraph& Add();
  void Remove(const om::common::ID id);
  bool IsValid(const om::common::ID id);
  const om::common::IDSet& GetValidIds();
  bool IsEnabled(const om::common::ID id);
  void SetEnabled(const om::common::ID id, const bool enable);

 private:
  om::common::GenericManager<OmAffinityGraph> graphs_;

  friend class YAML::convert<OmAffinityGraphManager>;
};
