#pragma once

#include <map>
#include "common/common.h"

namespace om {
namespace task {

struct SegGroup {
  enum class GroupType {
    SEED = 0,
    ALL = 1,
    AGREED = 2,
    USER = 3,
    DUST = 4,
  };
  std::string name;
  GroupType type;
  common::SegIDSet segments;
};

class Task {
 public:
  virtual ~Task() {}
  virtual int Id() = 0;
  virtual int CellId() = 0;
  virtual bool Reaping() = 0;
  virtual bool Start() = 0;
  virtual bool Submit() = 0;
  virtual const std::vector<SegGroup>& SegGroups() = 0;
};

}  // namespace om::task::
}  // namespace om::
