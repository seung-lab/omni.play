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
  for (int i = 1; i < data_.size(); ++i) {
    segments_[i] = OmSegment(data_[i], listData_[i], system_);
  }
}

/**
 * if requested segID is not 0 and is within the range of our store,
 * fetch the stored segment and return it only if it is valid
 * and the segment's segid matches the requested one
 **/
OmSegment* Store::GetSegment(const common::SegID segID) const {
  if (segID && segID < data_.size()) {
    OmSegment* seg = const_cast<OmSegment*>(&segments_[segID]);
    if (seg && segID == seg->value()) {
      return seg;
    }
  }

  log_debugs << "Invalid segid " << segID;
  return nullptr;
}

OmSegment* Store::AddSegment(const common::SegID value)  {
  zi::guard g(pagesLock_);
  if ( value >= data_.size()){
    resize(value+1);
  }
  segments_[value].SetValue(value);

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
  for(size_t i = oldSize ; i < data_.size(); ++i){
    segments_[i] = OmSegment(data_[i], listData_[i], system_);
  }
}

void Store::Flush() {
  zi::guard g(pagesLock_);
  data_.flush();
  listData_.flush();
}

bool Store::IsSegmentValid(const common::SegID value) {
  return GetSegment(value);

}
}
namespace data {
template class dynarray<
    OmSegment, MemPagedStoragePolicy<OmSegment, DEFAULT_MEM_PAGE_SIZE>>;
;
}
}
