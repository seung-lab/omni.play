#include "segment/omSegment.h"
#include "segment/lowLevel/store.h"
#include "segment/omSegment.h"
#include "utility/malloc.hpp"

namespace om {
namespace segment {

Store::Store(SegDataVector& data, SegListDataVector& listData,
             const coords::VolumeSystem& system)
    : data_(data), listData_(listData), system_(system) {

  segments_.resize(data.size());
  for (int i = 0; i < data_.size(); ++i) {
    segments_[i] = OmSegment(data_[i], listData_[i], system_);
  }
}

OmSegment* Store::GetSegment(const common::SegID value) const {
  if (value >= data_.size()) {
    log_debugs << "Invalid segid " << value;
    return nullptr;
  }

  return const_cast<OmSegment*>(&segments_[value]);
}

OmSegment* Store::AddSegment(const common::SegID value) {
  zi::guard g(pagesLock_);

  if ( value+1 > data_.size()){
    resize(value+1);
  }
  data_[value].value = value;
  segments_[value] = OmSegment(data_[value], listData_[value], system_);

  return &segments_[value];
}
void Store::resize(size_t newSize)
{
  size_t oldSize = data_.size();

  Data nullSeg;
  nullSeg.value = 0;
  nullSeg.size = 0;
  data_.resize(newSize, nullSeg);
  listData_.resize(newSize);
  segments_.resize(newSize);

  for(size_t i = oldSize; i < data_.size(); ++i){
    segments_[i] = OmSegment(data_[i], listData_[i], system_);
  }
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
namespace data {
template class dynarray<
    OmSegment, MemPagedStoragePolicy<OmSegment, DEFAULT_MEM_PAGE_SIZE>>;
;
}
}
