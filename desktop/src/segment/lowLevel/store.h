#pragma once

#include "common/common.h"
#include "segment/dataSources.hpp"
#include "container/pagedVector.hpp"

class OmSegment;

namespace om {
namespace segment {

class Store {
 public:
  Store(SegDataVector& data, SegListDataVector& listData,
        const coords::VolumeSystem& system, common::SegID maxSegID);

  OmSegment* GetSegment(const common::SegID value) const;
  OmSegment* AddSegment(const common::SegID value);
  bool IsSegmentValid(const common::SegID value);

  void Flush();

 private:
  SegDataVector& data_;
  SegListDataVector& listData_;
  const coords::VolumeSystem& system_;

  data::paged_dynarray<OmSegment> segments_;

  zi::spinlock pagesLock_;
};
}
}  // namespace om::segment::
