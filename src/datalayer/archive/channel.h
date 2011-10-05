#pragma once

#include "yaml-cpp/yaml.h"

class channelManager;
class channel;
class OmMipVolCoords;

namespace YAML {

Emitter &operator<<(Emitter& out, const channelManager& cm);
void operator>>(const Node& in, channelManager& cm);
Emitter& operator<<(Emitter& out, const channel& chan);
void operator>>(const Node& in, channel& chan);
Emitter& operator<<(Emitter& out, const OmMipVolCoords& c);
void operator>>(const Node& in, OmMipVolCoords& c);

} // namespace YAML