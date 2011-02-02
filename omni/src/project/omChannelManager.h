#ifndef OM_CHANNEL_MANAGER_H
#define OM_CHANNEL_MANAGER_H

#include "common/omCommon.h"
#include "common/om.hpp"
#include "system/omGenericManager.h"
#include "volume/omChannel.h"

class OmChannelManager {
public:
	OmChannel& GetChannel(const OmID id);
	OmChannel& AddChannel();
	void RemoveChannel(const OmID id);
	bool IsChannelValid(const OmID id);
	const OmIDsSet & GetValidChannelIds();
	bool IsChannelEnabled(const OmID id);
	void SetChannelEnabled(const OmID id, const bool enable);

private:
	OmGenericManager<OmChannel> mChannelManager;

	friend QDataStream&operator<<(QDataStream& out, const OmChannelManager&);
	friend QDataStream&operator>>(QDataStream& in, OmChannelManager&);
};

#endif
