#pragma once

#include "project/omProject.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omAffinityGraphManager.h"

class AffinityGraphDataWrapper {
public:
    static const OmIDsSet& ValidIDs(){
        return OmProject::Volumes().AffinityGraphs().GetValidIds();
    }

private:
    OmID id_;

public:
    AffinityGraphDataWrapper()
        : id_(0)
    {}

    explicit AffinityGraphDataWrapper(const OmID ID)
        : id_(ID)
    {}

    inline OmID GetID() const {
        return id_;
    }

    OmAffinityGraph& Create()
    {
        OmAffinityGraph& a = OmProject::Volumes().AffinityGraphs().Add();
        id_ = a.GetID();
        printf("create affinity graph %d\n", id_);
        return a;
    }
};

