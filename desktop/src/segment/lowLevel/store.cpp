#include "segment/omSegment.h"
#include "segment/lowLevel/store.h"
#include "segment/omSegment.h"
#include "utility/malloc.hpp"

namespace om {
namespace segment {

Store::Store(SegDataVector& data, SegListDataVector& listData,
             const coords::VolumeSystem& system, common::SegID maxSegID)
    : data_(data), listData_(listData), system_(system) {

  segments_.reserve(data.size());
  for (int i = 0; i < data_.size(); ++i) {
    segments_.emplace_back(data_[i], listData_[i], system_);
  }
}

OmSegment* Store::GetSegment(const common::SegID value) const {
  if (value >= data_.size()) {
    return nullptr;
  }
  return const_cast<OmSegment*>(&segments_[value]);
}

OmSegment* Store::AddSegment(const common::SegID value) {
  zi::guard g(pagesLock_);
  if (value < data_.size()) {
    OmSegment* seg = GetSegment(value);
    if (seg && !seg->value() == value) {
      data_[value].value = value;
    }
    return seg;
  }

  auto oldSize = segments_.size();
  auto newSize = value + 1;  // one indexed.

  data_.resize(newSize);
  data_[value].value = value;

  listData_.resize(newSize);

  segments_.reserve(newSize);
  for (int i = oldSize; i < data_.size(); ++i) {
    segments_.emplace_back(data_[i], listData_[i], system_);
  }
  return GetSegment(value);
}

void Store::Flush() {
  zi::guard g(pagesLock_);
  data_.flush();
  listData_.flush();
}

/**
 * a segment ptr is invalid if it is nullptr, or has an ID of 0
 **/
bool Store::IsSegmentValid(const common::SegID value) {
  if (!value) {
    return false;
  }

  OmSegment* seg = GetSegment(value);

  if (seg == nullptr) {
    return false;
  }

  return value == seg->value();
}
}
}  // namespace om::segment::