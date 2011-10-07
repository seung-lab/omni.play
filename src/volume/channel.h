#pragma once

/*
 * channel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "volume/channelImpl.h"
#include "datalayer/archive/channel.h"

namespace om {
namespace volume {

class channel : public channelImpl {

public:
    channel();
    channel(common::id id);
    virtual ~channel();

    virtual std::string GetDefaultHDF5DatasetName(){
        return "chanSingle";
    }

private:
    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const channel& chan);
    friend void YAML::operator>>(const YAML::Node& in, channel& chan);
};

} // namespace volume
} // namespace om