#pragma once

#include "yaml-cpp/yaml.h"

class OmFilter2dManager;
class OmFilter2d;

namespace YAML {

Emitter &operator<<(Emitter& out, const OmFilter2dManager& fm);
void operator>>(const Node& in, OmFilter2dManager& fm);
Emitter &operator<<(Emitter& out, const OmFilter2d& f);
void operator>>(const Node& in, OmFilter2d& f);

} // namespace YAML