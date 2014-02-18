#pragma once

#include "common/macro.hpp"
#include "yaml-cpp/yaml.h"
#include "common/logging.h"
#include "task/cell.h"
#include "common/time.h"
#include "network/http/httpRefreshable.hpp"

namespace om {
namespace task {

struct Dataset {
  int id;
  std::string name;
  std::string description;
  time::time created;
  time::time deleted;
  time::time completed;
  time::time lastmodified;
  int overlap;
  int min_overlap_volume;
};

typedef network::HTTPRefreshableType<std::vector<Dataset>> Datasets;
}
}  // namespace om::task::

namespace YAML {

template <>
struct convert<om::task::Dataset> {
  static Node encode(const om::task::Dataset& c) {
    Node node;
    node["id"] = c.id;
    node["name"] = c.name;
    node["description"] = c.description;
    node["created"] = c.created;
    node["deleted"] = c.deleted;
    node["completed"] = c.completed;
    node["lastmodified"] = c.lastmodified;
    node["overlap"] = c.overlap;
    node["min_overlap_volume"] = c.min_overlap_volume;
    return node;
  }

  static bool decode(const Node& node, om::task::Dataset& c) {
    try {

      c.id = node["id"].as<int>();
      c.name = node["name"].as<std::string>();
      c.description = node["description"].as<std::string>();
      // c.created = node["created"].as<om::time::time>();
      // c.deleted = node["deleted"].as<om::time::time>();
      // c.completed = node["completed"].as<om::time::time>();
      // c.lastmodified = node["lastmodified"].as<om::time::time>();
      c.overlap = node["overlap"].as<int>();
      c.min_overlap_volume = node["min_overlap_volume"].as<int>();
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs << std::string("Error Decoding Dataset: ") + e.what();
      return false;
    }
  }
};
}