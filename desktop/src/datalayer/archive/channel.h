#pragma once
#include "precomp.h"

class OmChannelManager;
class OmChannel;
class OmMipVolCoords;

namespace YAML {

template <>
struct convert<OmChannelManager> {
  static Node encode(const OmChannelManager& fm);
  static bool decode(const Node& node, OmChannelManager& fm);
};

template <>
struct convert<OmChannel> {
  static Node encode(const OmChannel& fm);
  static bool decode(const Node& node, OmChannel& fm);
};

}  // namespace YAML
