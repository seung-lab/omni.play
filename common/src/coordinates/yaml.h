#pragma once

#include "yaml-cpp/yaml.h"

namespace om { namespace coords {
class Global;
class GlobalBbox;
class VolumeSystem;
} }


namespace YAML
{

Emitter& operator<<(Emitter&, const om::coords::Global&);
void operator>>(const Node&, om::coords::Global&);
Emitter& operator<<(Emitter&, const om::coords::GlobalBbox&);
void operator>>(const Node&, om::coords::GlobalBbox&);
Emitter& operator<<(Emitter&, const om::coords::VolumeSystem&);
void operator>>(const Node&, om::coords::VolumeSystem&);

} // namespace YAML
