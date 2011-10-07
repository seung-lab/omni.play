#pragma once

#include "common/common.h"
#include "common/om.hpp"
#include "system/genericManager.hpp"
#include "volume/channel.h"
#include "datalayer/archive/channel.h"

class channelManager {
public:
    channel& GetChannel(const common::id id);
    channel& AddChannel();
    void RemoveChannel(const common::id id);
    bool IsChannelValid(const common::id id);
    const common::idsSet & GetValidChannelIds();
    bool IsChannelEnabled(const common::id id);
    void SetChannelEnabled(const common::id id, const bool enable);
    const std::vector<channel*> GetPtrVec() const;

private:
    genericManager<channel> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const channelManager&);
    friend void YAML::operator>>(const YAML::Node& in, channelManager&);
    friend QDataStream& operator<<(QDataStream& out, const channelManager&);
    friend QDataStream& operator>>(QDataStream& in, channelManager&);
};

