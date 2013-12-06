#pragma once

#include "segment/lists/omSegmentListsTypes.hpp"

class OmSegmentListGlobal {
 private:
  const std::vector<SegInfo> list_;

 public:
  OmSegmentListGlobal() {}

  OmSegmentListGlobal(const std::vector<SegInfo>& list) : list_(list) {}

  inline int64_t GetSizeWithChildren(const om::common::SegID segID) {
    if (segID >= list_.size()) {
      log_infos << "segment " << segID << "not found\n";
      return 0;
    }
    return list_[segID].sizeIncludingChildren;
  }

  inline int64_t GetSizeWithChildren(OmSegment* seg) {
    return GetSizeWithChildren(seg->value());
  }

  inline int64_t GetNumChildren(const om::common::SegID segID) {
    if (segID >= list_.size()) {
      log_infos << "segment " << segID << "not found\n";
      return 0;
    }
    return list_[segID].numChildren;
  }

  inline int64_t GetNumChildren(OmSegment* seg) {
    return GetNumChildren(seg->value());
  }

  boost::optional<SegInfo> Get(const om::common::SegID segID) {
    if (segID >= list_.size()) {
      log_infos << "segment " << segID << "not found\n";
      return boost::optional<SegInfo>();
    }
    return boost::optional<SegInfo>(list_[segID]);
  }
};
