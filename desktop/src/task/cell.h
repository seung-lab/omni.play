#pragma once

#include "yaml-cpp/yaml.h"
#include "common/logging.h"

namespace om {
namespace task {

struct Cell {
  std::string Name;
  std::string Description;
  uint32_t Target;
  std::string Completed;  // TODO: boost::optional<std::chrono::time_point>
  uint32_t CellID;
  uint32_t Count;
  uint32_t Underconfident;
  uint32_t Weight;
  int32_t Growth;
  uint32_t Avail;
};
}
}  // namespace om::task::

namespace YAML {

template <>
struct convert<om::task::Cell> {
  static Node encode(const om::task::Cell& c) {
    Node node;
    node["name"] = c.Name;
    node["description"] = c.Description;
    node["target"] = c.Target;
    node["completed"] = c.Completed;
    node["cell"] = c.CellID;
    node["count"] = c.Count;
    node["underconfident"] = c.Underconfident;
    node["weight"] = c.Weight;
    node["growth"] = c.Growth;
    node["avail"] = c.Avail;
    return node;
  }

  static bool decode(const Node& node, om::task::Cell& c) {
    try {
      c.Name = node["name"].as<std::string>("");
      c.Description = node["description"].as<std::string>("");
      c.Target = node["target"].as<uint32_t>(0);
      c.Completed = node["completed"].as<std::string>("");
      c.CellID = node["cell"].as<uint32_t>(0);
      c.Count = node["count"].as<uint32_t>(0);
      c.Underconfident = node["underconfident"].as<uint32_t>(0);
      c.Weight = node["weight"].as<uint32_t>(0);
      c.Growth = node["growth"].as<int32_t>(0);
      c.Avail = node["avail"].as<uint32_t>(0);
      return true;
    }
    catch (std::exception e) {
      log_debugs << std::string("Error Decoding Cell: ") + e.what();
      return false;
    }
  }
};
}