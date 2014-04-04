#pragma once
#include "precomp.h"

#include "common/common.h"

namespace om {
namespace task {

struct TaskInfo {
  uint32_t id;
  uint32_t weight;
  uint32_t inspected_weight;
  std::string path;
  uint32_t cell;
  uint32_t depth;
  uint32_t left_edge;
  uint32_t right_edge;
  int status;
  std::vector<std::string> users;
  int parent;
  std::string notes;
};

}  // namespace om::task::
}  // namespace om::

namespace YAML {

template <>
struct convert<om::task::TaskInfo> {
  static bool decode(const Node& node, om::task::TaskInfo& t) {
    try {
      t.id = node["id"].as<uint32_t>();
      t.weight = node["weightsum"].as<uint32_t>();
      t.inspected_weight = node["inspected_weight"].as<uint32_t>();
      t.path = node["segmentation_path"].as<std::string>();
      t.cell = node["cell"].as<uint32_t>();
      t.depth = node["depth"].as<uint32_t>();
      t.left_edge = node["left_edge"].as<uint32_t>();
      t.right_edge = node["right_edge"].as<uint32_t>();
      t.status = node["status"].as<int>();
      typedef std::vector<std::string> string_list;
      t.users = node["users"].as<string_list>(string_list());
      t.parent = node["parent"].as<int>(0);
      t.notes = node["notes"].as<std::string>("");
    }
    catch (std::exception e) {
      log_debugs << std::string("Error Decoding TaskInfo: ") + e.what();
      return false;
    }
    return true;
  }
};
}