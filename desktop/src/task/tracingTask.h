#pragma once

#include "task/task.h"
#include "common/common.h"
#include "yaml-cpp/yaml.h"
#include "network/http/httpRefreshable.hpp"

namespace YAML {
template <typename>
class convert;
}

namespace om {
namespace task {

struct TracingTaskData {
  TracingTaskData() : id(0), cellId(0) {}
  uint32_t id;
  uint32_t cellId;
  std::string path;
  common::SegIDSet seed;
  std::vector<SegGroup> groups;
};

class TracingTask : virtual public Task,
                    virtual public network::HTTPRefreshable<TracingTaskData> {
 public:
  TracingTask();
  virtual ~TracingTask();

  virtual int Id() { return data_.id; }
  virtual int CellId() { return data_.cellId; }
  virtual bool Reaping() { return false; }
  virtual bool Start();
  virtual bool Submit();
  virtual const std::vector<SegGroup>& SegGroups() { return data_.groups; }
};

}  // namespace om::task::
}  // namespace om::

namespace YAML {

template <>
struct convert<om::task::TracingTaskData> {
  static bool decode(const Node& node, om::task::TracingTaskData& t) {
    try {
      t.id = node["id"].as<uint32_t>();
      t.cellId = node["cell"].as<uint32_t>();
      t.path = node["data"]["channel"]["metadata"]["uri"].as<std::string>();
      if (t.path.compare(t.path.size() - 7 - 1, std::string::npos, ".files/")) {
        t.path = t.path.substr(0, t.path.size() - 7);
      }

      auto seedYaml = node["prior"]["segments"];
      t.seed.clear();
      for (const auto& s : seedYaml) {
        t.seed.insert(s.first.as<uint32_t>());
      }
      om::task::SegGroup seed;
      seed.name = "seed";
      seed.type = om::task::SegGroup::GroupType::SEED;
      seed.segments = t.seed;
      t.groups.push_back(seed);
      return true;
    }
    catch (std::exception e) {
      log_debugs << std::string("Error Decoding TracingTask: ") + e.what();
      return false;
    }
  }
};
}