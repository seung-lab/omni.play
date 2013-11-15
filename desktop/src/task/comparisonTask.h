#pragma once

#include <unordered_map>
#include "task.h"
#include "common/common.h"
#include "chunk/dataSources.hpp"
#include "yaml-cpp/yaml.h"

namespace YAML {
template <typename>
class convert;
}

namespace om {
class chunkCoord;
}

class OmChunkUniqueValuesManager;

namespace om {
namespace task {

class ComparisonTask : virtual public Task {
 public:
  // typedef std::unordered_map<common::SegID, int> SegFlagMap;
  // typedef common::SegIDSet SegFlagMap;
  typedef std::set<common::SegIDSet> UserSegContainer;
  ComparisonTask() : id_(0), cellId_(0) {};
  ComparisonTask(uint32_t id, uint32_t cellId, const std::string& path,
                 UserSegContainer&& userSegs);
  virtual ~ComparisonTask();

  virtual int Id() { return id_; }
  virtual int CellId() { return cellId_; }
  virtual bool Reaping() { return false; }
  virtual bool Start();
  virtual bool Submit();

 private:
  static bool chunkHasUserSegments(
      OmChunkUniqueValuesManager& uniqueValuesDS, const om::chunkCoord& chunk,
      const std::unordered_map<common::SegID, int>& segFlags);

  uint32_t id_;
  uint32_t cellId_;
  std::string path_;
  UserSegContainer userSegs_;

  friend class YAML::convert<ComparisonTask>;
};

}  // namespace om::task::
}  // namespace om::

namespace YAML {

template <>
struct convert<om::task::ComparisonTask> {
  static bool decode(const Node& node, om::task::ComparisonTask& t) {
    try {
      t.id_ = node["id"].as<uint32_t>();
      t.cellId_ = node["cell"].as<uint32_t>();
      t.path_ = node["data"]["channel"]["metadata"]["uri"].as<std::string>();
      if (t.path_.compare(t.path_.size() - 7 - 1, std::string::npos,
                          ".files/")) {
        t.path_ = t.path_.substr(0, t.path_.size() - 7);
      }

      auto validations = node["validations"];
      t.userSegs_.clear();
      for (const auto& val : validations) {
        om::common::SegIDSet segs;
        for (const auto& s : val["segments"]) {
          segs.insert(s.first.as<uint32_t>());
        }
        t.userSegs_.insert(std::move(segs));
      }
      return true;
    }
    catch (std::exception e) {
      log_debugs(std::string("Error Decoding ComparisonTask: ") + e.what());
      return false;
    }
  }
};
}