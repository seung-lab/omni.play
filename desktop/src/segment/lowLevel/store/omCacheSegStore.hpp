#pragma once

#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/io/omSegmentPage.hpp"

class OmCacheSegStore {
 private:
  OmSegmentsStore* const store_;
  const uint32_t pageSize_;

  const std::vector<OmSegmentPage*> initialPages_;
  const uint32_t numInitialPages_;

 public:
  OmCacheSegStore(OmSegmentsStore* store)
      : store_(store),
        pageSize_(store->PageSize()),
        initialPages_(store->Pages()),
        numInitialPages_(initialPages_.size()) {}

  /**
   * returns NULL if segment was never instantiated;
   **/
  OmSegment* GetSegment(const om::common::SegID segID) {
    return getSegment(segID, true);
  }

  OmSegment* GetSegmentUnsafe(const om::common::SegID segID) {
    return getSegment(segID, false);
  }

 private:
  inline OmSegment* getSegment(const om::common::SegID segID,
                               const bool isSafe) {
    const uint32_t pageNum = segID / pageSize_;

    if (pageNum < numInitialPages_) {
      // no locking needed--initialPages_ will never change
      return doGetSegment(initialPages_, pageNum, segID, isSafe);

    } else {

      // TODO: keep a copy of this vector as a member variable; some freshness
      // value,
      //   though, could need to be kept by OmSegmentStore to avoid repeated
      // copy;
      //   comparing vector sizes is NOT sufficient, as segments can be added in
      // any order,
      //   thus there may be NULL pages in the copy of the vector that don't get
      // updated
      //   when initialized in the sgemtn store
      const std::vector<OmSegmentPage*> curPages = store_->Pages();

      return doGetSegment(curPages, pageNum, segID, isSafe);
    }
  }

  inline OmSegment* doGetSegment(const std::vector<OmSegmentPage*>& pages,
                                 const uint32_t pageNum,
                                 const om::common::SegID segID,
                                 const bool /* isSafe */) {
    // if(om::NOT_SAFE == isSafe)
    // {
    //     OmSegmentPage& page = *pages[pageNum];
    //     return &(page[segID % pageSize_]);
    // }

    if (pageNum >= pages.size()) {
      return NULL;
    }

    if (!pages[pageNum]) {
      return NULL;
    }

    OmSegmentPage& page = *pages[pageNum];
    OmSegment* ret = &(page[segID % pageSize_]);

    const om::common::SegID loadedID = ret->data_->value;

    if (!loadedID) {
      return NULL;
    }

    if (loadedID != segID) {
      throw om::IoException("corruption detected in segment page");
    }

    return ret;
  }
};
