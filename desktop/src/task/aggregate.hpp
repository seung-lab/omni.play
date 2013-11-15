#pragma once

#include "common/common.h"
#include "utility/yaml/baseTypes.hpp"
#include "yaml-cpp/yaml.h"
#include <unordered_map>

namespace om {
namespace task {
struct Aggregate {
  std::unordered_map<common::SegID, float> Segments;
  int Weight;
};
}
}

namespace YAML {

template <>
struct convert<om::task::Aggregate> {
  static bool decode(const Node& node, om::task::Aggregate& a) {
    try {
      a.Segments =
          node["segments"].as<std::unordered_map<om::common::SegID, float>>();
      a.Weight = node["weight"].as<int>();
      return true;
    }
    catch (std::exception e) {
      log_debugs(std::string("Error Decoding Aggregate: ") + e.what());
      return false;
    }
  }
};
}