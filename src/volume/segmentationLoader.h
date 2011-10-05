#pragma once

#include "common/common.h"

class segmentation;
class OmPagingPtrStore;

#include <QSet>

namespace om {
namespace segmentation {

class loader {
private:
    segmentation *const vol_;

public:
    loader(segmentation* vol)
        : vol_(vol)
    {}

    void LoadSegmentPages(OmPagingPtrStore& ps,
                          QSet<PageNum>& validPageNumbers,
                          uint32_t size);
    
    void LoadSegmentPages(OmPagingPtrStore& ps);

private:
    void rebuildSegments();
};

} // namespace segmentation
} // namespace om
