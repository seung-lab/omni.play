#pragma once

#include "yaml-cpp/yaml.h"

class OmAffinityGraphManager;
class OmAffinityGraph;
class OmAffinityChannel;

namespace YAML {
    
    Emitter &operator<<(Emitter& out, const OmAffinityGraphManager& cm);
    void operator>>(const Node& in, OmAffinityGraphManager& cm);
    Emitter& operator<<(Emitter& out, const OmAffinityGraph& chan);
    void operator>>(const Node& in, OmAffinityGraph& chan);
    Emitter& operator<<(Emitter& out, const OmAffinityChannel& c);
    void operator>>(const Node& in, OmAffinityChannel& c);
    
} // namespace YAML