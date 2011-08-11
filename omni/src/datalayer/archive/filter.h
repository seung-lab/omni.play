#pragma once

#include "yaml-cpp/yaml.h"

class OmFilter2dManager;
class OmFilter2d;

namespace om {
namespace data {
namespace archive {

YAML::Emitter &operator<<(YAML::Emitter& out, const OmFilter2dManager& fm);
void operator>>(const YAML::Node& in, OmFilter2dManager& fm);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmFilter2d& f);
void operator>>(const YAML::Node& in, OmFilter2d& f);

}; // namespace archive
}; // namespace data
}; // namespace om