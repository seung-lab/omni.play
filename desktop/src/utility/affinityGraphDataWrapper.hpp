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
    mutable boost::optional<OmAffinityGraph&> affGraph_;

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
    
    inline OmAffinityGraph& GetAffinityGraph() const
    {
        if(!affGraph_){
            //printf("cached segmentation...\n");
            affGraph_ =
            boost::optional<OmAffinityGraph&>(OmProject::Volumes().AffinityGraphs().Get(id_));
        }
        return *affGraph_;
    }
    
    inline QString GetName() const {
        return QString::fromStdString(GetAffinityGraph().GetName());
    }
    
    inline bool isEnabled() const {
        return OmProject::Volumes().AffinityGraphs().IsEnabled(id_);
    }

    OmAffinityGraph& Create()
    {
        OmAffinityGraph& a = OmProject::Volumes().AffinityGraphs().Add();
        id_ = a.GetID();
        printf("create affinity graph %d\n", id_);
        return a;
    }
};

