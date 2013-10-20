#pragma once

struct DummyGroup : public OmManageableObject {
  om::common::SegIDSet mIDs;
  om::common::Color mColor;
  QString mName;
  friend class DummyGroups;
  friend QDataStream& operator>>(QDataStream& in, DummyGroup& g);
};

struct DummyGroups {
  // dummy sink
  OmGenericManager<DummyGroup> mGroupManager;
  QHash<QString, uint32_t> mGroupsByName;
  friend QDataStream& operator>>(QDataStream& in, DummyGroups&);
};

