#pragma once

#include "common/common.h"
#include "zi/omMutex.h"
#include "datalayer/archive/segmentation.h"

class QDataStream;
class OmCacheSegRootIDs;
class OmCacheSegStore;
class OmPagingPtrStore;
class OmSegment;
class OmSegmentPage;
class OmSegmentation;
class OmSegmentsImpl;

class OmSegmentsStore {
private:
    OmSegmentation *const segmentation_;

    const std::unique_ptr<OmPagingPtrStore> segmentPagesPtr_;
    OmPagingPtrStore *const segmentPages_;
    zi::spinlock pagesLock_;

    std::unique_ptr<OmCacheSegStore> cachedStore_;
    std::unique_ptr<OmCacheSegRootIDs> cacheRootIDs_;

public:
    OmSegmentsStore(OmSegmentation* segmentation);
    ~OmSegmentsStore();

// pages
    uint32_t PageSize();
    uint32_t NumPages();
    std::vector<OmSegmentPage*> Pages();
    void Flush();

// segments
    OmSegment* AddSegment(const om::common::SegID value);
    bool IsSegmentValid(const om::common::SegID value);

// caching
    void StartCaches();
    OmSegment* GetSegment(const om::common::SegID value);
    OmSegment* GetSegmentUnsafe(const om::common::SegID value);

    // WARNING: do not call from inside OmSegmentsImpl or OmSegmentsImplLowLevel
    om::common::SegID Root(const om::common::SegID segID);

private:
    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const OmSegmentsImpl&);
    friend void YAML::operator>>(const YAML::Node& in, OmSegmentsImpl&);
    friend QDataStream &operator<<(QDataStream& out, const OmSegmentsImpl&);
    friend QDataStream &operator>>(QDataStream& in, OmSegmentsImpl&);
};

