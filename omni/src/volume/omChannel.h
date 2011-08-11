#pragma once

/*
 * OmChannel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "volume/omChannelImpl.h"
#include "datalayer/archive/channel.h"

class OmChannel : public OmChannelImpl {

public:
    OmChannel();
    OmChannel(OmID id);
    virtual ~OmChannel();

    virtual QString GetDefaultHDF5DatasetName(){
        return "chanSingle";
    }

private:

    friend class OmBuildChannel;
    template <class T> friend class OmVolumeBuilder;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;
    template <class T> friend class OmVolumeImporter;

    friend QDataStream& operator<<(QDataStream& out, const OmChannel&);
    friend QDataStream& operator>>(QDataStream& in, OmChannel&);
    friend YAML::Emitter& om::data::archive::operator<<(YAML::Emitter& out, const OmChannel& chan);
    friend void om::data::archive::operator>>(const YAML::Node& in, OmChannel& chan);
};

