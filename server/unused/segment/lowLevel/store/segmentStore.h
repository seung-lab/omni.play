#pragma once

#include "common/common.h"
#include "zi/mutex.h"
#include "datalayer/archive/segmentation.h"

namespace om {
namespace volume { class segmentation; }

namespace segment {

class pagingPtrStore;
class segment;
class page;
class segmentsImpl;

class segmentsStore {
private:
    volume::segmentation *const segmentation_;

    const boost::scoped_ptr<pagingPtrStore> segmentPagesPtr_;
    pagingPtrStore *const segmentPages_;
    zi::spinlock pagesLock_;

public:
    segmentsStore(volume::segmentation* segmentation);
    ~segmentsStore();

// pages
    uint32_t PageSize();
    uint32_t NumPages();
    std::vector<page*> Pages();
    void Flush();

// segments
    segment AddSegment(const common::segId value);
    bool IsSegmentValid(const common::segId value);

// caching
//    segment* GetSegment(const common::segId value);
    segment GetSegmentUnsafe(const common::segId value);

private:
//    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segmentsImpl&);
//    friend void YAML::operator>>(const YAML::Node& in, segmentsImpl&);
};

} // namespace segment
} // namespace om
