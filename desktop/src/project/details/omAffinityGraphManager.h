#pragma once

#include "common/omCommon.h"
#include "common/om.hpp"
#include "system/omGenericManager.hpp"
#include "volume/omAffinityGraph.h"

class OmAffinityGraphManager {
 public:
  OmAffinityGraph& Get(const OmID id);
  OmAffinityGraph& Add();
  void Remove(const OmID id);
  bool IsValid(const OmID id);
  const OmIDsSet& GetValidIds();
  bool IsEnabled(const OmID id);
  void SetEnabled(const OmID id, const bool enable);

 private:
  OmGenericManager<OmAffinityGraph> graphs_;

  friend QDataStream& operator<<(QDataStream& out,
                                 const OmAffinityGraphManager&);
  friend QDataStream& operator>>(QDataStream& in, OmAffinityGraphManager&);
  friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out,
                                         const OmAffinityGraphManager&);
  friend void YAML::operator>>(const YAML::Node& in, OmAffinityGraphManager&);
};
