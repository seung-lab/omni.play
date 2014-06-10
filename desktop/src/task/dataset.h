#pragma once
#include "precomp.h"

#include "common/macro.hpp"
#include "yaml-cpp/yaml.h"
#include "common/logging.h"
#include "task/cell.h"
#include "common/time.h"

namespace om {
namespace task {

class Dataset {
 public:
 private:
  PROP(int, id);
  PROP_CONST_REF(std::string, name);
  PROP_CONST_REF(std::string, description);
  PROP_CONST_REF(time::time, created);
  PROP_CONST_REF(time::time, deleted);
  PROP_CONST_REF(time::time, completed);
  PROP_CONST_REF(time::time, lastmodified);
  PROP(int, overlap);
  PROP(int, min_overlap_volume);
  friend struct YAML::convert<om::task::Dataset>;
};
}
}  // namespace om::task::

namespace YAML {

template <>
struct convert<om::task::Dataset> {
  static Node encode(const om::task::Dataset& c) {
    Node node;
    node["id"] = c.id();
    node["name"] = c.name();
    node["description"] = c.description();
    node["created"] = c.created();
    node["deleted"] = c.deleted();
    node["completed"] = c.completed();
    node["lastmodified"] = c.lastmodified();
    node["overlap"] = c.overlap();
    node["min_overlap_volume"] = c.min_overlap_volume();
    return node;
  }

  static bool decode(const Node& node, om::task::Dataset& c) {
    try {

      c.id_ = node["id"].as<int>();
      c.name_ = node["name"].as<std::string>();
      c.description_ = node["description"].as<std::string>();
      // c.created_ = node["created"].as<om::time::time>();
      // c.deleted_ = node["deleted"].as<om::time::time>();
      // c.completed_ = node["completed"].as<om::time::time>();
      // c.lastmodified_ = node["lastmodified"].as<om::time::time>();
      c.overlap_ = node["overlap"].as<int>();
      c.min_overlap_volume_ = node["min_overlap_volume"].as<int>();
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs << std::string("Error Decoding Dataset: ") + e.what();
      return false;
    }
  }
};
}