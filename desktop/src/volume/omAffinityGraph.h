#pragma once

#include "system/omManageableObject.h"
#include "datalayer/archive/affinity.h"
#include "common/omString.hpp"

class OmChunk;
class OmAffinityChannel;
template <typename T> class OmRawChunk;

class OmAffinityGraph : public OmManageableObject {
private:
    std::map<om::AffinityGraph, om::shared_ptr<OmAffinityChannel> > channels_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmAffinityGraph& chan);
    friend void YAML::operator>>(const YAML::Node& in, OmAffinityGraph& chan);

public:
    OmAffinityGraph();
    OmAffinityGraph(const OmID id);
    virtual ~OmAffinityGraph();

    void ImportAllChannels(const QString& hdf5fnp);

    void ImportSingleChannel(const QString& hdf5fnp,
                             const om::AffinityGraph aff);

    OmAffinityChannel* GetChannel(const om::AffinityGraph aff) const;

    inline std::string GetName(){
        return "affinity" + om::string::num(GetID());
    }

    OmChunk* MipChunk(const om::AffinityGraph aff,
                      const om::chunkCoord& coord);

    om::shared_ptr<OmRawChunk<float> > RawChunk(const om::AffinityGraph aff,
                                                const om::chunkCoord& coord);

};

