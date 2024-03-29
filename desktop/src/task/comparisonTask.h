#pragma once
#include "precomp.h"

#include "task/task.h"
#include "common/common.h"
#include "chunk/dataSources.hpp"
#include "yaml-cpp/yaml.h"
#include "utility/yaml/baseTypes.hpp"
#include "users/omUsers.h"

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

  virtual uint32_t Id() const override { return id_; }
  virtual uint32_t CellId() const override { return cellId_; }
  virtual std::string Notes() const override { return notes_; }
  virtual bool Reaping() const override { return false; }
  virtual bool Start() override;
  virtual bool Submit() override;
  virtual const std::vector<SegGroup>& SegGroups() const override {
    return namedGroups_;
  }
  virtual uint32_t ParentID() const { return parentID_; }
  virtual uint32_t Status() const { return status_; }
  virtual uint32_t Weight() const { return weight_; }
  virtual uint32_t WeightSum() const { return weightSum_; }
  virtual std::string Users() const { return users_; }

 private:
  static bool chunkHasUserSegments(
      OmChunkUniqueValuesManager& uniqueValuesDS,
      const om::coords::Chunk& chunk,
      const std::unordered_map<common::SegID, int>& segFlags);

  uint32_t id_;
  uint32_t parentID_;
  uint32_t status_;
  uint32_t weight_;
  uint32_t weightSum_;
  uint32_t cellId_;
  std::string path_;
  std::string users_;
  std::string notes_;
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
      t.parentID_ = node["parent"].as<uint32_t>(0);
      t.status_ = node["status"].as<uint32_t>(0);
      t.weight_ = node["prior"]["weight"].as<uint32_t>(0);
      t.weightSum_ = node["weightSum"].as<uint32_t>(0);
      t.cellId_ = node["cell"].as<uint32_t>();
      t.path_ = node["data"]["channel"]["metadata"]["uri"].as<std::string>();
      if (t.path_.compare(t.path_.size() - 7 - 1, std::string::npos,
                          ".files/")) {
        t.path_ = t.path_.substr(0, t.path_.size() - 7);
      }
      t.users_ = node["users"].as<std::string>("");
      t.notes_ = node["wiki_notes"].as<std::string>("");

      auto groups = node["groups"];
      t.namedGroups_.clear();
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
          case om::task::SegGroup::GroupType::DUST:
            name = "Dust";
            break;
          case om::task::SegGroup::GroupType::USER_MISSED:
            name = group["username"].as<std::string>(om::users::defaultUser);
            break;
          case om::task::SegGroup::GroupType::USER_FOUND:
            name = std::string("only ") +
                   group["username"].as<std::string>(om::users::defaultUser);
            break;
          case om::task::SegGroup::GroupType::PARTIAL:
            name = "Partial";
            break;
        }
        for (const auto& g : group["groups"]) {
          om::task::SegGroup sg;
          sg.name = name;
          sg.type = type;

          if (g.IsMap()) {
            sg.dust = g["dust"].as<bool>(false);
            if (sg.dust) {
              sg.name += " dust";
            }

            sg.segments =
                g["segments"].as<std::set<uint32_t>>(std::set<uint32_t>());
            sg.size = g["size"].as<size_t>(0);
          } else if (g.IsSequence()) {
            sg.dust = sg.type == om::task::SegGroup::GroupType::DUST;
            sg.segments = g.as<std::set<uint32_t>>(std::set<uint32_t>());
            sg.size = 0;
          } else {
            log_errors << "Invalid Comparison Task";
            continue;
          }

          t.namedGroups_.push_back(std::move(sg));
        }
      }

      om::task::SegGroup seed;
      seed.name = "Seed";
      seed.type = om::task::SegGroup::GroupType::SEED;
      seed.size = 0;
      for (const auto& s : node["prior"]["segments"]) {
        seed.segments.insert(s.first.as<uint32_t>());
      }
      t.namedGroups_.push_back(seed);
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs << std::string("Error Decoding ComparisonTask: ") + e.what();
      return false;
    }
  }
};
}