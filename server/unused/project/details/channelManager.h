#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "volume/channel.h"
#include "datalayer/archive/channel.h"

namespace om {
namespace proj {

class channelManager {
public:
    volume::channel& GetChannel(const common::id id);
    volume::channel& AddChannel();
    void RemoveChannel(const common::id id);
    bool IsChannelValid(const common::id id);
    const common::idSet & GetValidChannelIds();
    bool IsChannelEnabled(const common::id id);
    void SetChannelEnabled(const common::id id, const bool enable);
    const std::vector<volume::channel*> GetPtrVec() const;

private:
    common::genericManager<volume::channel> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const channelManager&);
    friend void YAML::operator>>(const YAML::Node& in, channelManager&);
};

}
}
