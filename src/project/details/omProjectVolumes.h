#pragma once

/*
 *  Manages data structures that are shared between various parts of the system.  Making centralized
 *  changes in the StateManager will send events that cause the other interested systems to be
 *  notified and synchronized.
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"
#include "datalayer/archive/project.h"

class affinityGraphManager;
class channelManager;
class segmentationManager;

class OmProjectVolumes {
public:
    OmProjectVolumes();
    ~OmProjectVolumes();

    channelManager& Channels(){
        return *channels_;
    }

    segmentationManager& Segmentations(){
        return *segmentations_;
    }

    affinityGraphManager& AffinityGraphs(){
        return *affGraphs_;
    }

private:
    const boost::scoped_ptr<channelManager> channels_;
    const boost::scoped_ptr<segmentationManager> segmentations_;
    const boost::scoped_ptr<affinityGraphManager> affGraphs_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmProjectVolumes& p );
    friend void YAML::operator>>(const YAML::Node& in, OmProjectVolumes& p );
    friend QDataStream& operator<<(QDataStream& out, const OmProjectVolumes& p );
    friend QDataStream& operator>>(QDataStream& in, OmProjectVolumes& p );
};

