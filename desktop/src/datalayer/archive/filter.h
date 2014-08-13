#pragma once
#include "precomp.h"

class OmFilter2dManager;
class OmFilter2d;

namespace YAML {
template <>
struct convert<OmFilter2dManager> {
  static Node encode(const OmFilter2dManager& fm);
  static bool decode(const Node& node, OmFilter2dManager& fm);
};

template <>
struct convert<OmFilter2d> {
  static Node encode(const OmFilter2d& f);
  static bool decode(const Node& node, OmFilter2d& f);
};

}  // namespace YAML
