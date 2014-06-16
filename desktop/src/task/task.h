#pragma once
#include "precomp.h"

#include "common/common.h"

namespace om {
namespace task {

struct SegGroup {
  enum class GroupType {
    SEED = 0,
    ALL = 1,
    AGREED = 2,
    USER_FOUND = 3,
    DUST = 4,
    PARTIAL = 5,
    USER_MISSED = 6
  };
  std::string name;
  GroupType type;
  common::SegIDSet segments;
};

class Task {
 public:
  virtual ~Task() {}
  virtual int Id() const = 0;
  virtual int CellId() const = 0;
  virtual bool Reaping() const = 0;
  virtual bool Start() = 0;
  virtual bool Submit() = 0;
  virtual const std::vector<SegGroup>& SegGroups() const = 0;
};

}  // namespace om::task::
}  // namespace om::
