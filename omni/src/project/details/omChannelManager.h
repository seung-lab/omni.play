#pragma once

#include "common/omCommon.h"
#include "common/om.hpp"
#include "system/omGenericManager.hpp"
#include "volume/omChannel.h"
#include "datalayer/archive/channel.h"

class OmChannelManager {
public:
    OmChannel& GetChannel(const OmID id);
    OmChannel& AddChannel();
    void RemoveChannel(const OmID id);
    bool IsChannelValid(const OmID id);
    const OmIDsSet & GetValidChannelIds();
    bool IsChannelEnabled(const OmID id);
    void SetChannelEnabled(const OmID id, const bool enable);
    const std::vector<OmChannel*> GetPtrVec() const;

private:
    OmGenericManager<OmChannel> manager_;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmChannelManager&);
    friend void YAML::operator>>(const YAML::Node& in, OmChannelManager&);
    friend QDataStream& operator<<(QDataStream& out, const OmChannelManager&);
    friend QDataStream& operator>>(QDataStream& in, OmChannelManager&);
};

