#pragma once

#include "yaml-cpp/yaml.h"

namespace om
{
class globalCoord;
class globalBbox;
} // namespace om


namespace YAML
{
Emitter& operator<<(Emitter&, const om::globalCoord&);
void operator>>(const Node&, om::globalCoord&);
Emitter& operator<<(Emitter&, const om::globalBbox&);
void operator>>(const Node&, om::globalBbox&);

} // namespace YAML
