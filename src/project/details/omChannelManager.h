#pragma once

#include "common/common.h"
#include "common/om.hpp"
#include "system/omGenericManager.hpp"
#include "volume/channel.h"
#include "datalayer/archive/channel.h"

class channelManager {
public:
    channel& GetChannel(const OmID id);
    channel& AddChannel();
    void RemoveChannel(const OmID id);
    bool IsChannelValid(const OmID id);
    const OmIDsSet & GetValidChannelIds();
    bool IsChannelEnabled(const OmID id);
    void SetChannelEnabled(const OmID id, const bool enable);
    const std::vector<channel*> GetPtrVec() const;

private:
    OmGenericManager<channel> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const channelManager&);
    friend void YAML::operator>>(const YAML::Node& in, channelManager&);
    friend QDataStream& operator<<(QDataStream& out, const channelManager&);
    friend QDataStream& operator>>(QDataStream& in, channelManager&);
};

