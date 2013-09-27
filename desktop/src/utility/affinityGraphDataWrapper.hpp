#pragma once

#include "volume/omAffinityGraph.h"
#include "project/omProject.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omAffinityGraphManager.h"

class AffinityGraphDataWrapper {
public:
    static const om::common::IDSet& ValidIDs(){
        return OmProject::Volumes().AffinityGraphs().GetValidIds();
    }

private:
    om::common::ID id_;
    mutable boost::optional<OmAffinityGraph&> affGraph_;

public:
    AffinityGraphDataWrapper()
        : id_(0)
    {}

    explicit AffinityGraphDataWrapper(const om::common::ID ID)
        : id_(ID)
    {}

    inline om::common::ID GetID() const {
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
        auto& a = OmProject::Volumes().AffinityGraphs().Add();
        id_ = a.GetID();
        printf("create affinity graph %d\n", id_);
        return a;
    }
};

