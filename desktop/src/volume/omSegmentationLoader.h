#pragma once

#include "common/common.h"

class OmSegmentation;
class OmPagingPtrStore;

#include <QSet>

namespace om {
namespace segmentation {

class loader {
 private:
  OmSegmentation* const vol_;

 public:
  loader(OmSegmentation* vol) : vol_(vol) {}

  void LoadSegmentPages(OmPagingPtrStore& ps,
                        QSet<om::common::PageNum>& validPageNumbers,
                        uint32_t size);

  void LoadSegmentPages(OmPagingPtrStore& ps);

 private:
  void rebuildSegments();
};

}  // namespace segmentation
}  // namespace om
