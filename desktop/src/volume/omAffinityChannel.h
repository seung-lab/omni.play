#pragma once

#include "volume/omChannel.h"
#include "datalayer/archive/affinity.h"

class OmAffinityGraph;

class OmAffinityChannel : public OmChannel {
private:
    const om::common::AffinityGraph affinity_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmAffinityGraph& chan);

public:
    OmAffinityChannel(const om::common::AffinityGraph aff)
        : OmChannel()
        , affinity_(aff)
    {
        if (!(aff == om::common::X_AFFINITY ||
              aff == om::common::Y_AFFINITY ||
              aff == om::common::Z_AFFINITY))
        {
            throw OmArgException("Bad Affinity Type.");
        }
    }

    OmAffinityChannel(OmID id, const om::common::AffinityGraph aff)
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
        case om::common::X_AFFINITY:
            return str( boost::format("affinity/affinity%1%/X") % GetID());
        case om::common::Y_AFFINITY:
            return str( boost::format("affinity/affinity%1%/Y") % GetID());
        case om::common::Z_AFFINITY:
            return str( boost::format("affinity/affinity%1%/Z") % GetID());
        default:
            throw OmArgException("unknown arg");
        }
    }
};

