#pragma once

#include "common/enums.hpp"
#include "coordinates/chunkCoord.h"
#include "system/omManageableObject.h"
#include "datalayer/archive/affinity.h"
#include "common/string.hpp"

class OmChunk;
class OmAffinityChannel;
template <typename T> class OmRawChunk;

class OmAffinityGraph : public OmManageableObject {
 private:
  std::map<om::common::AffinityGraph, std::shared_ptr<OmAffinityChannel> >
      channels_;

  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                         const OmAffinityGraph& chan);
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmAffinityGraph& chan);

 public:
  OmAffinityGraph();
  OmAffinityGraph(const om::common::ID id);
  virtual ~OmAffinityGraph();

  void ImportAllChannels(const QString& hdf5fnp);

  void ImportSingleChannel(const QString& hdf5fnp,
                           const om::common::AffinityGraph aff);

  OmAffinityChannel* GetChannel(const om::common::AffinityGraph aff) const;

  inline std::string GetName() { return "affinity" + om::string::num(GetID()); }

  OmChunk* MipChunk(const om::common::AffinityGraph aff,
                    const om::chunkCoord& coord);

  std::shared_ptr<OmRawChunk<float> > RawChunk(
      const om::common::AffinityGraph aff, const om::chunkCoord& coord);

};
