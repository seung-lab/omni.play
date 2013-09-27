#pragma once

#include "common/common.h"
#include "common/macro.hpp"
#include "utility/color.hpp"

class OmGroup {
 public:
  OmGroup() {}

  OmGroup(om::common::ID id) : id_(id) {
    mColor = om::utils::color::GetRandomColor();
  }

  OmGroup(const om::common::SegIDSet& ids) {
    mColor = om::utils::color::GetRandomColor();
    AddIds(ids);
  }

  virtual ~OmGroup() {}

  void AddIds(const om::common::SegIDSet& ids) {
    for (auto id : ids) {
      mIDs.insert(id);
    }
  }

  void RemoveIds(const om::common::SegIDSet& ids) {
    for (auto id : ids) {
      mIDs.erase(id);
    }
  }

  om::common::GroupName GetName() const { return mName; }

  const om::common::SegIDSet& ids() const { return mIDs; }

 private:
  om::common::SegIDSet mIDs;
  om::common::Color mColor;

  om::common::GroupName mName;

  PROP_REF_SET(om::common::ID, id);
};
