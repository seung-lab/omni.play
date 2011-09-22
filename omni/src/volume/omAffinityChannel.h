#pragma once

#include "volume/omChannel.h"
#include "datalayer/archive/affinity.h"

class OmAffinityGraph;

class OmAffinityChannel : public OmChannelImpl {
private:
    const om::AffinityGraph affinity_;
    
    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmAffinityGraph& chan);
    
public:
    OmAffinityChannel(const om::AffinityGraph aff)
        : OmChannelImpl()
        , affinity_(aff)
    {
        if (!(aff == om::X_AFFINITY ||
              aff == om::Y_AFFINITY ||
              aff == om::Z_AFFINITY))
        {
            throw OmArgException("Bad Affinity Type.");
        }
    }
    
    OmAffinityChannel(OmID id, const om::AffinityGraph aff)
    : OmChannelImpl(id)
    , affinity_(aff)
    {}
    
    ~OmAffinityChannel()
    { }

    om::AffinityGraph GetAffinity() const {
        return affinity_;
    }

    virtual QString GetDefaultHDF5DatasetName(){
        return "affGraphSingle";
    }
    
    std::string GetDirectoryPath() const 
    {
        switch(affinity_)
        {
        case om::X_AFFINITY:
            return str( boost::format("affinity/affinity%1%/X") % GetID());
        case om::Y_AFFINITY:
            return str( boost::format("affinity/affinity%1%/Y") % GetID());
        case om::Z_AFFINITY:
            return str( boost::format("affinity/affinity%1%/Z") % GetID());
        }
    }
};

