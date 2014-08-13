#pragma once
#include "precomp.h"

class OmAffinityGraphManager;
class OmAffinityGraph;
class OmAffinityChannel;

namespace YAML {

template <>
struct convert<OmAffinityGraphManager> {
  static Node encode(const OmAffinityGraphManager& fm);
  static bool decode(const Node& node, OmAffinityGraphManager& fm);
};

template <>
struct convert<OmAffinityGraph> {
  static Node encode(const OmAffinityGraph& fm);
  static bool decode(const Node& node, OmAffinityGraph& fm);
};

template <>
struct convert<OmAffinityChannel> {
  static Node encode(const OmAffinityChannel& fm);
  static bool decode(const Node& node, OmAffinityChannel& fm);
};

}  // namespace YAML
