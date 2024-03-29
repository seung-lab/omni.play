#pragma once
#include "precomp.h"

#include "common/common.h"

namespace om {
namespace task {

struct TaskInfo {
  uint32_t id;
  float weight;
  uint32_t inspected_weight;
  std::string path;
  uint32_t cell;
  uint32_t depth;
  uint32_t left_edge;
  uint32_t right_edge;
  uint32_t status;
  std::string users;
  uint32_t parent;
  std::string notes;
  uint32_t allSize;
  uint32_t agreedSize;
  double agreement() const {
    return allSize ? (double)agreedSize / (double)allSize : 0;
  }
};

}  // namespace om::task::
}  // namespace om::

namespace YAML {

template <>
struct convert<om::task::TaskInfo> {
  static bool decode(const Node& node, om::task::TaskInfo& t) {
    try {
      t.id = node["id"].as<uint32_t>();
      t.weight = node["weightsum"].as<float>();
      t.inspected_weight = node["inspected_weight"].as<uint32_t>();
      t.path = node["segmentation_path"].as<std::string>();
      t.cell = node["cell"].as<uint32_t>();
      t.depth = node["depth"].as<uint32_t>();
      t.left_edge = node["left_edge"].as<uint32_t>();
      t.right_edge = node["right_edge"].as<uint32_t>();
      t.status = node["status"].as<int>();
      t.users = node["users"].as<std::string>("");
      t.parent = node["parent"].as<int>(0);
      t.notes = node["wiki_notes"].as<std::string>("");
      t.allSize = node["allSize"].as<uint32_t>(0);
      t.agreedSize = node["agreedSize"].as<uint32_t>(0);
    }
    catch (std::exception e) {
      log_debugs << std::string("Error Decoding TaskInfo: ") + e.what();
      return false;
    }
    return true;
  }
};
}