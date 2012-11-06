#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "volume/omChannel.h"
#include "datalayer/archive/channel.h"

class OmChannelManager {
public:
    OmChannel& GetChannel(const om::common::ID id);
    OmChannel& AddChannel();
    void RemoveChannel(const om::common::ID id);
    bool IsChannelValid(const om::common::ID id);
    const om::common::IDSet & GetValidChannelIds();
    bool IsChannelEnabled(const om::common::ID id);
    void SetChannelEnabled(const om::common::ID id, const bool enable);
    const std::vector<OmChannel*> GetPtrVec() const;

private:
    om::common::GenericManager<OmChannel> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmChannelManager&);
    friend void YAML::operator>>(const YAML::Node& in, OmChannelManager&);
    friend QDataStream& operator<<(QDataStream& out, const OmChannelManager&);
    friend QDataStream& operator>>(QDataStream& in, OmChannelManager&);
};

