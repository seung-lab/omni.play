#pragma once

#include "common/common.h"

class OmSegmentPage;
class OmSimpleProgress;
class OmSegmentation;

namespace om {
namespace segmentation {
class loader;
}
}

class OmPagingPtrStore {
 public:
  OmPagingPtrStore(OmSegmentation*);

  ~OmPagingPtrStore();

  void Flush();

  uint32_t PageSize() { return pageSize_; }

  OmSegment* AddSegment(const om::common::SegID value);

  std::vector<OmSegmentPage*> Pages() { return pages_; }

  uint32_t NumPages() { return pages_.size(); }

  const std::set<om::common::PageNum> ValidPageNums() const {
    return validPageNums_;
  }

  OmSegmentation* Vol() const { return vol_; }

 private:
  OmSegmentation* const vol_;

  uint32_t pageSize_;
  std::vector<OmSegmentPage*> pages_;

  std::set<om::common::PageNum> validPageNums_;

  void loadAllSegmentPages();
  void resizeVectorIfNeeded(const om::common::PageNum pageNum);

  QString metadataPathQStr();
  void loadMetadata();
  void storeMetadata();

  void loadPage(const om::common::PageNum page, OmSimpleProgress* prog);

  friend class om::segmentation::loader;
};
