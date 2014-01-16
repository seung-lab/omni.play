#pragma once

#include <unordered_map>
#include "task/task.h"
#include "common/common.h"
#include "chunk/dataSources.hpp"
#include "yaml-cpp/yaml.h"
#include "utility/yaml/baseTypes.hpp"

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
  ComparisonTask() : id_(0), cellId_(0) {};
  ComparisonTask(uint32_t id, uint32_t cellId, const std::string& path,
                 std::vector<SegGroup>&& namedGroups);
  virtual ~ComparisonTask();

  virtual int Id() { return id_; }
  virtual int CellId() { return cellId_; }
  virtual bool Reaping() { return false; }
  virtual bool Start();
  virtual bool Submit();
  virtual const std::vector<SegGroup>& SegGroups() { return namedGroups_; }

 private:
  static bool chunkHasUserSegments(
      OmChunkUniqueValuesManager& uniqueValuesDS,
      const om::coords::Chunk& chunk,
      const std::unordered_map<common::SegID, int>& segFlags);

  uint32_t id_;
  uint32_t cellId_;
  std::string path_;
  std::vector<SegGroup> namedGroups_;

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

      auto groups = node["groups"];
      t.namedGroups_.clear();
      for (const auto& group : groups) {
        om::common::SegIDSet segs;
        auto name = group["username"].as<std::string>();
        auto type = (om::task::SegGroup::GroupType)group["type"].as<int>();
        int index = 1;
        for (const auto& g : group["groups"]) {
          om::task::SegGroup sg;
          sg.name = name + " : " + std::to_string(index++);
          sg.type = type;
          sg.segments = g.as<std::set<uint32_t>>();
          t.namedGroups_.push_back(std::move(sg));
        }
      }

      om::task::SegGroup seed;
      seed.name = "seed";
      seed.type = om::task::SegGroup::GroupType::SEED;
      for (const auto& s : node["prior"]["segments"]) {
        seed.segments.insert(s.first.as<uint32_t>());
      }
      t.namedGroups_.push_back(seed);
      return true;
    }
    catch (std::exception e) {
      log_debugs << std::string("Error Decoding ComparisonTask: ") + e.what();
      return false;
    }
  }
};
}