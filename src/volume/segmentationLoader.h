#pragma once

#include "common/common.h"

class segmentation;
class pagingPtrStore;

namespace om {
namespace segmentation {

class loader {
private:
    segmentation *const vol_;

public:
    loader(segmentation* vol)
        : vol_(vol)
    {}

    void LoadSegmentPages(pagingPtrStore& ps,
                          QSet<PageNum>& validPageNumbers,
                          uint32_t size);

    void LoadSegmentPages(pagingPtrStore& ps);

private:
    void rebuildSegments();
};

} // namespace segmentation
} // namespace om
