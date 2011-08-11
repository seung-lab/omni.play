#pragma once

#include "yaml-cpp/yaml.h"

class OmChannelManager;
class OmChannel;
class OmMipVolCoords;

namespace om {
namespace data {
namespace archive {

YAML::Emitter &operator<<(YAML::Emitter& out, const OmChannelManager& cm);
void operator>>(const YAML::Node& in, OmChannelManager& cm);
YAML::Emitter& operator<<(YAML::Emitter& out, const OmChannel& chan);
void operator>>(const YAML::Node& in, OmChannel& chan);
YAML::Emitter& operator<<(YAML::Emitter& out, const OmMipVolCoords& c);
void operator>>(const YAML::Node& in, OmMipVolCoords& c);

}; // namespace archive
}; // namespace data
}; // namespace om