#pragma once

#include "system/omManageableObject.h"
#include "datalayer/archive/affinity.h"
#include "common/omString.hpp"

class OmChunk;
class OmAffinityChannel;
template <typename T> class OmRawChunk;

class OmAffinityGraph : public OmManageableObject {
private:
    std::map<om::common::AffinityGraph, om::shared_ptr<OmAffinityChannel> > channels_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmAffinityGraph& chan);
    friend void YAML::operator>>(const YAML::Node& in, OmAffinityGraph& chan);

public:
    OmAffinityGraph();
    OmAffinityGraph(const om::common::ID id);
    virtual ~OmAffinityGraph();

    void ImportAllChannels(const QString& hdf5fnp);

    void ImportSingleChannel(const QString& hdf5fnp,
                             const om::common::AffinityGraph aff);

    OmAffinityChannel* GetChannel(const om::common::AffinityGraph aff) const;

    inline std::string GetName(){
        return "affinity" + om::string::num(GetID());
    }

    OmChunk* MipChunk(const om::common::AffinityGraph aff,
                      const om::coords::Chunk& coord);

    om::shared_ptr<OmRawChunk<float> > RawChunk(const om::common::AffinityGraph aff,
                                                const om::coords::Chunk& coord);

};

