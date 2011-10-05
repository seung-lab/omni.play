#include "segment/lists/omSegmentLists.h"
#include "segment/lists/omSegmentListByMRU.hpp"

void OmSegmentListByMRU::rebuildList()
{
    std::vector<OmSegment*> segs;
    lru_.Dump(segs);

    vec_.clear();
    vec_.reserve(segs.size());

    FOR_EACH(iter, segs)
    {
        OmSegment* seg = *iter;

        const SegInfo info =
            { seg,
              seg->value(),
              segmentLists_->GetSizeWithChildren(seg),
              segmentLists_->GetNumChildren(seg)
            };

        vec_.push_back(info);
    }

    std::reverse(vec_.begin(), vec_.end());

    dirty_ = false;
}
