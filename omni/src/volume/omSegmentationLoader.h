#pragma once

#include "common/omCommon.h"

class OmSegmentation;
class OmPagingPtrStore;

#include <QSet>

namespace om {
namespace segmentation {

class loader {
private:
    OmSegmentation *const vol_;

public:
    loader(OmSegmentation* vol)
        : vol_(vol)
    {}

    void LoadSegmentPages(OmPagingPtrStore& ps,
                          QSet<PageNum>& validPageNumbers,
                          uint32_t size);

private:
    void rebuildSegments();
};

} // namespace segmentation
} // namespace om
