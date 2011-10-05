#include "volume/channel.h"

channel::channel()
	: channelImpl()
{}

channel::channel(OmID id)
	: channelImpl(id)
{}

channel::~channel()
{}
