#pragma once

#include "common/common.h"
#include "system/omGenericManager.hpp"

#include <unordered_map>

class OmSegmentation;
class OmGroup;

class OmGroups : boost::noncopyable {
 public:
  OmGroups(OmSegmentation* seg);
  ~OmGroups();

  OmGroup& AddGroup(om::common::GroupName);
  OmGroup& GetGroup(om::common::GroupID);
  OmGroup& GetGroup(om::common::GroupName);

  om::common::GroupID idFromName(om::common::GroupName);
  void SetGroup(const om::common::SegIDSet& set, om::common::GroupName name);
  void UnsetGroup(const om::common::SegIDSet& set, om::common::GroupName name);

  om::common::GroupIDSet GetGroups();
  om::common::GroupIDSet GetGroups(om::common::SegID);

  om::common::ID GetSegmentationID();
  void populateGroupsList();

  OmGenericManager<OmGroup> mGroupManager;
  std::unordered_map<om::common::GroupName, om::common::GroupID> mGroupsByName;

 private:
  void setGroupIDs(const om::common::SegIDSet& set, OmGroup* group, bool doSet);

  OmSegmentation* mSegmentation;


};
