#include "volume/channel.h"

namespace om {
namespace volume {
    
channel::channel()
	: channelImpl()
{}

channel::channel(common::id id)
	: channelImpl(id)
{}

channel::~channel()
{}

} // namespace volume
} // namespace om