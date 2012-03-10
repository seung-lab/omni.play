#pragma once

#include "yaml-cpp/yaml.h"

namespace om { namespace coords {
class global;
class globalBbox;
class volumeSystem;
} }


namespace YAML
{

Emitter& operator<<(Emitter&, const om::coords::global&);
void operator>>(const Node&, om::coords::global&);
Emitter& operator<<(Emitter&, const om::coords::globalBbox&);
void operator>>(const Node&, om::coords::globalBbox&);
Emitter& operator<<(Emitter&, const om::coords::volumeSystem&);
void operator>>(const Node&, om::coords::volumeSystem&);

} // namespace YAML
