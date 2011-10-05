#pragma once

/*
 * channel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "volume/channelImpl.h"
#include "datalayer/archive/channel.h"

class channel : public channelImpl {

public:
    channel();
    channel(OmID id);
    virtual ~channel();

    virtual QString GetDefaultHDF5DatasetName(){
        return "chanSingle";
    }

private:

    friend class OmBuildChannel;
    template <class T> friend class OmVolumeBuilder;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;
    template <class T> friend class OmVolumeImporter;

    friend QDataStream& operator<<(QDataStream& out, const channel&);
    friend QDataStream& operator>>(QDataStream& in, channel&);
    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const channel& chan);
    friend void YAML::operator>>(const YAML::Node& in, channel& chan);
};

