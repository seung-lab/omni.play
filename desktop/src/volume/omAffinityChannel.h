#pragma once

#include "volume/omChannel.h"
#include "datalayer/archive/affinity.h"

class OmAffinityGraph;

class OmAffinityChannel : public OmChannel {
private:
    const om::common::AffinityGraph affinity_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out,
                                           const OmAffinityGraph& chan);

public:
    OmAffinityChannel(const om::common::AffinityGraph aff)
        : OmChannel()
        , affinity_(aff)
    {
        if (!(aff == om::common::AffinityGraph::X_AFFINITY ||
              aff == om::common::AffinityGraph::Y_AFFINITY ||
              aff == om::common::AffinityGraph::Z_AFFINITY))
        {
            throw om::ArgException("Bad Affinity Type.");
        }
    }

    OmAffinityChannel(om::common::ID id, const om::common::AffinityGraph aff)
        : OmChannel(id)
        , affinity_(aff)
    {}

    ~OmAffinityChannel()
    { }

    om::common::AffinityGraph GetAffinity() const {
        return affinity_;
    }

    virtual QString GetDefaultHDF5DatasetName(){
        return "affGraphSingle";
    }

    std::string GetDirectoryPath() const
    {
        switch(affinity_)
        {
        case om::common::AffinityGraph::X_AFFINITY:
            return str( boost::format("affinity/affinity%1%/X") % GetID());
        case om::common::AffinityGraph::Y_AFFINITY:
            return str( boost::format("affinity/affinity%1%/Y") % GetID());
        case om::common::AffinityGraph::Z_AFFINITY:
            return str( boost::format("affinity/affinity%1%/Z") % GetID());
        default:
            throw om::ArgException("unknown arg");
        }
    }
};

