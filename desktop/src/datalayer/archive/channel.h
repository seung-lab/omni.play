#pragma once

#include "yaml-cpp/yaml.h"

class OmChannelManager;
class OmChannel;
class OmMipVolCoords;

namespace YAML {

Emitter& operator<<(Emitter& out, const OmChannelManager& cm);
void operator>>(const Node& in, OmChannelManager& cm);
Emitter& operator<<(Emitter& out, const OmChannel& chan);
void operator>>(const Node& in, OmChannel& chan);
Emitter& operator<<(Emitter& out, const OmMipVolCoords& c);
void operator>>(const Node& in, OmMipVolCoords& c);

}  // namespace YAML