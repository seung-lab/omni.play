#pragma once

#include "common/common.h"
#include <zi/mutex.hpp>
#include "segment/dataSources.hpp"

#include <unordered_map>

class OmSegment;
class OmSegments;

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

  // FIXME: this is broken for adding new segments
  // segments MUST be paged w/ fixed page sizes
  // else, when adding new segments, a vector resize operation
  // could occur, and the addresses of the segments could change!
  std::vector<OmSegment> segments_;

  zi::spinlock pagesLock_;
};
}
}  // namespace om::segment::
