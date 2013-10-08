#pragma once

#include "common/omCommon.h"
#include "system/omManageableObject.h"
#include "utility/color.hpp"
#include "datalayer/archive/segmentation.h"

class OmGroup : public OmManageableObject {
 public:
  OmGroup() {}

  OmGroup(OmID id) : OmManageableObject(id) {
    mColor = om::utils::color::GetRandomColor();
  }

  OmGroup(const OmSegIDsSet& ids) {
    mColor = om::utils::color::GetRandomColor();
    AddIds(ids);
  }

  virtual ~OmGroup() {}

  void AddIds(const OmSegIDsSet& ids) {
    FOR_EACH(iter, ids) { mIDs.insert(*iter); }
  }

  void RemoveIds(const OmSegIDsSet& ids) {
    FOR_EACH(iter, ids) { mIDs.erase(*iter); }
  }

  OmGroupName GetName() const { return mName; }

  const OmSegIDsSet& GetIDs() const { return mIDs; }

 private:
  OmSegIDsSet mIDs;
  OmColor mColor;

  OmGroupName mName;
  friend class OmGroups;

  friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmGroup& g);
  friend void YAML::operator>>(const YAML::Node& in, OmGroup& g);
  friend QDataStream& operator<<(QDataStream& out, const OmGroup& g);
  friend QDataStream& operator>>(QDataStream& in, OmGroup& g);
};
