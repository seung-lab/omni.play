#pragma once

#include "yaml-cpp/yaml.h"

namespace om 
{ 
namespace project { class channelManager; }
namespace volume { class channel; }
namespace coords { class volumeSystem; }
}

namespace YAML {

Emitter &operator<<(Emitter& out, const om::project::channelManager& cm);
void operator>>(const Node& in, om::project::channelManager& cm);
Emitter& operator<<(Emitter& out, const om::volume::channel& chan);
void operator>>(const Node& in, om::volume::channel& chan);
Emitter& operator<<(Emitter& out, const om::coords::volumeSystem& c);
void operator>>(const Node& in, om::coords::volumeSystem& c);

} // namespace YAML