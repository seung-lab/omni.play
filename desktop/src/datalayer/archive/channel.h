#pragma once
#include "precomp.h"

class OmChannelManager;
class OmChannel;
class OmMipVolCoords;

namespace YAMLold {

Emitter& operator<<(Emitter& out, const OmChannelManager& cm);
void operator>>(const Node& in, OmChannelManager& cm);
Emitter& operator<<(Emitter& out, const OmChannel& chan);
void operator>>(const Node& in, OmChannel& chan);

}  // namespace YAMLold
