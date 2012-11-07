#pragma once

#include "yaml-cpp/yaml.h"

namespace om {
namespace coords {
	class VolumeSystem;
}}

class OmChannelManager;
class OmChannel;

namespace YAML {

Emitter &operator<<(Emitter& out, const OmChannelManager& cm);
void operator>>(const Node& in, OmChannelManager& cm);
Emitter& operator<<(Emitter& out, const OmChannel& chan);
void operator>>(const Node& in, OmChannel& chan);

} // namespace YAML