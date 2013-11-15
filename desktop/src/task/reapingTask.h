#pragma once

#include "task/task.h"
#include "task/aggregate.hpp"
#include "common/common.h"
#include "yaml-cpp/yaml.h"

namespace YAML {
template <typename>
class convert;
}

namespace om {
namespace task {

class ReapingTask : virtual public Task {
 public:
  ReapingTask();
  ReapingTask(uint32_t id, uint32_t cellId, const std::string& path,
              common::SegIDSet&& seed, Aggregate&& aggregate);
  virtual ~ReapingTask();

  virtual int Id() { return id_; }
  virtual int CellId() { return cellId_; }
  virtual bool Reaping() { return false; }
  virtual bool Start();
  virtual bool Submit();

  void set_aggregate(Aggregate&& aggregate) {
    aggregate_ = std::move(aggregate);
  }

 private:
  uint32_t id_;
  uint32_t cellId_;
  std::string path_;
  common::SegIDSet seed_;
  Aggregate aggregate_;
  friend class YAML::convert<ReapingTask>;
};

}  // namespace om::task::
}  // namespace om::

namespace YAML {

template <>
struct convert<om::task::ReapingTask> {
  static bool decode(const Node& node, om::task::ReapingTask& t) {
    try {
      t.id_ = node["id"].as<uint32_t>();
      t.cellId_ = node["cell"].as<uint32_t>();
      t.path_ = node["data"]["channel"]["metadata"]["uri"].as<std::string>();
      if (t.path_.compare(t.path_.size() - 7 - 1, std::string::npos,
                          ".files/")) {
        t.path_ = t.path_.substr(0, t.path_.size() - 7);
      }

      auto seedYaml = node["prior"]["segments"];
      t.seed_.clear();
      for (const auto& s : seedYaml) {
        t.seed_.insert(s.first.as<uint32_t>());
      }
      return true;
    }
    catch (std::exception e) {
      log_debugs(std::string("Error Decoding ReapingTask: ") + e.what());
      return false;
    }
  }
};
}