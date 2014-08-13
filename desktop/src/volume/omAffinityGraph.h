#pragma once
#include "precomp.h"

#include "coordinates/chunk.h"
#include "common/enums.hpp"
#include "system/omManageableObject.h"
#include "datalayer/archive/affinity.h"
#include "common/string.hpp"

class OmChunk;
class OmAffinityChannel;
template <typename T>
class OmRawChunk;

class OmAffinityGraph : public OmManageableObject {
 public:
  OmAffinityGraph();
  OmAffinityGraph(const om::common::ID id);
  virtual ~OmAffinityGraph();

  void ImportAllChannels(const QString& hdf5fnp);

  void ImportSingleChannel(const QString& hdf5fnp,
                           const om::common::AffinityGraph aff);

  OmAffinityChannel* GetChannel(const om::common::AffinityGraph aff) const;

  inline std::string GetName() { return "affinity" + om::string::num(GetID()); }

 private:
  std::map<om::common::AffinityGraph, std::shared_ptr<OmAffinityChannel> >
      channels_;

  friend class YAML::convert<OmAffinityGraph>;
};
