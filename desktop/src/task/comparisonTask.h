#pragma once

#include <unordered_map>
#include "task/task.h"
#include "common/common.h"
#include "chunk/dataSources.hpp"
#include "yaml-cpp/yaml.h"
#include "utility/yaml/baseTypes.hpp"
#include "users/omUsers.h"
#include "network/http/http.hpp"

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

class ComparisonTask : virtual public Task,
                       virtual public network::IHTTPRefreshable {
 public:
  ComparisonTask();
  ComparisonTask(uint32_t id, uint32_t cellId, const std::string& path,
                 std::vector<SegGroup>&& namedGroups);
  virtual ~ComparisonTask();

  virtual int Id() { return data_.id; }
  virtual int CellId() { return data_.cellId; }
  virtual bool Reaping() { return false; }
  virtual bool Start();
  virtual bool Submit();
  virtual const std::vector<SegGroup>& SegGroups() { return data_.namedGroups; }
  virtual void Refresh(const std::string& data) {
    if (!data.size()) {
      return;
    }

    try {
      auto node = YAML::Load(data);
      data_ = node.as<Data>();
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed loading JSON: " << e.what();
    }
  }

  struct Data {
    uint32_t id;
    uint32_t cellId;
    std::string path;
    std::vector<SegGroup> namedGroups;
  };

 private:
  static bool chunkHasUserSegments(
      OmChunkUniqueValuesManager& uniqueValuesDS,
      const om::coords::Chunk& chunk,
      const std::unordered_map<common::SegID, int>& segFlags);

  Data data_;
};

}  // namespace om::task::
}  // namespace om::

namespace YAML {

template <>
struct convert<om::task::ComparisonTask::Data> {
  static bool decode(const Node& node, om::task::ComparisonTask::Data& t) {
    try {
      t.id = node["id"].as<uint32_t>();
      t.cellId = node["cell"].as<uint32_t>();
      t.path = node["data"]["channel"]["metadata"]["uri"].as<std::string>();
      if (t.path.compare(t.path.size() - 7 - 1, std::string::npos, ".files/")) {
        t.path = t.path.substr(0, t.path.size() - 7);
      }

      auto groups = node["groups"];
      t.namedGroups.clear();
      for (const auto& group : groups) {
        std::string name;
        om::common::SegIDSet segs;
        auto type = (om::task::SegGroup::GroupType)group["type"].as<int>();
        switch (type) {
          case om::task::SegGroup::GroupType::ALL:
            name = "All";
            break;
          case om::task::SegGroup::GroupType::AGREED:
            name = "Agreed";
            break;
          case om::task::SegGroup::GroupType::USER:
            name = group["username"].as<std::string>(om::users::defaultUser);
            break;
          case om::task::SegGroup::GroupType::DUST:
            name = "Dust";
            break;
          case om::task::SegGroup::GroupType::PARTIAL:
            name = "Partial";
            break;
        }
        for (const auto& g : group["groups"]) {
          om::task::SegGroup sg;
          sg.name = name;
          sg.type = type;
          sg.segments = g.as<std::set<uint32_t>>(std::set<uint32_t>());
          t.namedGroups.push_back(std::move(sg));
        }
      }

      om::task::SegGroup seed;
      seed.name = "seed";
      seed.type = om::task::SegGroup::GroupType::SEED;
      for (const auto& s : node["prior"]["segments"]) {
        seed.segments.insert(s.first.as<uint32_t>());
      }
      t.namedGroups.push_back(seed);
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs << std::string("Error Decoding ComparisonTask: ") + e.what();
      return false;
    }
  }
};
}